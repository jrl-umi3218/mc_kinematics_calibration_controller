#include "KinematicsCalibrationController_Initial.h"

#include "../KinematicsCalibrationController.h"

#include <mc_rtc/gui/Label.h>
#include <mc_rtc/io_utils.h>
#include <filesystem>

namespace fs = std::filesystem;

void KinematicsCalibrationController_Initial::configure(const mc_rtc::Configuration & config)
{
}

void KinematicsCalibrationController_Initial::start(mc_control::fsm::Controller & ctl_)
{
  auto & ctl = static_cast<KinematicsCalibrationController &>(ctl_);

  // MainRobot, or robot specified in 'robot' field (or by index)
  robot_ = mc_rbdyn::robotNameFromConfig(config_, ctl.robots());
  mc_rtc::log::info("Using robot {} for KinematicsCalibrationController", robot_);

  // Get default logger path in user's .local/share
  const char * home = std::getenv("HOME");
  fs::path dir;
  if(home)
  {
    dir = fs::path{home} / ".local/share/mc-rtc/controllers";
  }
  else
  {
    dir = fs::path{"/tmp/mc-rtc/controllers"};
  }
  dir = dir / "KinematicsCalibrationController" / robot_;

  // Create async logger
  for(const auto & hole : holes)
  {
    for(const auto & position : positions)
    {
      // dir/robot/position/hole_<num>.csv
      auto loggerName = fmt::format("{}/{}", position, hole);
      auto loggerPath = fmt::format("{}/{}.csv", dir.string(), loggerName);
      mc_rtc::log::info("Creating async logger for hole {} at path {}", loggerName, loggerPath);
      auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(loggerPath, true);
      auto [loggerElem, success] = holeLogger_.emplace(loggerName, std::make_shared<spdlog::logger>(loggerName, sink));
      auto & logger = loggerElem->second;
      logger->set_pattern("%v");
      // #panda_joint1,panda_joint2,panda_joint3,panda_joint4,panda_joint5,panda_joint6,panda_joint7
      logger->info("#{}", mc_rtc::io::to_string(
            ctl.realRobot(robot_).refJointOrder(), ",")
            );
    }
  }


  using namespace mc_rtc::gui;
  ctl.gui()->addElement(this, {},
      Label("Help",
        []{ return "Welcome to the KinematicsCalibrationController"; }
        ),
      ComboInput("Position",
        KinematicsCalibrationController_Initial::positions,
        [this]() { return position_; }, [this](const std::string & position) { position_ = position; }),
      ComboInput("Hole",
        KinematicsCalibrationController_Initial::holes,
        [this]() { return hole_; }, [this](const std::string & hole) { hole_ = hole; }),
      Button("Add data", [this, &ctl]() { addHoleData(ctl); }),
      Button("Save (async)", [this]() { asyncSave(true); }),
      Button("Exit", [this]() { asyncSave(); running_ = false; })
      );
}

bool KinematicsCalibrationController_Initial::run(mc_control::fsm::Controller & ctl_)
{
  auto & ctl = static_cast<KinematicsCalibrationController &>(ctl_);
  output("OK");

  if(++iter_ % saveIterRate_ == 0)
  {
    asyncSave(false);
  }

  return running_;
}

void KinematicsCalibrationController_Initial::teardown(mc_control::fsm::Controller & ctl_)
{
  auto & ctl = static_cast<KinematicsCalibrationController &>(ctl_);
  ctl.gui()->removeElements(this);
  asyncSave(true);
}


void KinematicsCalibrationController_Initial::addHoleData(mc_control::fsm::Controller & ctl)
{
  mc_rtc::log::info("Adding data for hole {}", hole_);
  auto loggerName = fmt::format("{}/{}", position_, hole_);
  if(holeLogger_.find(loggerName) == holeLogger_.end()) { mc_rtc::log::error("Logger {} not found in holeLogger_", loggerName); return; }
  holeLogger_[loggerName]->info("{}", mc_rtc::io::to_string(ctl.realRobot(robot_).encoderValues(), ","));
};

void KinematicsCalibrationController_Initial::asyncSave(bool show)
{
  for(auto & [_, logger] : holeLogger_)
  {
    if(show) { mc_rtc::log::info("Requesting async logger flush {}", logger->name()); }
    logger->flush();
  }
}

EXPORT_SINGLE_STATE("KinematicsCalibrationController_Initial", KinematicsCalibrationController_Initial)
