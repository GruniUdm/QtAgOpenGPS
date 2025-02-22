// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
#ifndef __SETTINGS_KEYS_H__
#define __SETTINGS_KEYS_H__

#include "newsettings.h"

//Generated by generate_settings.py from settings.csv

#define SETTINGS_ab_lineLength "ab/lineLength"
#define SETTINGS_ardMac_hydLowerTime "ardMac/hydLowerTime"
#define SETTINGS_ardMac_hydRaiseTime "ardMac/hydRaiseTime"
#define SETTINGS_ardMac_isDanFoss "ardMac/isDanFoss"
#define SETTINGS_ardMac_isHydEnabled "ardMac/isHydEnabled"
#define SETTINGS_ardMac_setting0 "ardMac/setting0"
#define SETTINGS_ardMac_user1 "ardMac/user1"
#define SETTINGS_ardMac_user2 "ardMac/user2"
#define SETTINGS_ardMac_user3 "ardMac/user3"
#define SETTINGS_ardMac_user4 "ardMac/user4"
#define SETTINGS_ardSteer_maxPulseCounts "ardSteer/maxPulseCounts"
#define SETTINGS_ardSteer_maxSpeed "ardSteer/maxSpeed"
#define SETTINGS_ardSteer_minSpeed "ardSteer/minSpeed"
#define SETTINGS_ardSteer_setting0 "ardSteer/setting0"
#define SETTINGS_ardSteer_setting1 "ardSteer/setting1"
#define SETTINGS_ardSteer_setting2 "ardSteer/setting2"
#define SETTINGS_as_ackerman "as/ackerman"
#define SETTINGS_as_countsPerDegree "as/countsPerDegree"
#define SETTINGS_as_deadZoneDelay "as/deadZoneDelay"
#define SETTINGS_as_deadZoneDistance "as/deadZoneDistance"
#define SETTINGS_as_deadZoneHeading "as/deadZoneHeading"
#define SETTINGS_as_functionSpeedLimit "as/functionSpeedLimit"
#define SETTINGS_as_guidanceLookAheadTime "as/guidanceLookAheadTime"
#define SETTINGS_as_highSteerPWM "as/highSteerPWM"
#define SETTINGS_as_isAutoSteerAutoOn "as/isAutoSteerAutoOn"
#define SETTINGS_as_isConstantContourOn "as/isConstantContourOn"
#define SETTINGS_as_isSteerInReverse "as/isSteerInReverse"
#define SETTINGS_as_Kp "as/Kp"
#define SETTINGS_as_lowSteerPWM "as/lowSteerPWM"
#define SETTINGS_as_maxSteerSpeed "as/maxSteerSpeed"
#define SETTINGS_as_minSteerPWM "as/minSteerPWM"
#define SETTINGS_as_minSteerSpeed "as/minSteerSpeed"
#define SETTINGS_as_modeMultiplierStanley "as/modeMultiplierStanley"
#define SETTINGS_as_modeTime "as/modeTime"
#define SETTINGS_as_modeXTE "as/modeXTE"
#define SETTINGS_as_numGuideLines "as/numGuideLines"
#define SETTINGS_as_sideHillCompensation "as/sideHillCompensation"
#define SETTINGS_as_snapDistance "as/snapDistance"
#define SETTINGS_as_snapDistanceRef "as/snapDistanceRef"
#define SETTINGS_as_uTurnCompensation "as/uTurnCompensation"
#define SETTINGS_as_uTurnSmoothing "as/uTurnSmoothing"
#define SETTINGS_as_wasOffset "as/wasOffset"
#define SETTINGS_bnd_isDrawPivot "bnd/isDrawPivot"
#define SETTINGS_brand_HBrand "brand/HBrand"
#define SETTINGS_brand_TBrand "brand/TBrand"
#define SETTINGS_brand_WDBrand "brand/WDBrand"
#define SETTINGS_cam_camLink "cam/camLink"
#define SETTINGS_color_isMultiColorSections "color/isMultiColorSections"
#define SETTINGS_color_sec01 "color/sec01"
#define SETTINGS_color_sec02 "color/sec02"
#define SETTINGS_color_sec03 "color/sec03"
#define SETTINGS_color_sec04 "color/sec04"
#define SETTINGS_color_sec05 "color/sec05"
#define SETTINGS_color_sec06 "color/sec06"
#define SETTINGS_color_sec07 "color/sec07"
#define SETTINGS_color_sec08 "color/sec08"
#define SETTINGS_color_sec09 "color/sec09"
#define SETTINGS_color_sec10 "color/sec10"
#define SETTINGS_color_sec11 "color/sec11"
#define SETTINGS_color_sec12 "color/sec12"
#define SETTINGS_color_sec13 "color/sec13"
#define SETTINGS_color_sec14 "color/sec14"
#define SETTINGS_color_sec15 "color/sec15"
#define SETTINGS_color_sec16 "color/sec16"
#define SETTINGS_display_antiAliasSamples "display/antiAliasSamples"
#define SETTINGS_display_autoDayNight "display/autoDayNight"
#define SETTINGS_display_brightness "display/brightness"
#define SETTINGS_display_brightnessSystem "display/brightnessSystem"
#define SETTINGS_display_buttonOrder "display/buttonOrder"
#define SETTINGS_display_camPitch "display/camPitch"
#define SETTINGS_display_camSmooth "display/camSmooth"
#define SETTINGS_display_camZoom "display/camZoom"
#define SETTINGS_display_colorDayBackground "display/colorDayBackground"
#define SETTINGS_display_colorDayBorder "display/colorDayBorder"
#define SETTINGS_display_colorDayFrame "display/colorDayFrame"
#define SETTINGS_display_colorFieldDay "display/colorFieldDay"
#define SETTINGS_display_colorFieldNight "display/colorFieldNight"
#define SETTINGS_display_colorNightBackground "display/colorNightBackground"
#define SETTINGS_display_colorNightBorder "display/colorNightBorder"
#define SETTINGS_display_colorNightFrame "display/colorNightFrame"
#define SETTINGS_display_colorSectionsDay "display/colorSectionsDay"
#define SETTINGS_display_colorSectionsNight "display/colorSectionsNight"
#define SETTINGS_display_colorTextDay "display/colorTextDay"
#define SETTINGS_display_colorTextNight "display/colorTextNight"
#define SETTINGS_display_colorVehicle "display/colorVehicle"
#define SETTINGS_display_customColors "display/customColors"
#define SETTINGS_display_customSectionColors "display/customSectionColors"
#define SETTINGS_display_features "display/features"
#define SETTINGS_display_isAutoOffAgIO "display/isAutoOffAgIO"
#define SETTINGS_display_isAutoStartAgIO "display/isAutoStartAgIO"
#define SETTINGS_display_isBrightnessOn "display/isBrightnessOn"
#define SETTINGS_display_isDayMode "display/isDayMode"
#define SETTINGS_display_isHardwareMessages "display/isHardwareMessages"
#define SETTINGS_display_isKeyboardOn "display/isKeyboardOn"
#define SETTINGS_display_isLineSmooth "display/isLineSmooth"
#define SETTINGS_display_isLogElevation "display/isLogElevation"
#define SETTINGS_display_isSectionLinesOn "display/isSectionLinesOn"
#define SETTINGS_display_isShutDownWhenNoPower "display/isShutDownWhenNoPower"
#define SETTINGS_display_isStartFullscreen "display/isStartFullscreen"
#define SETTINGS_display_isSvennArrowOn "display/isSvennArrowOn"
#define SETTINGS_display_isTermsAccepted "display/isTermsAccepted"
#define SETTINGS_display_isTextureOn "display/isTextureOn"
#define SETTINGS_display_isVehicleImage "display/isVehicleImage"
#define SETTINGS_display_lightbarCmPerPixel "display/lightbarCmPerPixel"
#define SETTINGS_display_lineWidth "display/lineWidth"
#define SETTINGS_display_panelSimLocation "display/panelSimLocation"
#define SETTINGS_display_showBack "display/showBack"
#define SETTINGS_display_topTrackNum "display/topTrackNum"
#define SETTINGS_display_triangleResolution "display/triangleResolution"
#define SETTINGS_display_useTrackZero "display/useTrackZero"
#define SETTINGS_display_vehicleOpacity "display/vehicleOpacity"
#define SETTINGS_f_boundaryTriggerDistance "f/boundaryTriggerDistance"
#define SETTINGS_f_currentDir "f/currentDir"
#define SETTINGS_f_isRemoteWorkSystemOn "f/isRemoteWorkSystemOn"
#define SETTINGS_f_isSteerWorkSwitchEnabled "f/isSteerWorkSwitchEnabled"
#define SETTINGS_f_isSteerWorkSwitchManualSections "f/isSteerWorkSwitchManualSections"
#define SETTINGS_f_isWorkSwitchActiveLow "f/isWorkSwitchActiveLow"
#define SETTINGS_f_isWorkSwitchEnabled "f/isWorkSwitchEnabled"
#define SETTINGS_f_isWorkSwitchManualSections "f/isWorkSwitchManualSections"
#define SETTINGS_f_minHeadingStepDistance "f/minHeadingStepDistance"
#define SETTINGS_f_userTotalArea "f/userTotalArea"
#define SETTINGS_feature_isABLineOn "feature/isABLineOn"
#define SETTINGS_feature_isABSmoothOn "feature/isABSmoothOn"
#define SETTINGS_feature_isAgIOOn "feature/isAgIOOn"
#define SETTINGS_feature_isAutoSectionOn "feature/isAutoSectionOn"
#define SETTINGS_feature_isAutoSteerOn "feature/isAutoSteerOn"
#define SETTINGS_feature_isBndContourOn "feature/isBndContourOn"
#define SETTINGS_feature_isBoundaryOn "feature/isBoundaryOn"
#define SETTINGS_feature_isContourOn "feature/isContourOn"
#define SETTINGS_feature_isCurveOn "feature/isCurveOn"
#define SETTINGS_feature_isCycleLinesOn "feature/isCycleLinesOn"
#define SETTINGS_feature_isHeadlandOn "feature/isHeadlandOn"
#define SETTINGS_feature_isHideContourOn "feature/isHideContourOn"
#define SETTINGS_feature_isLateralOn "feature/isLateralOn"
#define SETTINGS_feature_isManualSectionOn "feature/isManualSectionOn"
#define SETTINGS_feature_isOffsetFixOn "feature/isOffsetFixOn"
#define SETTINGS_feature_isRecPathOn "feature/isRecPathOn"
#define SETTINGS_feature_isSteerModeOn "feature/isSteerModeOn"
#define SETTINGS_feature_isTramOn "feature/isTramOn"
#define SETTINGS_feature_isUTurnOn "feature/isUTurnOn"
#define SETTINGS_feature_isWebCamOn "feature/isWebCamOn"
#define SETTINGS_feature_isYouTurnOn "feature/isYouTurnOn"
#define SETTINGS_gps_ageAlarm "gps/ageAlarm"
#define SETTINGS_gps_dualHeadingOffset "gps/dualHeadingOffset"
#define SETTINGS_gps_dualReverseDetectionDistance "gps/dualReverseDetectionDistance"
#define SETTINGS_gps_fixFromWhichSentence "gps/fixFromWhichSentence"
#define SETTINGS_gps_forwardComp "gps/forwardComp"
#define SETTINGS_gps_headingFromWhichSource "gps/headingFromWhichSource"
#define SETTINGS_gps_isRTK "gps/isRTK"
#define SETTINGS_gps_isRTKKillAutoSteer "gps/isRTKKillAutoSteer"
#define SETTINGS_gps_jumpFixAlarmDispance "gps/jumpFixAlarmDispance"
#define SETTINGS_gps_minimumStepLimit "gps/minimumStepLimit"
#define SETTINGS_gps_reverseComp "gps/reverseComp"
#define SETTINGS_gps_simLatitude "gps/simLatitude"
#define SETTINGS_gps_simLongitude "gps/simLongitude"
#define SETTINGS_gps_udpWatchMSec "gps/udpWatchMSec"
#define SETTINGS_headland_isSectionControlled "headland/isSectionControlled"
#define SETTINGS_imu_fusionWeight2 "imu/fusionWeight2"
#define SETTINGS_imu_invertRoll "imu/invertRoll"
#define SETTINGS_imu_isDualAsIMU "imu/isDualAsIMU"
#define SETTINGS_imu_isHeadingCorrectionFromAutoSteer "imu/isHeadingCorrectionFromAutoSteer"
#define SETTINGS_imu_isReverseOn "imu/isReverseOn"
#define SETTINGS_imu_pitchZeroX16 "imu/pitchZeroX16"
#define SETTINGS_imu_rollFilter "imu/rollFilter"
#define SETTINGS_imu_rollZero "imu/rollZero"
#define SETTINGS_jobMenu_location "jobMenu/location"
#define SETTINGS_jobMenu_size "jobMenu/size"
#define SETTINGS_key_hotKeys "key/hotKeys"
#define SETTINGS_menu_isCompassOn "menu/isCompassOn"
#define SETTINGS_menu_isGridOn "menu/isGridOn"
#define SETTINGS_menu_isLightBarNotSteerBar "menu/isLightBarNotSteerBar"
#define SETTINGS_menu_isLightBarOn "menu/isLightBarOn"
#define SETTINGS_menu_isMetric "menu/isMetric"
#define SETTINGS_menu_isOGLZoom "menu/isOGLZoom"
#define SETTINGS_menu_isPureOn "menu/isPureOn"
#define SETTINGS_menu_isSideGuideLines "menu/isSideGuideLines"
#define SETTINGS_menu_isSimulatorOn "menu/isSimulatorOn"
#define SETTINGS_menu_isSkyOn "menu/isSkyOn"
#define SETTINGS_menu_isSpeedoOn "menu/isSpeedoOn"
#define SETTINGS_relay_pinConfig "relay/pinConfig"
#define SETTINGS_section_isFast "section/isFast"
#define SETTINGS_section_position1 "section/position1"
#define SETTINGS_section_position10 "section/position10"
#define SETTINGS_section_position11 "section/position11"
#define SETTINGS_section_position12 "section/position12"
#define SETTINGS_section_position13 "section/position13"
#define SETTINGS_section_position14 "section/position14"
#define SETTINGS_section_position15 "section/position15"
#define SETTINGS_section_position16 "section/position16"
#define SETTINGS_section_position17 "section/position17"
#define SETTINGS_section_position2 "section/position2"
#define SETTINGS_section_position3 "section/position3"
#define SETTINGS_section_position4 "section/position4"
#define SETTINGS_section_position5 "section/position5"
#define SETTINGS_section_position6 "section/position6"
#define SETTINGS_section_position7 "section/position7"
#define SETTINGS_section_position8 "section/position8"
#define SETTINGS_section_position9 "section/position9"
#define SETTINGS_seed_blockageIsOn "seed/blockageIsOn"
#define SETTINGS_seed_blockCountMax "seed/blockCountMax"
#define SETTINGS_seed_blockCountMin "seed/blockCountMin"
#define SETTINGS_seed_blockRow1 "seed/blockRow1"
#define SETTINGS_seed_blockRow2 "seed/blockRow2"
#define SETTINGS_seed_blockRow3 "seed/blockRow3"
#define SETTINGS_seed_blockRow4 "seed/blockRow4"
#define SETTINGS_seed_numRows "seed/numRows"
#define SETTINGS_sound_autoSteerSound "sound/autoSteerSound"
#define SETTINGS_sound_isHydLiftOn "sound/isHydLiftOn"
#define SETTINGS_sound_isSectionOn "sound/isSectionOn"
#define SETTINGS_sound_isUturnOn "sound/isUturnOn"
#define SETTINGS_tool_defaultSectionWidth "tool/defaultSectionWidth"
#define SETTINGS_tool_isDirectionMarkers "tool/isDirectionMarkers"
#define SETTINGS_tool_isDisplayTramControl "tool/isDisplayTramControl"
#define SETTINGS_tool_isSectionOffWhenOut "tool/isSectionOffWhenOut"
#define SETTINGS_tool_isSectionsNotZones "tool/isSectionsNotZones"
#define SETTINGS_tool_isTBT "tool/isTBT"
#define SETTINGS_tool_isToolFront "tool/isToolFront"
#define SETTINGS_tool_isToolRearFixed "tool/isToolRearFixed"
#define SETTINGS_tool_isToolTrailing "tool/isToolTrailing"
#define SETTINGS_tool_isTramOuterInverted "tool/isTramOuterInverted"
#define SETTINGS_tool_numSectionsMulti "tool/numSectionsMulti"
#define SETTINGS_tool_sectionWidthMulti "tool/sectionWidthMulti"
#define SETTINGS_tool_toolTrailingHitchLength "tool/toolTrailingHitchLength"
#define SETTINGS_tool_trailingToolToPivotLength "tool/trailingToolToPivotLength"
#define SETTINGS_tool_zones "tool/zones"
#define SETTINGS_tram_alpha "tram/alpha"
#define SETTINGS_tram_basedOn "tram/basedOn"
#define SETTINGS_tram_isTramOnBackBuffer "tram/isTramOnBackBuffer"
#define SETTINGS_tram_offset "tram/offset"
#define SETTINGS_tram_passes "tram/passes"
#define SETTINGS_tram_skips "tram/skips"
#define SETTINGS_tram_snapAdj "tram/snapAdj"
#define SETTINGS_tram_width "tram/width"
#define SETTINGS_vehicle_antennaHeight "vehicle/antennaHeight"
#define SETTINGS_vehicle_antennaOffset "vehicle/antennaOffset"
#define SETTINGS_vehicle_antennaPivot "vehicle/antennaPivot"
#define SETTINGS_vehicle_goalPointAcquireFactor "vehicle/goalPointAcquireFactor"
#define SETTINGS_vehicle_goalPointLookAhead "vehicle/goalPointLookAhead"
#define SETTINGS_vehicle_goalPointLookAheadHold "vehicle/goalPointLookAheadHold"
#define SETTINGS_vehicle_goalPointLookAheadMult "vehicle/goalPointLookAheadMult"
#define SETTINGS_vehicle_hitchLength "vehicle/hitchLength"
#define SETTINGS_vehicle_hydraulicLiftLookAhead "vehicle/hydraulicLiftLookAhead"
#define SETTINGS_vehicle_isMachineControlToAutoSteer "vehicle/isMachineControlToAutoSteer"
#define SETTINGS_vehicle_isPivotBehindAntenna "vehicle/isPivotBehindAntenna"
#define SETTINGS_vehicle_isStanleyUsed "vehicle/isStanleyUsed"
#define SETTINGS_vehicle_isSteerAxleAhead "vehicle/isSteerAxleAhead"
#define SETTINGS_vehicle_lookAheadMinimum "vehicle/lookAheadMinimum"
#define SETTINGS_vehicle_maxAngularVelocity "vehicle/maxAngularVelocity"
#define SETTINGS_vehicle_maxSteerAngle "vehicle/maxSteerAngle"
#define SETTINGS_vehicle_minCoverage "vehicle/minCoverage"
#define SETTINGS_vehicle_minTurningRadius "vehicle/minTurningRadius"
#define SETTINGS_vehicle_numSections "vehicle/numSections"
#define SETTINGS_vehicle_panicStopSpeed "vehicle/panicStopSpeed"
#define SETTINGS_vehicle_purePursuitIntegralGainAB "vehicle/purePursuitIntegralGainAB"
#define SETTINGS_vehicle_slowSpeedCutoff "vehicle/slowSpeedCutoff"
#define SETTINGS_vehicle_stanleyDistanceErrorGain "vehicle/stanleyDistanceErrorGain"
#define SETTINGS_vehicle_stanleyHeadingErrorGain "vehicle/stanleyHeadingErrorGain"
#define SETTINGS_vehicle_stanleyIntegralDistanceAwayTriggerAB "vehicle/stanleyIntegralDistanceAwayTriggerAB"
#define SETTINGS_vehicle_stanleyIntegralGainAB "vehicle/stanleyIntegralGainAB"
#define SETTINGS_vehicle_tankTrailingHitchLength "vehicle/tankTrailingHitchLength"
#define SETTINGS_vehicle_toolLookAheadOff "vehicle/toolLookAheadOff"
#define SETTINGS_vehicle_toolLookAheadOn "vehicle/toolLookAheadOn"
#define SETTINGS_vehicle_toolOffDelay "vehicle/toolOffDelay"
#define SETTINGS_vehicle_toolOffset "vehicle/toolOffset"
#define SETTINGS_vehicle_toolOverlap "vehicle/toolOverlap"
#define SETTINGS_vehicle_toolWidth "vehicle/toolWidth"
#define SETTINGS_vehicle_trackWidth "vehicle/trackWidth"
#define SETTINGS_vehicle_vehicleName "vehicle/vehicleName"
#define SETTINGS_vehicle_vehicleType "vehicle/vehicleType"
#define SETTINGS_vehicle_wheelbase "vehicle/wheelbase"
#define SETTINGS_window_abDrawLocation "window/abDrawLocation"
#define SETTINGS_window_bingMapSize "window/bingMapSize"
#define SETTINGS_window_bingZoom "window/bingZoom"
#define SETTINGS_window_buildTracksLocation "window/buildTracksLocation"
#define SETTINGS_window_formNudgeLocation "window/formNudgeLocation"
#define SETTINGS_window_gridLocation "window/gridLocation"
#define SETTINGS_window_gridSize "window/gridSize"
#define SETTINGS_window_headAcheSize "window/headAcheSize"
#define SETTINGS_window_headlineSize "window/headlineSize"
#define SETTINGS_window_isKioskMode "window/isKioskMode"
#define SETTINGS_window_isShutdownComputer "window/isShutdownComputer"
#define SETTINGS_window_location "window/location"
#define SETTINGS_window_mapBndSize "window/mapBndSize"
#define SETTINGS_window_maximized "window/maximized"
#define SETTINGS_window_minimized "window/minimized"
#define SETTINGS_window_quickABLocation "window/quickABLocation"
#define SETTINGS_window_rateMapSize "window/rateMapSize"
#define SETTINGS_window_rateMapZoom "window/rateMapZoom"
#define SETTINGS_window_size "window/size"
#define SETTINGS_window_steerSettingsLocation "window/steerSettingsLocation"
#define SETTINGS_window_tramLineSize "window/tramLineSize"
#define SETTINGS_youturn_distanceFromBoundary "youturn/distanceFromBoundary"
#define SETTINGS_youturn_extensionLength "youturn/extensionLength"
#define SETTINGS_youturn_radius "youturn/radius"
#define SETTINGS_youturn_skipWidth "youturn/skipWidth"
#define SETTINGS_youturn_style "youturn/style"
#define SETTINGS_youturn_toolWidths "youturn/toolWidths"
#define SETTINGS_youturn_youMoveDistance "youturn/youMoveDistance"
#define SETTINGS_test_testStrings "test/testStrings"
#define SETTINGS_test_testDoubles "test/testDoubles"
#define SETTINGS_test_testColorRgbF "test/testColorRgbF"
#endif
