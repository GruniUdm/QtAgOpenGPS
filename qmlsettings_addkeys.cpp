// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
//
#include "qmlsettings.h"

void QMLSettings::setupKeys() {
    addKey(QString("setWindow_Location"),QString("window/location"),"QPoint");
    addKey(QString("setWindow_Size"),QString("window/size"),"QString");
    addKey(QString("setWindow_Maximized"),QString("window/maximized"),"bool");
    addKey(QString("setWindow_Minimized"),QString("window/minimized"),"bool");
    addKey(QString("setDisplay_triangleResolution"),QString("display/triangleResolution"),"double");
    addKey(QString("setMenu_isMetric"),QString("menu/isMetric"),"bool");
    addKey(QString("setMenu_isGridOn"),QString("menu/isGridOn"),"bool");
    addKey(QString("setMenu_isLightbarOn"),QString("menu/isLightBarOn"),"bool");
    addKey(QString("setF_CurrentDir"),QString("f/currentDir"),"QString");
    addKey(QString("setF_isWorkSwitchEnabled"),QString("f/isWorkSwitchEnabled"),"bool");
    addKey(QString("setIMU_pitchZeroX16"),QString("imu/pitchZeroX16"),"int");
    addKey(QString("setIMU_rollZero"),QString("imu/rollZero"),"double");
    addKey(QString("setF_minHeadingStepDistance"),QString("f/minHeadingStepDistance"),"double");
    addKey(QString("setAS_lowSteerPWM"),QString("as/lowSteerPWM"),"double");
    addKey(QString("setAS_wasOffset"),QString("as/wasOffset"),"int");
    addKey(QString("setF_UserTotalArea"),QString("f/userTotalArea"),"double");
    addKey(QString("setAS_minSteerPWM"),QString("as/minSteerPWM"),"double");
    addKey(QString("setF_boundaryTriggerDistance"),QString("f/boundaryTriggerDistance"),"double");
    addKey(QString("setAS_highSteerPWM"),QString("as/highSteerPWM"),"double");
    addKey(QString("setMenu_isSideGuideLines"),QString("menu/isSideGuideLines"),"bool");
    addKey(QString("setAS_countsPerDegree"),QString("as/countsPerDegree"),"double");
    addKey(QString("setMenu_isPureOn"),QString("menu/isPureOn"),"bool");
    addKey(QString("setMenu_isSimulatorOn"),QString("menu/isSimulatorOn"),"bool");
    addKey(QString("setMenu_isSkyOn"),QString("menu/isSkyOn"),"bool");
    addKey(QString("setDisplay_lightbarCmPerPixel"),QString("display/lightbarCmPerPixel"),"int");
    addKey(QString("setGPS_fixFromWhichSentence"),QString("gps/fixFromWhichSentence"),"QString");
    addKey(QString("setGPS_headingFromWhichSource"),QString("gps/headingFromWhichSource"),"QString");
    addKey(QString("setGPS_SimLatitude"),QString("gps/simLatitude"),"double");
    addKey(QString("setGPS_SimLongitude"),QString("gps/simLongitude"),"double");
    addKey(QString("setAS_snapDistance"),QString("as/snapDistance"),"double");
    addKey(QString("setF_isWorkSwitchManualSections"),QString("f/isWorkSwitchManualSections"),"bool");
    addKey(QString("setAS_isAutoSteerAutoOn"),QString("as/isAutoSteerAutoOn"),"bool");
    addKey(QString("setDisplay_lineWidth"),QString("display/lineWidth"),"int");
    addKey(QString("setDisplay_panelSimLocation"),QString("display/panelSimLocation"),"QPoint");
    addKey(QString("setTram_tramWidth"),QString("tram/width"),"double");
    addKey(QString("setTram_snapAdj"),QString("tram/snapAdj"),"double");
    addKey(QString("setTram_passes"),QString("tram/passes"),"int");
    addKey(QString("setTram_offset"),QString("tram/offset"),"double");
    addKey(QString("setMenu_isOGLZoomOn"),QString("menu/isOGLZoom"),"int");
    addKey(QString("setMenu_isCompassOn"),QString("menu/isCompassOn"),"bool");
    addKey(QString("setMenu_isSpeedoOn"),QString("menu/isSpeedoOn"),"bool");
    addKey(QString("setDisplay_colorDayFrame"),QString("display/colorDayFrame"),"QColor");
    addKey(QString("setDisplay_colorNightFrame"),QString("display/colorNightFrame"),"QColor");
    addKey(QString("setDisplay_colorSectionsDay"),QString("display/colorSectionsDay"),"QColor");
    addKey(QString("setDisplay_colorFieldDay"),QString("display/colorFieldDay"),"QColor");
    addKey(QString("setDisplay_isDayMode"),QString("display/isDayMode"),"bool");
    addKey(QString("setDisplay_colorSectionsNight"),QString("display/colorSectionsNight"),"QColor");
    addKey(QString("setDisplay_colorFieldNight"),QString("display/colorFieldNight"),"QColor");
    addKey(QString("setDisplay_isAutoDayNight"),QString("display/autoDayNight"),"bool");
    addKey(QString("setDisplay_customColors"),QString("display/customColors"),"QString");
    addKey(QString("setDisplay_isTermsAccepted"),QString("display/isTermsAccepted"),"bool");
    addKey(QString("setGPS_isRTK"),QString("gps/isRTK"),"bool");
    addKey(QString("setDisplay_isStartFullScreen"),QString("display/isStartFullscreen"),"bool");
    addKey(QString("setDisplay_isKeyboardOn"),QString("display/isKeyboardOn"),"bool");
    addKey(QString("setIMU_rollFilter"),QString("imu/rollFilter"),"double");
    addKey(QString("setAS_uTurnSmoothing"),QString("as/uTurnSmoothing"),"int");
    addKey(QString("setIMU_invertRoll"),QString("imu/invertRoll"),"bool");
    addKey(QString("setAS_ackerman"),QString("as/ackerman"),"double");
    addKey(QString("setF_isWorkSwitchActiveLow"),QString("f/isWorkSwitchActiveLow"),"bool");
    addKey(QString("setAS_Kp"),QString("as/Kp"),"double");
    addKey(QString("setSound_isUturnOn"),QString("sound/isUturnOn"),"bool");
    addKey(QString("setSound_isHydLiftOn"),QString("sound/isHydLiftOn"),"bool");
    addKey(QString("setDisplay_colorTextNight"),QString("display/colorTextNight"),"QColor");
    addKey(QString("setDisplay_colorTextDay"),QString("display/colorTextDay"),"QColor");
    addKey(QString("setTram_isTramOnBackBuffer"),QString("tram/isTramOnBackBuffer"),"bool");
    addKey(QString("setDisplay_camZoom"),QString("display/camZoom"),"double");
    addKey(QString("setDisplay_colorVehicle"),QString("display/colorVehicle"),"QColor");
    addKey(QString("setDisplay_vehicleOpacity"),QString("display/vehicleOpacity"),"int");
    addKey(QString("setDisplay_isVehicleImage"),QString("display/isVehicleImage"),"bool");
    addKey(QString("setIMU_isHeadingCorrectionFromAutoSteer"),QString("imu/isHeadingCorrectionFromAutoSteer"),"QString");
    addKey(QString("setDisplay_isTextureOn"),QString("display/isTextureOn"),"bool");
    addKey(QString("setAB_lineLength"),QString("ab/lineLength"),"double");
    addKey(QString("setGPS_udpWatchMsec"),QString("gps/udpWatchMSec"),"int");
    addKey(QString("setF_isSteerWorkSwitchManualSections"),QString("f/isSteerWorkSwitchManualSections"),"bool");
    addKey(QString("setAS_isConstantContourOn"),QString("as/isConstantContourOn"),"bool");
    addKey(QString("setAS_guidanceLookAheadTime"),QString("as/guidanceLookAheadTime"),"double");
    addKey(QString("setFeatures"),QString("display/features"),"QString");
    addKey(QString("setIMU_isDualAsIMU"),QString("imu/isDualAsIMU"),"bool");
    addKey(QString("setAS_sideHillComp"),QString("as/sideHillCompensation"),"double");
    addKey(QString("setIMU_isReverseOn"),QString("imu/isReverseOn"),"bool");
    addKey(QString("setGPS_forwardComp"),QString("gps/forwardComp"),"double");
    addKey(QString("setGPS_reverseComp"),QString("gps/reverseComp"),"double");
    addKey(QString("setGPS_ageAlarm"),QString("gps/ageAlarm"),"int");
    addKey(QString("setGPS_isRTK_KillAutoSteer"),QString("gps/isRTK_KillAutoSteer"),"bool");
    addKey(QString("setColor_sec01"),QString("color/sec01"),"QColor");
    addKey(QString("setColor_sec02"),QString("color/sec02"),"QColor");
    addKey(QString("setColor_sec03"),QString("color/sec03"),"QColor");
    addKey(QString("setColor_sec04"),QString("color/sec04"),"QColor");
    addKey(QString("setColor_sec05"),QString("color/sec05"),"QColor");
    addKey(QString("setColor_sec06"),QString("color/sec06"),"QColor");
    addKey(QString("setColor_sec07"),QString("color/sec07"),"QColor");
    addKey(QString("setColor_sec08"),QString("color/sec08"),"QColor");
    addKey(QString("setColor_sec09"),QString("color/sec09"),"QColor");
    addKey(QString("setColor_sec10"),QString("color/sec10"),"QColor");
    addKey(QString("setColor_sec11"),QString("color/sec11"),"QColor");
    addKey(QString("setColor_sec12"),QString("color/sec12"),"QColor");
    addKey(QString("setColor_sec13"),QString("color/sec13"),"QColor");
    addKey(QString("setColor_sec14"),QString("color/sec14"),"QColor");
    addKey(QString("setColor_sec15"),QString("color/sec15"),"QColor");
    addKey(QString("setColor_sec16"),QString("color/sec16"),"QColor");
    addKey(QString("setColor_isMultiColorSections"),QString("color/isMultiColorSections"),"bool");
    addKey(QString("setDisplay_customSectionColors"),QString("display/customSectionColors"),"QString");
    addKey(QString("setBrand_TBrand"),QString("brand/TBrand"),"QString");
    addKey(QString("setHeadland_isSectionControlled"),QString("headland/isSectionControlled"),"bool");
    addKey(QString("setSound_isAutoSteerOn"),QString("sound/autoSteerSound"),"bool");
    addKey(QString("setRelay_pinConfig"),QString("relay/pinConfig"),"leavealone");
    addKey(QString("setDisplay_camSmooth"),QString("display/camSmooth"),"int");
    addKey(QString("setGPS_dualHeadingOffset"),QString("gps/dualHeadingOffset"),"double");
    addKey(QString("setF_isSteerWorkSwitchEnabled"),QString("f/isSteerWorkSwitchEnabled"),"bool");
    addKey(QString("setF_isRemoteWorkSystemOn"),QString("f/isRemoteWorkSystemOn"),"bool");
    addKey(QString("setDisplay_isAutoStartAgIO"),QString("display/isAutoStartAgIO"),"bool");
    addKey(QString("setAS_ModeXTE"),QString("as/modeXTE"),"double");
    addKey(QString("setAS_ModeTime"),QString("as/modeTime"),"int");
    addKey(QString("setVehicle_toolWidth"),QString("vehicle/toolWidth"),"double");
    addKey(QString("setVehicle_toolOverlap"),QString("vehicle/toolOverlap"),"double");
    addKey(QString("setTool_toolTrailingHitchLength"),QString("tool/toolTrailingHitchLength"),"double");
    addKey(QString("setVehicle_numSections"),QString("vehicle/numSections"),"int");
    addKey(QString("setSection_position1"),QString("section/position1"),"double");
    addKey(QString("setSection_position2"),QString("section/position2"),"double");
    addKey(QString("setSection_position3"),QString("section/position3"),"double");
    addKey(QString("setSection_position4"),QString("section/position4"),"double");
    addKey(QString("setSection_position5"),QString("section/position5"),"double");
    addKey(QString("setSection_position6"),QString("section/position6"),"double");
    addKey(QString("setSection_position7"),QString("section/position7"),"double");
    addKey(QString("setSection_position8"),QString("section/position8"),"double");
    addKey(QString("setSection_position9"),QString("section/position9"),"double");
    addKey(QString("setSection_position10"),QString("section/position10"),"double");
    addKey(QString("setSection_position11"),QString("section/position11"),"double");
    addKey(QString("setSection_position12"),QString("section/position12"),"double");
    addKey(QString("setSection_position13"),QString("section/position13"),"double");
    addKey(QString("setSection_position14"),QString("section/position14"),"double");
    addKey(QString("setSection_position15"),QString("section/position15"),"double");
    addKey(QString("setSection_position16"),QString("section/position16"),"double");
    addKey(QString("setSection_position17"),QString("section/position17"),"double");
    addKey(QString("purePursuitIntegralGainAB"),QString("vehicle/purePursuitIntegralGainAB"),"double");
    addKey(QString("set_youMoveDistance"),QString("youturn/youMoveDistance"),"double");
    addKey(QString("setVehicle_antennaHeight"),QString("vehicle/antennaHeight"),"double");
    addKey(QString("setVehicle_toolLookAheadOn"),QString("vehicle/toollookAheadOn"),"double");
    addKey(QString("setTool_isToolTrailing"),QString("tool/isToolTrailing"),"bool");
    addKey(QString("setVehicle_toolOffset"),QString("vehicle/toolOffset"),"double");
    addKey(QString("setTool_isToolRearFixed"),QString("tool/isToolRearFixed"),"bool");
    addKey(QString("setVehicle_antennaPivot"),QString("vehicle/antennaPivot"),"double");
    addKey(QString("setVehicle_wheelbase"),QString("vehicle/wheelbase"),"double");
    addKey(QString("setVehicle_hitchLength"),QString("vehicle/hitchLength"),"double");
    addKey(QString("setVehicle_toolLookAheadOff"),QString("vehicle/toolLookAheadOff"),"double");
    addKey(QString("setVehicle_isPivotBehindAntenna"),QString("vehicle/isPivotBehindAntenna"),"bool");
    addKey(QString("setVehicle_isSteerAxleAhead"),QString("vehicle/isSteerAxleAhead"),"bool");
    addKey(QString("setVehicle_slowSpeedCutoff"),QString("vehicle/slowSpeedCutoff"),"double");
    addKey(QString("setVehicle_tankTrailingHitchLength"),QString("vehicle/tankTrailingHitchLength"),"double");
    addKey(QString("setVehicle_minCoverage"),QString("vehicle/minCoverage"),"int");
    addKey(QString("setVehicle_goalPointLookAhead"),QString("vehicle/goalPointLookAhead"),"double");
    addKey(QString("setVehicle_maxAngularVelocity"),QString("vehicle/maxAngularVelocity"),"double");
    addKey(QString("setVehicle_maxSteerAngle"),QString("vehicle/maxSteerAngle"),"double");
    addKey(QString("set_youTurnExtensionLength"),QString("youturn/extensionLength"),"int");
    addKey(QString("set_youToolWidths"),QString("youturn/toolWidths"),"double");
    addKey(QString("setVehicle_minTurningRadius"),QString("vehicle/minTurningRadius"),"double");
    addKey(QString("setVehicle_antennaOffset"),QString("vehicle/antennaOffset"),"double");
    addKey(QString("set_youTurnDistanceFromBoundary"),QString("youturn/distanceFromBoundary"),"double");
    addKey(QString("setVehicle_lookAheadMinimum"),QString("vehicle/lookAheadMinimum"),"double");
    addKey(QString("setVehicle_goalPointLookAheadMult"),QString("vehicle/goalPointLookAheadMult"),"double");
    addKey(QString("stanleyDistanceErrorGain"),QString("vehicle/stanleyDistanceErrorGain"),"double");
    addKey(QString("stanleyHeadingErrorGain"),QString("vehicle/stanleyHeadingErrorGain"),"double");
    addKey(QString("setVehicle_isStanleyUsed"),QString("vehicle/isStanlyUsed"),"bool");
    addKey(QString("setTram_BasedOn"),QString("tram/basedOn"),"int");
    addKey(QString("setTram_Skips"),QString("tram/skips"),"int");
    addKey(QString("setTool_isToolTBT"),QString("tool/isTBT"),"bool");
    addKey(QString("setVehicle_vehicleType"),QString("vehicle/vehicleType"),"int");
    addKey(QString("set_youSkipWidth"),QString("youturn/skipWidth"),"int");
    addKey(QString("setArdSteer_setting1"),QString("ardSteer/setting1"),"double");
    addKey(QString("setArdSteer_minSpeed"),QString("ardSteer/minSpeed"),"double");
    addKey(QString("setArdSteer_maxSpeed"),QString("ardSteer/maxSpeed"),"double");
    addKey(QString("setArdSteer_setting0"),QString("ardSteer/setting0"),"double");
    addKey(QString("setVehicle_hydraulicLiftLookAhead"),QString("vehicle/hydraulicLiftLookAhead"),"double");
    addKey(QString("setVehicle_isMachineControlToAutoSteer"),QString("vehicle/isMachineControlToAutoSteer"),"bool");
    addKey(QString("setArdSteer_maxPulseCounts"),QString("ardSteer/ardSteerMaxPulseCounts"),"double");
    addKey(QString("setArdMac_hydRaiseTime"),QString("ardMac/hydRaiseTime"),"double");
    addKey(QString("setArdMac_hydLowerTime"),QString("ardMac/hydLowerTime"),"double");
    addKey(QString("setArdMac_isHydEnabled"),QString("ardMac/isHydEnabled"),"double");
    addKey(QString("setTool_defaultSectionWidth"),QString("tool/defaultSectionWidth"),"double");
    addKey(QString("setVehicle_toolOffDelay"),QString("vehicle/toolOffDelay"),"double");
    addKey(QString("setArdMac_setting0"),QString("ardMac/setting0"),"double");
    addKey(QString("setArdSteer_setting2"),QString("ardSteer/setting2"),"double");
    addKey(QString("stanleyIntegralDistanceAwayTriggerAB"),QString("vehicle/stanleyIntegralDistanceAwayTriggerAB"),"double");
    addKey(QString("setTool_isToolFront"),QString("tool/isToolFront"),"bool");
    addKey(QString("setVehicle_trackWidth"),QString("vehicle/trackWidth"),"double");
    addKey(QString("setArdMac_isDanfoss"),QString("ardMac/isDanFoss"),"bool");
    addKey(QString("stanleyIntegralGainAB"),QString("vehicle/stanleyIntegralGainAB"),"double");
    addKey(QString("setSection_isFast"),QString("section/isFast"),"bool");
    addKey(QString("setArdMac_user1"),QString("ardMac/user1"),"double");
    addKey(QString("setArdMac_user2"),QString("ardMac/user2"),"double");
    addKey(QString("setArdMac_user3"),QString("ardMac/user3"),"double");
    addKey(QString("setArdMac_user4"),QString("ardMac/user4"),"double");
    addKey(QString("setVehicle_panicStopSpeed"),QString("vehicle/panicStopSpeed"),"double");
    addKey(QString("setAS_ModeMultiplierStanley"),QString("as/modeMultiplierStanley"),"double");
    addKey(QString("setDisplay_brightness"),QString("display/brightness"),"int");
    addKey(QString("set_youTurnRadius"),QString("youturn/radius"),"double");
    addKey(QString("setDisplay_brightnessSystem"),QString("display/brightnessSystem"),"int");
    addKey(QString("setTool_isSectionsNotZones"),QString("tool/isSectionsNotZones"),"bool");
    addKey(QString("setTool_numSectionsMulti"),QString("tool/numSectionsMulti"),"int");
    addKey(QString("setTool_zones"),QString("tool/zones"),"leavealone");
    addKey(QString("setTool_sectionWidthMulti"),QString("tool/sectionWidthMulti"),"double");
    addKey(QString("setDisplay_isBrightnessOn"),QString("display/isBrightnessOn"),"bool");
    addKey(QString("setKey_hotkeys"),QString("key/hotKeys"),"QString");
    addKey(QString("setVehicle_goalPointLookAheadHold"),QString("vehicle/goalPointLookAheadHold"),"double");
    addKey(QString("setTool_isSectionOffWhenOut"),QString("tool/isSectionOffWhenOut"),"bool");
    addKey(QString("set_uTurnStyle"),QString("youturn/style"),"int");
    addKey(QString("setGPS_minimumStepLimit"),QString("gps/minimumStepLimit"),"double");
    addKey(QString("setAS_isSteerInReverse"),QString("as/isSteerInReverse"),"bool");
    addKey(QString("setAS_functionSpeedLimit"),QString("as/functionSpeedLimit"),"double");
    addKey(QString("setAS_maxSteerSpeed"),QString("as/maxSteerSpeed"),"double");
    addKey(QString("setAS_minSteerSpeed"),QString("as/minSteerSpeed"),"double");
    addKey(QString("setBrand_HBrand"),QString("brand/HBrand"),"QString");
    addKey(QString("setBrand_WDBrand"),QString("brand/WDBrand"),"QString");
    addKey(QString("setIMU_fusionWeight2"),QString("imu/fusionWeight2"),"double");
    addKey(QString("setDisplay_isSvennArrowOn"),QString("display/isSvennArrowOn"),"bool");
    addKey(QString("setTool_isTramOuterInverted"),QString("tool/isTramOuterInverted"),"bool");
    addKey(QString("setJobMenu_location"),QString("jobMenu/location"),"QPoint");
    addKey(QString("setJobMenu_size"),QString("jobMenu/size"),"QString");
    addKey(QString("setWindow_steerSettingsLocation"),QString("window/steerSettingsLocation"),"QPoint");
    addKey(QString("setWindow_buildTracksLocation"),QString("window/buildTracksLocation"),"QPoint");
    addKey(QString("setTool_trailingToolToPivotLength"),QString("tool/trailingToolToPivotLength"),"double");
    addKey(QString("setWindow_formNudgeLocation"),QString("window/formNudgeLocation"),"QPoint");
    addKey(QString("setWindow_formNudgeSize"),QString("window/formNudgeSize"),"QString");
    addKey(QString("setAS_snapDistanceRef"),QString("as/snapDistanceRef"),"double");
    addKey(QString("setDisplay_buttonOrder"),QString("display/buttonOrder"),"QString");
    addKey(QString("setDisplay_camPitch"),QString("display/camPitch"),"double");
    addKey(QString("setWindow_abDrawSize"),QString("window/abDrawSize"),"QString");
    addKey(QString("setWindow_HeadlineSize"),QString("window/headlineSize"),"QString");
    addKey(QString("setWindow_HeadAcheSize"),QString("window/headAcheSize"),"QString");
    addKey(QString("setWindow_MapBndSize"),QString("window/mapBndSize"),"QString");
    addKey(QString("setWindow_BingMapSize"),QString("window/bingMapSize"),"QString");
    addKey(QString("setWindow_BingZoom"),QString("window/bingZoom"),"int");
    addKey(QString("setWindow_RateMapSize"),QString("window/rateMapSize"),"QString");
    addKey(QString("setWindow_RateMapZoom"),QString("window/rateMapZoom"),"int");
    addKey(QString("setWindow_QuickABLocation"),QString("window/quickABLocation"),"QPoint");
    addKey(QString("setDisplay_isLogElevation"),QString("display/isLogElevation"),"bool");
    addKey(QString("setSound_isSectionsOn"),QString("sound/isSectionOn"),"bool");
    addKey(QString("setGPS_dualReverseDetectionDistance"),QString("gps/dualReverseDetectionDistance"),"double");
    addKey(QString("setTool_isDisplayTramControl"),QString("tool/isDisplayTramControl"),"bool");
    addKey(QString("setAS_uTurnCompensation"),QString("as/uTurnCompensation"),"double");
    addKey(QString("setWindow_gridSize"),QString("window/gridSize"),"QString");
    addKey(QString("setWindow_gridLocation"),QString("window/gridLocation"),"QPoint");
    addKey(QString("setWindow_isKioskMode"),QString("window/isKioskMode"),"bool");
    addKey(QString("setDisplay_isAutoOffAgIO"),QString("display/isAutoOffAgIO"),"bool");
    addKey(QString("setWindow_isShutdownComputer"),QString("window/isShutdownComputer"),"bool");
    addKey(QString("setDisplay_isShutdownWhenNoPower"),QString("display/isShutDownWhenNoPower"),"bool");
    addKey(QString("setDisplay_isHardwareMessages"),QString("display/isHardwareMessages"),"bool");
    addKey(QString("setGPS_jumpFixAlarmDistance"),QString("gps/jumpFixAlarmDispance"),"int");
    addKey(QString("setAS_deadZoneDistance"),QString("as/deadZoneDistance"),"int");
    addKey(QString("setAS_deadZoneHeading"),QString("as/deadZoneHeading"),"int");
    addKey(QString("setMenu_isLightbarNotSteerBar"),QString("menu/isLightBarNotSteerBar"),"bool");
    addKey(QString("setTool_isDirectionMarkers"),QString("tool/isDirectionMarkers"),"bool");
    addKey(QString("setAS_numGuideLines"),QString("as/numGuideLines"),"int");
    addKey(QString("setAS_deadZoneDelay"),QString("as/deadZoneDelay"),"int");
    addKey(QString("setTram_alpha"),QString("tram/alpha"),"double");
    addKey(QString("setVehicle_goalPointAcquireFactor"),QString("vehicle/goalPointAcquireFactor"),"double");
    addKey(QString("setBnd_isDrawPivot"),QString("bnd/isDrawPivot"),"bool");
    addKey(QString("setDisplay_isSectionLinesOn"),QString("display/isSectionLinesOn"),"bool");
    addKey(QString("setDisplay_isLineSmooth"),QString("display/isLineSmooth"),"bool");
    addKey(QString("setWindow_tramLineSize"),QString("window/tramLineSize"),"QString");
    addKey(QString("setFeature_isHeadlandOn"),QString("feature/isHeadlandOn"), "bool");
    addKey(QString("setFeature_isTramOn"),QString("feature/isTramOn"), "bool");
    addKey(QString("setFeature_isBoundaryOn"),QString("feature/isBoundaryOn"), "bool");
    addKey(QString("setFeature_isBndContourOn"),QString("feature/isBndContourOn"), "bool");
    addKey(QString("setFeature_isRecPathOn"),QString("feature/isRecPathOn"), "bool");
    addKey(QString("setFeature_isABSmoothOn"),QString("feature/isABSmoothOn"), "bool");
    addKey(QString("setFeature_isHideContourOn"),QString("feature/isHideContourOn"), "bool");
    addKey(QString("setFeature_isWebCamOn"),QString("feature/isWebCamOn"), "bool");
    addKey(QString("setFeature_isOffsetFixOn"),QString("feature/isOffsetFixOn"), "bool");
    addKey(QString("setFeature_isAgIOOn"),QString("feature/isAgIOOn"), "bool");
    addKey(QString("setFeature_isContourOn"),QString("feature/isContourOn"), "bool");
    addKey(QString("setFeature_isYouTurnOn"),QString("feature/isYouTurnOn"), "bool");
    addKey(QString("setFeature_isSteerModeOn"),QString("feature/isSteerModeOn"), "bool");
    addKey(QString("setFeature_isManualSectionOn"),QString("feature/isManualSectionOn"), "bool");
    addKey(QString("setFeature_isAutoSectionOn"),QString("feature/isAutoSectionOn"), "bool");
    addKey(QString("setFeature_isCycleLinesOn"),QString("feature/isCycleLinesOn"), "bool");
    addKey(QString("setFeature_isABLineOn"),QString("feature/isABLineOn"), "bool");
    addKey(QString("setFeature_isCurveOn"),QString("feature/isCurveOn"), "bool");
    addKey(QString("setFeature_isAutoSteerOn"),QString("feature/isAutoSteerOn"), "bool");
    addKey(QString("setFeature_isUTurnOn"),QString("feature/isUTurnOn"), "bool");
    addKey(QString("setFeature_isLateralOn"),QString("feature/isLateralOn"), "bool");
    addKey(QString("setDisplay_useTrackZero"),QString("display/useTrackZero"),"bool");
    addKey(QString("setDisplay_topTrackNum"),QString("display/topTrackNum"),"bool");
    addKey(QString("setSeed_blockCountMin"),QString("seed/blockCountMin"),"double");
    addKey(QString("setSeed_blockCountMax"),QString("seed/blockCountMax"),"double");
    addKey(QString("setSeed_blockRow1"),QString("seed/blockRow1"),"double");
    addKey(QString("setSeed_blockRow2"),QString("seed/blockRow2"),"double");
    addKey(QString("setSeed_blockRow3"),QString("seed/blockRow3"),"double");
    addKey(QString("setSeed_blockRow4"),QString("seed/blockRow4"),"double");
    addKey(QString("setSeed_numRows"),QString("seed/numRows"),"double");
    addKey(QString("setSeed_blockageIsOn"),QString("seed/blockageIsOn"),"bool");
    addKey(QString("setCam_CamLink"),QString("cam/camLink"),"QString");
}
