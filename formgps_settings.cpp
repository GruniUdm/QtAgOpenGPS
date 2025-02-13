// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// This loads the setting (or some of them) into variables, that we can access later
#include "formgps.h"
#include "newsettings.h"

void FormGPS::loadSettings()
{
    isMetric = settings->value(SETTINGS_menu_isMetric).value<bool>();

    isUTurnOn = settings->value(SETTINGS_feature_isUTurnOn).value<bool>();
    isLateralOn = settings->value(SETTINGS_feature_isLateralOn).value<bool>();

    pn.headingTrueDualOffset = settings->value(SETTINGS_gps_dualHeadingOffset).value<double>();

    frameDayColor = settings->value(SETTINGS_display_colorDayFrame).value<QColor>();
    frameNightColor = settings->value(SETTINGS_display_colorNightFrame).value<QColor>();
    sectionColorDay = settings->value(SETTINGS_display_colorSectionsDay).value<QColor>();
    fieldColorDay = settings->value(SETTINGS_display_colorFieldDay).value<QColor>();
    fieldColorNight = settings->value(SETTINGS_display_colorFieldNight).value<QColor>();

    //check color for 255, reset it to properties
    //Properties.Settings.Default.setDisplay_colorDayFrame = frameDayColor;
    //Properties.Settings.Default.setDisplay_colorNightFrame = frameNightColor;
    //Properties.Settings.Default.setDisplay_colorSectionsDay = sectionColorDay;
    //Properties.Settings.Default.setDisplay_colorFieldDay = fieldColorDay;
    //Properties.Settings.Default.setDisplay_colorFieldNight = fieldColorNight;

    isSkyOn = settings->value(SETTINGS_menu_isSkyOn).value<bool>();
    isTextureOn = settings->value(SETTINGS_display_isTextureOn).value<bool>();

    isGridOn = settings->value(SETTINGS_menu_isGridOn).value<bool>();
    isBrightnessOn = settings->value(SETTINGS_display_isBrightnessOn).value<bool>();

    isCompassOn = settings->value(SETTINGS_menu_isCompassOn).value<bool>();
    isSpeedoOn = settings->value(SETTINGS_menu_isSpeedoOn).value<bool>();
    isSideGuideLines = settings->value(SETTINGS_menu_isSideGuideLines).value<bool>();
    isSvennArrowOn = settings->value(SETTINGS_display_isSvennArrowOn).value<bool>();

    lightbarCmPerPixel = settings->value(SETTINGS_display_lightbarCmPerPixel).value<int>();

    //isLogNMEA = settings->value("menu/isLogNMEA;
    isPureDisplayOn = settings->value(SETTINGS_menu_isPureOn).value<bool>();

    isAutoStartAgIO = settings->value(SETTINGS_display_isAutoStartAgIO).value<bool>();

    vehicleOpacity = settings->value(SETTINGS_display_vehicleOpacity).value<double>() * 0.01;
    vehicleOpacityByte = (char)(255 * (settings->value(SETTINGS_display_vehicleOpacity).value<double>() * 0.01));
    isVehicleImage = settings->value(SETTINGS_display_isVehicleImage).value<bool>();

    //TODO: custom colors for display

    //TODO: check for 255
    textColorDay = settings->value(SETTINGS_display_colorTextDay).value<QColor>();
    textColorNight = settings->value(SETTINGS_display_colorTextNight).value<QColor>();

    vehicleColor = settings->value(SETTINGS_display_colorVehicle).value<QColor>();

    isLightbarOn = settings->value(SETTINGS_menu_isLightBarOn).value<bool>();


    //hotkeys = Properties.Settings.Default.setKey_hotkeys.ToCharArray();
    udpWatchLimit = settings->value(SETTINGS_gps_udpWatchMSec).value<int>();
    //check for 255
    //TODO
    //string[] words = Properties.Settings.Default.setDisplay_customColors.Split(',');

    isRTK = settings->value(SETTINGS_gps_isRTK).value<bool>();
    isRTK_KillAutosteer = settings->value(SETTINGS_gps_isRTKKillAutoSteer).value<bool>();

    pn.ageAlarm = settings->value(SETTINGS_gps_ageAlarm).value<int>();

    isConstantContourOn = settings->value(SETTINGS_as_isConstantContourOn).value<bool>();
    isSteerInReverse = settings->value(SETTINGS_as_isSteerInReverse).value<bool>();

    guidanceLookAheadTime = settings->value(SETTINGS_as_guidanceLookAheadTime).value<double>();

    //gyd pulls directly from settings
    //gyd.sideHillCompFactor = property_setAS_sideHillComp;

    fd.UpdateFieldBoundaryGUIAreas(bnd.bndList);

    isStanleyUsed = settings->value(SETTINGS_vehicle_isStanleyUsed).value<bool>();
    isDay = settings->value(SETTINGS_display_isDayMode).value<bool>();

    tool.loadSettings();
    if (tool.isSectionsNotZones){
        tool.sectionSetPositions();
        tool.sectionCalcWidths();
    } else {
        tool.sectionCalcMulti();
    }

    //disable youturn buttons
    headingFromSource = settings->value(SETTINGS_gps_headingFromWhichSource).value<QString>();

    //load various saved settings or properties into the support classes
    ahrs.loadSettings();
    camera.loadSettings();
    pn.loadSettings();
    if(!isJobStarted)
        sim.loadSettings();
    vehicle.loadSettings();
    yt.loadSettings();
}
