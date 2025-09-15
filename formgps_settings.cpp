// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// This loads the setting (or some of them) into variables, that we can access later
#include "formgps.h"
#include "classes/settingsmanager.h"

void FormGPS::loadSettings()
{
    isMetric = SettingsManager::instance()->value(SETTINGS_menu_isMetric).value<bool>();

    isUTurnOn = SettingsManager::instance()->value(SETTINGS_feature_isUTurnOn).value<bool>();
    isLateralOn = SettingsManager::instance()->value(SETTINGS_feature_isLateralOn).value<bool>();

    pn.headingTrueDualOffset = SettingsManager::instance()->value(SETTINGS_gps_dualHeadingOffset).value<double>();

    frameDayColor = SettingsManager::instance()->value(SETTINGS_display_colorDayFrame).value<QColor>();
    frameNightColor = SettingsManager::instance()->value(SETTINGS_display_colorNightFrame).value<QColor>();
    sectionColorDay = SettingsManager::instance()->value(SETTINGS_display_colorSectionsDay).value<QColor>();
    fieldColorDay = SettingsManager::instance()->value(SETTINGS_display_colorFieldDay).value<QColor>();
    fieldColorNight = SettingsManager::instance()->value(SETTINGS_display_colorFieldNight).value<QColor>();

    //check color for 255, reset it to properties
    //Properties.Settings.Default.setDisplay_colorDayFrame = frameDayColor;
    //Properties.Settings.Default.setDisplay_colorNightFrame = frameNightColor;
    //Properties.Settings.Default.setDisplay_colorSectionsDay = sectionColorDay;
    //Properties.Settings.Default.setDisplay_colorFieldDay = fieldColorDay;
    //Properties.Settings.Default.setDisplay_colorFieldNight = fieldColorNight;

    isSkyOn = SettingsManager::instance()->value(SETTINGS_menu_isSkyOn).value<bool>();
    isTextureOn = SettingsManager::instance()->value(SETTINGS_display_isTextureOn).value<bool>();

    isGridOn = SettingsManager::instance()->value(SETTINGS_menu_isGridOn).value<bool>();
    isBrightnessOn = SettingsManager::instance()->value(SETTINGS_display_isBrightnessOn).value<bool>();

    isCompassOn = SettingsManager::instance()->value(SETTINGS_menu_isCompassOn).value<bool>();
    isSpeedoOn = SettingsManager::instance()->value(SETTINGS_menu_isSpeedoOn).value<bool>();
    isSideGuideLines = SettingsManager::instance()->value(SETTINGS_menu_isSideGuideLines).value<bool>();
    isSvennArrowOn = SettingsManager::instance()->value(SETTINGS_display_isSvennArrowOn).value<bool>();

    lightbarCmPerPixel = SettingsManager::instance()->value(SETTINGS_display_lightbarCmPerPixel).value<int>();

    //isLogNMEA = SettingsManager::instance()->value("menu/isLogNMEA;
    isPureDisplayOn = SettingsManager::instance()->value(SETTINGS_menu_isPureOn).value<bool>();

    isAutoStartAgIO = SettingsManager::instance()->value(SETTINGS_display_isAutoStartAgIO).value<bool>();

    vehicleOpacity = SettingsManager::instance()->value(SETTINGS_display_vehicleOpacity).value<double>() * 0.01;
    vehicleOpacityByte = (char)(255 * (SettingsManager::instance()->value(SETTINGS_display_vehicleOpacity).value<double>() * 0.01));
    isVehicleImage = SettingsManager::instance()->value(SETTINGS_display_isVehicleImage).value<bool>();

    //TODO: custom colors for display

    //TODO: check for 255
    textColorDay = SettingsManager::instance()->value(SETTINGS_display_colorTextDay).value<QColor>();
    textColorNight = SettingsManager::instance()->value(SETTINGS_display_colorTextNight).value<QColor>();

    vehicleColor = SettingsManager::instance()->value(SETTINGS_display_colorVehicle).value<QColor>();

    isLightbarOn = SettingsManager::instance()->value(SETTINGS_menu_isLightBarOn).value<bool>();


    //hotkeys = Properties.Settings.Default.setKey_hotkeys.ToCharArray();
    // udpWatchLimit = SettingsManager::instance()->value(SETTINGS_gps_udpWatchMSec).value<int>(); // ❌ REMOVED - Phase 4.6: No UDP FormGPS
    //check for 255
    //TODO
    //string[] words = Properties.Settings.Default.setDisplay_customColors.Split(',');

    isRTK = SettingsManager::instance()->value(SETTINGS_gps_isRTK).value<bool>();
    isRTK_KillAutosteer = SettingsManager::instance()->value(SETTINGS_gps_isRTKKillAutoSteer).value<bool>();

    pn.ageAlarm = SettingsManager::instance()->value(SETTINGS_gps_ageAlarm).value<int>();

    isConstantContourOn = SettingsManager::instance()->value(SETTINGS_as_isConstantContourOn).value<bool>();
    isSteerInReverse = SettingsManager::instance()->value(SETTINGS_as_isSteerInReverse).value<bool>();

    guidanceLookAheadTime = SettingsManager::instance()->value(SETTINGS_as_guidanceLookAheadTime).value<double>();

    //gyd pulls directly from settings
    //gyd.sideHillCompFactor = property_setAS_sideHillComp;

    fd.UpdateFieldBoundaryGUIAreas(bnd.bndList);

    isStanleyUsed = SettingsManager::instance()->value(SETTINGS_vehicle_isStanleyUsed).value<bool>();
    isDay = SettingsManager::instance()->value(SETTINGS_display_isDayMode).value<bool>();

    tool.loadSettings();
    if (tool.isSectionsNotZones){
        tool.sectionSetPositions();
        tool.sectionCalcWidths();
    } else {
        tool.sectionCalcMulti();
    }

    //disable youturn buttons
    headingFromSource = SettingsManager::instance()->value(SETTINGS_gps_headingFromWhichSource).value<QString>();

    //load various saved settings or properties into the support classes
    ahrs.loadSettings();
    camera.loadSettings();
    pn.loadSettings();
    if(!isJobStarted)
        sim.loadSettings();
    CVehicle::instance()->loadSettings();
    yt.loadSettings();
}
