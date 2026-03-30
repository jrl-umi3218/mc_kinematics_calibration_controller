#pragma once

#include <mc_control/fsm/State.h>
#include <mc_rtc/logging.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <string_view>

using namespace std::string_view_literals; // Enables sv suffix only


struct KinematicsCalibrationController_Initial : mc_control::fsm::State
{
  struct PerHoleData
  {
    std::vector<double> q;
  };
  using Controller = mc_control::fsm::Controller;

  void configure(const mc_rtc::Configuration & config) override;
  void start(Controller & ctl) override;
  bool run(Controller & ctl) override;
  void teardown(Controller & ctl) override;

  void addHoleData(Controller & ctl);
  void saveAndExit();

  protected:
    bool running_ = true;
    std::string robot_ = "";
    // static constexpr std::array holes = {"hole_0"sv, "hole_1"sv};
    std::vector<std::string> holes {"hole_0", "hole_1"};
    std::vector<std::string> positions {"front_right", "front_center", "front_left", "back_left", "back_right", "center"};
    std::string hole_ = holes.front();
    std::string position_ = positions.front();
    std::map<std::string, std::shared_ptr<spdlog::logger>> holeLogger_;
};
