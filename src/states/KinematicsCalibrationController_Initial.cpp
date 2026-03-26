#include "KinematicsCalibrationController_Initial.h"

#include "../KinematicsCalibrationController.h"

void KinematicsCalibrationController_Initial::configure(const mc_rtc::Configuration & config)
{
}

void KinematicsCalibrationController_Initial::start(mc_control::fsm::Controller & ctl_)
{
  auto & ctl = static_cast<KinematicsCalibrationController &>(ctl_);
}

bool KinematicsCalibrationController_Initial::run(mc_control::fsm::Controller & ctl_)
{
  auto & ctl = static_cast<KinematicsCalibrationController &>(ctl_);
  output("OK");
  return true;
}

void KinematicsCalibrationController_Initial::teardown(mc_control::fsm::Controller & ctl_)
{
  auto & ctl = static_cast<KinematicsCalibrationController &>(ctl_);
}

EXPORT_SINGLE_STATE("KinematicsCalibrationController_Initial", KinematicsCalibrationController_Initial)
