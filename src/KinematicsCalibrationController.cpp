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

  // Set initial pose by attaching the tables together
  if(robot().hasFrame("Front_exterior") && robot("panda_tibia").hasFrame("Right_interior"))
  {
    auto & panda_femur = robot("panda_femur");
    auto & panda_tibia = robot("panda_tibia");
    auto X_0_pt = panda_tibia.posW();

    auto X_pt_Right_interior = panda_tibia.frame("Right_interior").position() * X_0_pt.inv();
    auto X_Right_interior_Front_exterior = sva::RotZ(mc_rtc::constants::PI / 2);
    auto X_Front_exterior_0_pf = X_0_pt * panda_femur.frame("Front_exterior").position().inv();
    auto X_0_pf = X_Front_exterior_0_pf * X_Right_interior_Front_exterior * X_pt_Right_interior * X_0_pt;

    // Manually compensate for most of the calibration offset
    // however the issue is most likely in the robot's kinematics model itself
    auto calibOffsetWorld = sva::PTransformd::Identity(); 
    // calibOffsetWorld.translation().x() = 0.0;
    // calibOffsetWorld.translation().y() = -0.015;
    // calibOffsetWorld.translation().z() = 0.01;
    X_0_pf = X_0_pf * calibOffsetWorld;

    robot("panda_femur").posW(X_0_pf);
    realRobot("panda_femur").posW(X_0_pf);
    outputRobot("panda_femur").posW(X_0_pf);

    robot("panda_femur_original").posW(X_0_pf);
    realRobot("panda_femur_original").posW(X_0_pf);
    outputRobot("panda_femur_original").posW(X_0_pf);
  }
}


