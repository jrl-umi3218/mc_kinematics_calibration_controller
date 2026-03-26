#pragma once

#if defined _WIN32 || defined __CYGWIN__
#  define KinematicsCalibrationController_DLLIMPORT __declspec(dllimport)
#  define KinematicsCalibrationController_DLLEXPORT __declspec(dllexport)
#  define KinematicsCalibrationController_DLLLOCAL
#else
// On Linux, for GCC >= 4, tag symbols using GCC extension.
#  if __GNUC__ >= 4
#    define KinematicsCalibrationController_DLLIMPORT __attribute__((visibility("default")))
#    define KinematicsCalibrationController_DLLEXPORT __attribute__((visibility("default")))
#    define KinematicsCalibrationController_DLLLOCAL __attribute__((visibility("hidden")))
#  else
// Otherwise (GCC < 4 or another compiler is used), export everything.
#    define KinematicsCalibrationController_DLLIMPORT
#    define KinematicsCalibrationController_DLLEXPORT
#    define KinematicsCalibrationController_DLLLOCAL
#  endif // __GNUC__ >= 4
#endif // defined _WIN32 || defined __CYGWIN__

#ifdef KinematicsCalibrationController_STATIC
// If one is using the library statically, get rid of
// extra information.
#  define KinematicsCalibrationController_DLLAPI
#  define KinematicsCalibrationController_LOCAL
#else
// Depending on whether one is building or using the
// library define DLLAPI to import or export.
#  ifdef KinematicsCalibrationController_EXPORTS
#    define KinematicsCalibrationController_DLLAPI KinematicsCalibrationController_DLLEXPORT
#  else
#    define KinematicsCalibrationController_DLLAPI KinematicsCalibrationController_DLLIMPORT
#  endif // KinematicsCalibrationController_EXPORTS
#  define KinematicsCalibrationController_LOCAL KinematicsCalibrationController_DLLLOCAL
#endif // KinematicsCalibrationController_STATIC