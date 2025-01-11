// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
//
#include "qmlsettings.h"

void QMLSettings::setupKeys() {
    addKey(QString("setWindow_Location"),QString("display/windowLocation"),"QPoint");
    addKey(QString("setWindow_Size"),QString("display/windowSize"),"QString");
    addKey(QString("setWindow_Maximized"),QString("display/maximized"),"bool");
    addKey(QString("setWindow_Minimized"),QString("display/minimized"),"bool");
    addKey(QString("setDisplay_triangleResolution"),QString("display/triangleResolution"),"double");
    addKey(QString("setMenu_isMetric"),QString("display/isMetric"),"bool");
    addKey(QString("setMenu_isGridOn"),QString("display/showGrid"),"bool");
    addKey(QString("setMenu_isLightbarOn"),QString("display/isLightBarOn"),"bool");
    addKey(QString("setF_CurrentDir"),QString("display/currentDir"),"QString");
    addKey(QString("setF_isWorkSwitchEnabled"),QString("tool/isWorkSwitchEnabled"),"bool");
    addKey(QString("setIMU_pitchZeroX16"),QString("gps/IMUPitchZeroX16"),"int");
    addKey(QString("setIMU_rollZero"),QString("gps/IMURollZeroX16"),"double");
    addKey(QString("setF_minHeadingStepDistance"),QString("gps/minHeadingStepDistance"),"double");
    addKey(QString("setAS_lowSteerPWM"),QString("autosteer/lowSteerPWM"),"double");
    addKey(QString("setAS_wasOffset"),QString("autosteer/wasOffset"),"int");
    addKey(QString("setF_UserTotalArea"),QString("display/userTotalArea"),"double");
    addKey(QString("setAS_minSteerPWM"),QString("autosteer/minSteerPWM"),"double");
    addKey(QString("setF_boundaryTriggerDistance"),QString("autosteer/boundaryTriggerDistance"),"double");
    addKey(QString("setAS_highSteerPWM"),QString("autosteer/highSteerPWM"),"double");
    addKey(QString("setMenu_isSideGuideLines"),QString("display/isSideGuideLines"),"bool");
    addKey(QString("setAS_countsPerDegree"),QString("autosteer/countsPerDegree"),"double");
    addKey(QString("setMenu_isPureOn"),QString("display/isPureOn"),"bool");
    addKey(QString("setMenu_isSimulatorOn"),QString("sim/on"),"bool");
    addKey(QString("setMenu_isSkyOn"),QString("display/isSkyOn"),"bool");
    addKey(QString("setDisplay_lightbarCmPerPixel"),QString("display/lightbarCMPerPixel"),"int");
    addKey(QString("setGPS_fixFromWhichSentence"),QString("gps/fixFromWhichSentence"),"QString");
    addKey(QString("setGPS_headingFromWhichSource"),QString("gps/headingFromWhichSource"),"QString");
    addKey(QString("setGPS_SimLatitude"),QString("sim/latitude"),"double");
    addKey(QString("setGPS_SimLongitude"),QString("sim/longitude"),"double");
    addKey(QString("setAS_snapDistance"),QString("autosteer/snapDistance"),"double");
    addKey(QString("setF_isWorkSwitchManualSections"),QString("tool/isWorkSwitchManual"),"bool");
    addKey(QString("setAS_isAutoSteerAutoOn"),QString("autosteer/autoOn"),"bool");
    addKey(QString("setDisplay_lineWidth"),QString("display/lineWidth"),"int");
    addKey(QString("setDisplay_panelSimLocation"),QString("display/simLocation"),"QPoint");
    addKey(QString("setTram_tramWidth"),QString("tram/width"),"double");
    addKey(QString("setTram_snapAdj"),QString("tram/swapAdj"),"double");
    addKey(QString("setTram_passes"),QString("tram/passes"),"int");
    addKey(QString("setTram_offset"),QString("tram/offset"),"double");
    addKey(QString("setMenu_isOGLZoomOn"),QString("display/oglZoom"),"int");
    addKey(QString("setMenu_isCompassOn"),QString("display/isCompassOn"),"bool");
    addKey(QString("setMenu_isSpeedoOn"),QString("display/isSpeedOn"),"bool");
    addKey(QString("setDisplay_colorDayFrame"),QString("display/colorDayFrame"),"QColor");
    addKey(QString("setDisplay_colorNightFrame"),QString("display/colorNightFrame"),"QColor");
    addKey(QString("setDisplay_colorSectionsDay"),QString("display/colorSectionsDay"),"QColor");
    addKey(QString("setDisplay_colorFieldDay"),QString("display/colorFieldDay"),"QColor");
    addKey(QString("setDisplay_isDayMode"),QString("display/dayMode"),"bool");
    addKey(QString("setDisplay_colorSectionsNight"),QString("display/colorSectionsNight"),"QColor");
    addKey(QString("setDisplay_colorFieldNight"),QString("display/colorFieldNight"),"QColor");
    addKey(QString("setDisplay_isAutoDayNight"),QString("display/autoDayNight"),"bool");
    addKey(QString("setDisplay_customColors"),QString("display/customColors"),"QString");
    addKey(QString("setDisplay_isTermsAccepted"),QString("display/terms"),"bool");
    addKey(QString("setGPS_isRTK"),QString("gps/isRTK"),"bool");
    addKey(QString("setDisplay_isStartFullScreen"),QString("display/startFullscreen"),"bool");
    addKey(QString("setDisplay_isKeyboardOn"),QString("display/keyboard"),"bool");
    addKey(QString("setIMU_rollFilter"),QString("gps/IMURollFilter"),"double");
    addKey(QString("setAS_uTurnSmoothing"),QString("autosteer/uTurnSmoothing"),"int");
    addKey(QString("setIMU_invertRoll"),QString("gps/IMUInvertRoll"),"bool");
    addKey(QString("setAS_ackerman"),QString("autosteer/ackerman"),"double");
    addKey(QString("setF_isWorkSwitchActiveLow"),QString("tool/isWorkSwitchActiveLow"),"bool");
    addKey(QString("setAS_Kp"),QString("autosteer/Kp"),"double");
    addKey(QString("setSound_isUturnOn"),QString("display/soundUturn"),"bool");
    addKey(QString("setSound_isHydLiftOn"),QString("display/soundHydLift"),"bool");
    addKey(QString("setDisplay_colorTextNight"),QString("display/colorTextNight"),"QColor");
    addKey(QString("setDisplay_colorTextDay"),QString("display/colorTextDay"),"QColor");
    addKey(QString("setTram_isTramOnBackBuffer"),QString("tram/tramOnBackBuffer"),"bool");
    addKey(QString("setDisplay_camZoom"),QString("display/camZoom"),"double");
    addKey(QString("setDisplay_colorVehicle"),QString("display/vehicleColor"),"QColor");
    addKey(QString("setDisplay_vehicleOpacity"),QString("display/vehicleOpacity"),"int");
    addKey(QString("setDisplay_isVehicleImage"),QString("display/isVehicleImage"),"bool");
    addKey(QString("setIMU_isHeadingCorrectionFromAutoSteer"),QString("gps/isHeadingCorrectionFromAutoSteer"),"QString");
    addKey(QString("setDisplay_isTextureOn"),QString("display/texture"),"bool");
    addKey(QString("setAB_lineLength"),QString("ABLine/lineLength"),"double");
    addKey(QString("setGPS_udpWatchMsec"),QString("gps/udpWatchMS"),"int");
    addKey(QString("setF_isSteerWorkSwitchManualSections"),QString("tool/isWorkSwitchManualSections"),"bool");
    addKey(QString("setAS_isConstantContourOn"),QString("autosteer/constantContourOn"),"bool");
    addKey(QString("setAS_guidanceLookAheadTime"),QString("autosteer/guidanceLookAheadTime"),"double");
    addKey(QString("setFeatures"),QString("display/features"),"QString");
    addKey(QString("setIMU_isDualAsIMU"),QString("gps/dualAsIMU"),"bool");
    addKey(QString("setAS_sideHillComp"),QString("gps/sideHillCompensation"),"double");
    addKey(QString("setIMU_isReverseOn"),QString("gps/revereOn"),"bool");
    addKey(QString("setGPS_forwardComp"),QString("gps/forwardComp"),"double");
    addKey(QString("setGPS_reverseComp"),QString("gps/reverseComp"),"double");
    addKey(QString("setGPS_ageAlarm"),QString("gps/ageAlarm"),"int");
    addKey(QString("setGPS_isRTK_KillAutoSteer"),QString("gps/RTKKillAutoSteer"),"bool");
    addKey(QString("setColor_sec01"),QString("display/section1Color"),"QColor");
    addKey(QString("setColor_sec02"),QString("display/section2Color"),"QColor");
    addKey(QString("setColor_sec03"),QString("display/section3Color"),"QColor");
    addKey(QString("setColor_sec04"),QString("display/section4Color"),"QColor");
    addKey(QString("setColor_sec05"),QString("display/section5Color"),"QColor");
    addKey(QString("setColor_sec06"),QString("display/section6Color"),"QColor");
    addKey(QString("setColor_sec07"),QString("display/section7Color"),"QColor");
    addKey(QString("setColor_sec08"),QString("display/section8Color"),"QColor");
    addKey(QString("setColor_sec09"),QString("display/section9Color"),"QColor");
    addKey(QString("setColor_sec10"),QString("display/section10Color"),"QColor");
    addKey(QString("setColor_sec11"),QString("display/section11Color"),"QColor");
    addKey(QString("setColor_sec12"),QString("display/section12Color"),"QColor");
    addKey(QString("setColor_sec13"),QString("display/section13Color"),"QColor");
    addKey(QString("setColor_sec14"),QString("display/section14Color"),"QColor");
    addKey(QString("setColor_sec15"),QString("display/section15Color"),"QColor");
    addKey(QString("setColor_sec16"),QString("display/section16Color"),"QColor");
    addKey(QString("setColor_isMultiColorSections"),QString("display/multiColorSections"),"bool");
    addKey(QString("setDisplay_customSectionColors"),QString("display/customSectionColors"),"QString");
    addKey(QString("setBrand_TBrand"),QString("display/TBrand"),"QString");
    addKey(QString("setHeadland_isSectionControlled"),QString("todo/headlandIsSectionControlled"),"bool");
    addKey(QString("setSound_isAutoSteerOn"),QString("display/autoSteerSound"),"bool");
    addKey(QString("setRelay_pinConfig"),QString("todo/relayPinConfig"),"leavealone");
    addKey(QString("setDisplay_camSmooth"),QString("display/camSmooth"),"int");
    addKey(QString("setGPS_dualHeadingOffset"),QString("gps/dualHeadingOffset"),"double");
    addKey(QString("setF_isSteerWorkSwitchEnabled"),QString("tool/isSteerWorkSwitchEnabled"),"bool");
    addKey(QString("setF_isRemoteWorkSystemOn"),QString("tool/isRemoteWorkSystemOn"),"bool");
    addKey(QString("setDisplay_isAutoStartAgIO"),QString("display/autoStartAgIO"),"bool");
    addKey(QString("setAS_ModeXTE"),QString("autosteer/modeXTE"),"double");
    addKey(QString("setAS_ModeTime"),QString("autosteer/modeTime"),"int");
    addKey(QString("setVehicle_toolWidth"),QString("tool/width"),"double");
    addKey(QString("setVehicle_toolOverlap"),QString("tool/overlap"),"double");
    addKey(QString("setTool_toolTrailingHitchLength"),QString("tool/trailingHitchLength"),"double");
    addKey(QString("setVehicle_numSections"),QString("tool/numSections"),"int");
    addKey(QString("setSection_position1"),QString("tool/sectionposition1"),"double");
    addKey(QString("setSection_position2"),QString("tool/sectionposition2"),"double");
    addKey(QString("setSection_position3"),QString("tool/sectionposition3"),"double");
    addKey(QString("setSection_position4"),QString("tool/sectionposition4"),"double");
    addKey(QString("setSection_position5"),QString("tool/sectionposition5"),"double");
    addKey(QString("setSection_position6"),QString("tool/sectionposition6"),"double");
    addKey(QString("setSection_position7"),QString("tool/sectionposition7"),"double");
    addKey(QString("setSection_position8"),QString("tool/sectionposition8"),"double");
    addKey(QString("setSection_position9"),QString("tool/sectionposition9"),"double");
    addKey(QString("setSection_position10"),QString("tool/sectionposition10"),"double");
    addKey(QString("setSection_position11"),QString("tool/sectionposition11"),"double");
    addKey(QString("setSection_position12"),QString("tool/sectionposition12"),"double");
    addKey(QString("setSection_position13"),QString("tool/sectionposition13"),"double");
    addKey(QString("setSection_position14"),QString("tool/sectionposition14"),"double");
    addKey(QString("setSection_position15"),QString("tool/sectionposition15"),"double");
    addKey(QString("setSection_position16"),QString("tool/sectionposition16"),"double");
    addKey(QString("setSection_position17"),QString("tool/sectionposition17"),"double");
    addKey(QString("purePursuitIntegralGainAB"),QString("vehicle/purePursuitIntegralGainAB"),"double");
    addKey(QString("set_youMoveDistance"),QString("todo/youMoveDistance"),"double");
    addKey(QString("setVehicle_antennaHeight"),QString("vehicle/antennaHeight"),"double");
    addKey(QString("setVehicle_toolLookAheadOn"),QString("tool/lookAheadOn"),"double");
    addKey(QString("setTool_isToolTrailing"),QString("tool/isTrailing"),"bool");
    addKey(QString("setVehicle_toolOffset"),QString("tool/offset"),"double");
    addKey(QString("setTool_isToolRearFixed"),QString("tool/rearFixed"),"bool");
    addKey(QString("setVehicle_antennaPivot"),QString("vehicle/antennaPivot"),"double");
    addKey(QString("setVehicle_wheelbase"),QString("vehicle/wheelbase"),"double");
    addKey(QString("setVehicle_hitchLength"),QString("vehicle/hitchLength"),"double");
    addKey(QString("setVehicle_toolLookAheadOff"),QString("tool/lookAheadOff"),"double");
    addKey(QString("setVehicle_isPivotBehindAntenna"),QString("vehicle/isPivotBehindAntenna"),"bool");
    addKey(QString("setVehicle_isSteerAxleAhead"),QString("vehicle/isSteerAxleAhead"),"bool");
    addKey(QString("setVehicle_slowSpeedCutoff"),QString("vehicle/slowSpeedCutoff"),"double");
    addKey(QString("setVehicle_tankTrailingHitchLength"),QString("tool/tankTrailingHitchLength"),"double");
    addKey(QString("setVehicle_minCoverage"),QString("vehicle/minCoverage"),"int");
    addKey(QString("setVehicle_goalPointLookAhead"),QString("vehicle/goalPointLookAhead"),"double");
    addKey(QString("setVehicle_maxAngularVelocity"),QString("vehicle/maxAngularVelocity"),"double");
    addKey(QString("setVehicle_maxSteerAngle"),QString("vehicle/maxSteerAngle"),"double");
    addKey(QString("set_youTurnExtensionLength"),QString("uturn/extensionLength"),"int");
    addKey(QString("set_youToolWidths"),QString("yturn/toolWidths"),"double");
    addKey(QString("setVehicle_minTurningRadius"),QString("vehicle/minTurningRadius"),"double");
    addKey(QString("setVehicle_antennaOffset"),QString("vehicle/antennaOffset"),"double");
    addKey(QString("set_youTurnDistanceFromBoundary"),QString("uturn/distanceFromBoundary"),"double");
    addKey(QString("setVehicle_lookAheadMinimum"),QString("vehicle/lookAheadMinimum"),"double");
    addKey(QString("setVehicle_goalPointLookAheadMult"),QString("vehicle/goalPointLookAheadMult"),"double");
    addKey(QString("stanleyDistanceErrorGain"),QString("vehicle/stanleyDistanceErrorGain"),"double");
    addKey(QString("stanleyHeadingErrorGain"),QString("vehicle/stanleyHeadingErrorGain"),"double");
    addKey(QString("setVehicle_isStanleyUsed"),QString("vehicle/isStanlyUsed"),"bool");
    addKey(QString("setTram_BasedOn"),QString("tram/basedOn"),"int");
    addKey(QString("setTram_Skips"),QString("tram/skips"),"int");
    addKey(QString("setTool_isToolTBT"),QString("tool/isTBT"),"bool");
    addKey(QString("setVehicle_vehicleType"),QString("vehicle/type"),"int");
    addKey(QString("set_youSkipWidth"),QString("uturn/skipWidth"),"int");
    addKey(QString("setArdSteer_setting1"),QString("autosteer/ardSteerSetting1"),"double");
    addKey(QString("setArdSteer_minSpeed"),QString("autosteer/ardSteerMinSpeed"),"double");
    addKey(QString("setArdSteer_maxSpeed"),QString("autosteer/ardSteerMaxSpeed"),"double");
    addKey(QString("setArdSteer_setting0"),QString("autosteer/ardSteerSetting0"),"double");
    addKey(QString("setVehicle_hydraulicLiftLookAhead"),QString("vehicle/hydraulicLiftLookAhead"),"double");
    addKey(QString("setVehicle_isMachineControlToAutoSteer"),QString("vehicle/isMachineControlToAutoSteer"),"bool");
    addKey(QString("setArdSteer_maxPulseCounts"),QString("autosteer/ardSteerMaxPulseCounts"),"double");
    addKey(QString("setArdMac_hydRaiseTime"),QString("todo/ardMacHydRaiseTime"),"double");
    addKey(QString("setArdMac_hydLowerTime"),QString("todo/ardMacHydLowerTime"),"double");
    addKey(QString("setArdMac_isHydEnabled"),QString("todo/ardMacIsHydEnabled"),"double");
    addKey(QString("setTool_defaultSectionWidth"),QString("tool/defaultSectionWidth"),"double");
    addKey(QString("setVehicle_toolOffDelay"),QString("tool/offDelay"),"double");
    addKey(QString("setArdMac_setting0"),QString("todo/ardMacSetting0"),"double");
    addKey(QString("setArdSteer_setting2"),QString("todo/ArdSteerSetting2"),"double");
    addKey(QString("stanleyIntegralDistanceAwayTriggerAB"),QString("vehicle/stanleyIntegralDistanceAwayTriggerAB"),"double");
    addKey(QString("setTool_isToolFront"),QString("tool/isFront"),"bool");
    addKey(QString("setVehicle_trackWidth"),QString("vehicle/trackWidth"),"double");
    addKey(QString("setArdMac_isDanfoss"),QString("todo/ardMacIsDanFoss"),"bool");
    addKey(QString("stanleyIntegralGainAB"),QString("vehicle/stanleyIntegralGainAB"),"double");
    addKey(QString("setSection_isFast"),QString("tool/sectionIsFast"),"bool");
    addKey(QString("setArdMac_user1"),QString("todo/ardMacUser1"),"double");
    addKey(QString("setArdMac_user2"),QString("todo/ardMacUser2"),"double");
    addKey(QString("setArdMac_user3"),QString("todo/ardMacUser3"),"double");
    addKey(QString("setArdMac_user4"),QString("todo/ardMacUser4"),"double");
    addKey(QString("setVehicle_panicStopSpeed"),QString("vehicle/panicStopSpeed"),"double");
    addKey(QString("setAS_ModeMultiplierStanley"),QString("autosteer/modeMultiplierStanley"),"double");
    addKey(QString("setDisplay_brightness"),QString("display/brightness"),"int");
    addKey(QString("set_youTurnRadius"),QString("uturn/radius"),"double");
    addKey(QString("setDisplay_brightnessSystem"),QString("display/brightnessSystem"),"int");
    addKey(QString("setTool_isSectionsNotZones"),QString("tool/sectionsNotZones"),"bool");
    addKey(QString("setTool_numSectionsMulti"),QString("tool/numSectionsMulti"),"int");
    addKey(QString("setTool_zones"),QString("tool/zones"),"leavealone");
    addKey(QString("setTool_sectionWidthMulti"),QString("tool/sectionWidthMulti"),"double");
    addKey(QString("setDisplay_isBrightnessOn"),QString("display/brightnessOn"),"bool");
    addKey(QString("setKey_hotkeys"),QString("display/hotKeys"),"QString");
    addKey(QString("setVehicle_goalPointLookAheadHold"),QString("vehicle/goalPointLookAheadHold"),"double");
    addKey(QString("setTool_isSectionOffWhenOut"),QString("tool/isSectionOffWhenOut"),"bool");
    addKey(QString("set_uTurnStyle"),QString("uturn/style"),"int");
    addKey(QString("setGPS_minimumStepLimit"),QString("gps/minimumStepLimit"),"double");
    addKey(QString("setAS_isSteerInReverse"),QString("autosteer/isSteerInReverse"),"bool");
    addKey(QString("setAS_functionSpeedLimit"),QString("autosteer/functionSpeedLimit"),"double");
    addKey(QString("setAS_maxSteerSpeed"),QString("autosteer/maxSteerSpeed"),"double");
    addKey(QString("setAS_minSteerSpeed"),QString("autosteer/minSteerSpeed"),"double");
    addKey(QString("setBrand_HBrand"),QString("display/HBrand"),"QString");
    addKey(QString("setBrand_WDBrand"),QString("display/WDBrand"),"QString");
    addKey(QString("setIMU_fusionWeight2"),QString("gps/IMUFusionWeight2"),"double");
    addKey(QString("setDisplay_isSvennArrowOn"),QString("display/svennArrow"),"bool");
    addKey(QString("setTool_isTramOuterInverted"),QString("tool/isTramOuterInverted"),"bool");
    addKey(QString("setJobMenu_location"),QString("display/jobMenuLocation"),"QPoint");
    addKey(QString("setJobMenu_size"),QString("display/jobMenuSize"),"QString");
    addKey(QString("setWindow_steerSettingsLocation"),QString("display/steerSettingsLocation"),"QPoint");
    addKey(QString("setWindow_buildTracksLocation"),QString("dialogs/tracksLocation"),"QPoint");
    addKey(QString("setTool_trailingToolToPivotLength"),QString("tool/trailingToPivotLength"),"double");
    addKey(QString("setWindow_formNudgeLocation"),QString("dialogs/nudgeLocation"),"QPoint");
    addKey(QString("setWindow_formNudgeSize"),QString("autosteer/nudgeSize"),"QString");
    addKey(QString("setAS_snapDistanceRef"),QString("autosteer/snapDistanceRef"),"double");
    addKey(QString("setDisplay_buttonOrder"),QString("display/buttonOrder"),"QString");
    addKey(QString("setDisplay_camPitch"),QString("display/camPitch"),"double");
    addKey(QString("setWindow_abDrawSize"),QString("dialogs/abDrawSize"),"QString");
    addKey(QString("setWindow_HeadlineSize"),QString("dialogs/headlineSize"),"QString");
    addKey(QString("setWindow_HeadAcheSize"),QString("dialogs/headAcheSize"),"QString");
    addKey(QString("setWindow_MapBndSize"),QString("dialogs/mapBndSize"),"QString");
    addKey(QString("setWindow_BingMapSize"),QString("dialogs/bingMapSize"),"QString");
    addKey(QString("setWindow_BingZoom"),QString("dialogs/bingZoom"),"int");
    addKey(QString("setWindow_RateMapSize"),QString("dialogs/rateMapSize"),"QString");
    addKey(QString("setWindow_RateMapZoom"),QString("dialogs/rateMapZoom"),"int");
    addKey(QString("setWindow_QuickABLocation"),QString("dialogs/quickABLocation"),"QPoint");
    addKey(QString("setDisplay_isLogElevation"),QString("display/isLogElevation"),"bool");
    addKey(QString("setSound_isSectionsOn"),QString("display/soundIsSectionOn"),"bool");
    addKey(QString("setGPS_dualReverseDetectionDistance"),QString("gps/dualReverseDetectionDistance"),"double");
    addKey(QString("setTool_isDisplayTramControl"),QString("display/showTramControl"),"bool");
    addKey(QString("setAS_uTurnCompensation"),QString("autosteer/uTurnCompensation"),"double");
    addKey(QString("setWindow_gridSize"),QString("dialogs/gridSize"),"QString");
    addKey(QString("setWindow_gridLocation"),QString("dialogs/gridLocation"),"QPoint");
    addKey(QString("setWindow_isKioskMode"),QString("dialogs/isKioskMode"),"bool");
    addKey(QString("setDisplay_isAutoOffAgIO"),QString("display/isAutoOffAgIO"),"bool");
    addKey(QString("setWindow_isShutdownComputer"),QString("dialogs/isShutdownComputer"),"bool");
    addKey(QString("setDisplay_isShutdownWhenNoPower"),QString("display/isShutDownWhenNoPower"),"bool");
    addKey(QString("setDisplay_isHardwareMessages"),QString("display/isHardwareMessages"),"bool");
    addKey(QString("setGPS_jumpFixAlarmDistance"),QString("gps/jumpFixAlarmDispance"),"int");
    addKey(QString("setAS_deadZoneDistance"),QString("autosteer/deadZoneDistance"),"int");
    addKey(QString("setAS_deadZoneHeading"),QString("autosteer/deadZoneHeading"),"int");
    addKey(QString("setMenu_isLightbarNotSteerBar"),QString("display/isLightBarNotSteerBar"),"bool");
    addKey(QString("setTool_isDirectionMarkers"),QString("tool/isDirectionMarkers"),"bool");
    addKey(QString("setAS_numGuideLines"),QString("autosteer/numGuideLines"),"int");
    addKey(QString("setAS_deadZoneDelay"),QString("autosteer/deadZoneDelay"),"int");
    addKey(QString("setTram_alpha"),QString("tram/alpha"),"double");
    addKey(QString("setVehicle_goalPointAcquireFactor"),QString("vehicle/goalPointAcquireFactor"),"double");
    addKey(QString("setBnd_isDrawPivot"),QString(""),"bool");
    addKey(QString("setDisplay_isSectionLinesOn"),QString(""),"bool");
    addKey(QString("setDisplay_isLineSmooth"),QString(""),"bool");
    addKey(QString("setWindow_tramLineSize"),QString(""),"QString");
    addKey(QString("setBnd_isDrawPivot"),QString("bnd/isDrawPivot"),"bool");
    addKey(QString("setDisplay_isSectionLinesOn"),QString("display/isSectionLinesOn"),"bool");
    addKey(QString("setDisplay_isLineSmooth"),QString("display/isLineSmooth"),"bool");
    addKey(QString("setWindow_tramLineSize"),QString("dialogs/tramLineSize"),"QString");
    addKey(QString("setFeature_isHeadlandOn"),QString("displayFeatures/isHeadlandOn"), "bool");
    addKey(QString("setFeature_isTramOn"),QString("displayFeatures/isTramOn"), "bool");
    addKey(QString("setFeature_isBoundaryOn"),QString("displayFeatures/isBoundaryOn"), "bool");
    addKey(QString("setFeature_isBndContourOn"),QString("displayFeatures/isBndContourOn"), "bool");
    addKey(QString("setFeature_isRecPathOn"),QString("displayFeatures/isRecPathOn"), "bool");
    addKey(QString("setFeature_isABSmoothOn"),QString("displayFeatures/isABSmoothOn"), "bool");
    addKey(QString("setFeature_isHideContourOn"),QString("displayFeatures/isHideContourOn"), "bool");
    addKey(QString("setFeature_isWebCamOn"),QString("displayFeatures/isWebCamOn"), "bool");
    addKey(QString("setFeature_isOffsetFixOn"),QString("displayFeatures/isOffsetFixOn"), "bool");
    addKey(QString("setFeature_isAgIOOn"),QString("displayFeatures/isAgIOOn"), "bool");
    addKey(QString("setFeature_isContourOn"),QString("displayFeatures/isContourOn"), "bool");
    addKey(QString("setFeature_isYouTurnOn"),QString("displayFeatures/isYouTurnOn"), "bool");
    addKey(QString("setFeature_isSteerModeOn"),QString("displayFeatures/isSteerModeOn"), "bool");
    addKey(QString("setFeature_isManualSectionOn"),QString("displayFeatures/isManualSectionOn"), "bool");
    addKey(QString("setFeature_isAutoSectionOn"),QString("displayFeatures/isAutoSectionOn"), "bool");
    addKey(QString("setFeature_isCycleLinesOn"),QString("displayFeatures/isCycleLinesOn"), "bool");
    addKey(QString("setFeature_isABLineOn"),QString("displayFeatures/isABLineOn"), "bool");
    addKey(QString("setFeature_isCurveOn"),QString("displayFeatures/isCurveOn"), "bool");
    addKey(QString("setFeature_isAutoSteerOn"),QString("displayFeatures/isAutoSteerOn"), "bool");
    addKey(QString("setFeature_isUTurnOn"),QString("displayFeatures/isUTurnOn"), "bool");
    addKey(QString("setFeature_isLateralOn"),QString("displayFeatures/isLateralOn"), "bool");
    addKey(QString("setDisplay_useTrackZero"),QString("display/useTrackZero"),"bool");
    addKey(QString("setDisplay_topTrackNum"),QString("display/topTrackNum"),"bool");
    addKey(QString("setSeed_blockCountMin"),QString("seed/blockCountMin"),"double");
    addKey(QString("setSeed_blockCountMax"),QString("seed/blockCountMax"),"double");
    addKey(QString("setSeed_blockRow1"),QString("seed/blockRow1"),"int");
    addKey(QString("setSeed_blockRow2"),QString("seed/blockRow2"),"int");
    addKey(QString("setSeed_blockRow3"),QString("seed/blockRow3"),"int");
    addKey(QString("setSeed_blockRow4"),QString("seed/blockRow4"),"int");
    addKey(QString("setSeed_numRows"),QString("seed/numRows"),"int");
    addKey(QString("setSeed_blockageIsOn"),QString("seed/blockageIsOn"),"bool");
}
