#include "KinematicsCalibrationController.h"

KinematicsCalibrationController::KinematicsCalibrationController(mc_rbdyn::RobotModulePtr rm, double dt, const mc_rtc::Configuration & config)
: mc_control::fsm::Controller(rm, dt, config,
    mc_control::ControllerParameters{}
      .load_robot_config_into({})
      .load_robot_config_with_module_name(false)
      .overwrite_config(true))
{

  mc_rtc::log::success("KinematicsCalibrationController init done ");
  mc_rtc::log::info("config is:\n{}", config.dump(true, true));
}

bool KinematicsCalibrationController::run()
{
  return mc_control::fsm::Controller::run();
}

void KinematicsCalibrationController::reset(const mc_control::ControllerResetData & reset_data)
{
  mc_control::fsm::Controller::reset(reset_data);
}


