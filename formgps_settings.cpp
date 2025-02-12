// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// This loads the setting (or some of them) into variables, that we can access later
#include "formgps.h"
#include "newsettings.h"

void FormGPS::loadSettings()
{
    isMetric = settings->value("menu/isMetric").value<bool>();

    isUTurnOn = settings->value("feature/isUTurnOn").value<bool>();
    isLateralOn = settings->value("feature/isLateralOn").value<bool>();

    pn.headingTrueDualOffset = settings->value("gps/dualHeadingOffset").value<double>();

    frameDayColor = settings->value("display/colorDayFrame").value<QColor>();
    frameNightColor = settings->value("display/colorNightFrame").value<QColor>();
    sectionColorDay = settings->value("display/colorSectionsDay").value<QColor>();
    fieldColorDay = settings->value("display/colorFieldDay").value<QColor>();
    fieldColorNight = settings->value("display/colorFieldNight").value<QColor>();

    //check color for 255, reset it to properties
    //Properties.Settings.Default.setDisplay_colorDayFrame = frameDayColor;
    //Properties.Settings.Default.setDisplay_colorNightFrame = frameNightColor;
    //Properties.Settings.Default.setDisplay_colorSectionsDay = sectionColorDay;
    //Properties.Settings.Default.setDisplay_colorFieldDay = fieldColorDay;
    //Properties.Settings.Default.setDisplay_colorFieldNight = fieldColorNight;

    isSkyOn = settings->value("menu/isSkyOn").value<bool>();
    isTextureOn = settings->value("display/isTextureOn").value<bool>();

    isGridOn = settings->value("menu/isGridOn").value<bool>();
    isBrightnessOn = settings->value("display/isBrightnessOn").value<bool>();

    isCompassOn = settings->value("menu/isCompassOn").value<bool>();
    isSpeedoOn = settings->value("menu/isSpeedoOn").value<bool>();
    isSideGuideLines = settings->value("menu/isSideGuideLines").value<bool>();
    isSvennArrowOn = settings->value("display/isSvennArrowOn").value<bool>();

    lightbarCmPerPixel = settings->value("display/lightbarCmPerPixel").value<int>();

    //isLogNMEA = settings->value("menu/isLogNMEA;
    isPureDisplayOn = settings->value("menu/isPureOn").value<bool>();

    isAutoStartAgIO = settings->value("display/isAutoStartAgIO").value<bool>();

    vehicleOpacity = settings->value("display/vehicleOpacity").value<double>() * 0.01;
    vehicleOpacityByte = (char)(255 * (settings->value("display/vehicleOpacity").value<double>() * 0.01));
    isVehicleImage = settings->value("display/isVehicleImage").value<bool>();

    //TODO: custom colors for display

    //TODO: check for 255
    textColorDay = settings->value("display/colorTextDay").value<QColor>();
    textColorNight = settings->value("display/colorTextNight").value<QColor>();

    vehicleColor = settings->value("display/colorVehicle").value<QColor>();

    isLightbarOn = settings->value("menu/isLightBarOn").value<bool>();


    //hotkeys = Properties.Settings.Default.setKey_hotkeys.ToCharArray();
    udpWatchLimit = settings->value("gps/udpWatchMSec").value<int>();
    //check for 255
    //TODO
    //string[] words = Properties.Settings.Default.setDisplay_customColors.Split(',');

    isRTK = settings->value("gps/isRTK").value<bool>();
    isRTK_KillAutosteer = settings->value("gps/isRTKKillAutoSteer").value<bool>();

    pn.ageAlarm = settings->value("gps/ageAlarm").value<int>();

    isConstantContourOn = settings->value("as/isConstantContourOn").value<bool>();
    isSteerInReverse = settings->value("as/isSteerInReverse").value<bool>();

    guidanceLookAheadTime = settings->value("as/guidanceLookAheadTime").value<double>();

    //gyd pulls directly from settings
    //gyd.sideHillCompFactor = property_setAS_sideHillComp;

    fd.UpdateFieldBoundaryGUIAreas(bnd.bndList);

    isStanleyUsed = settings->value("vehicle/isStanleyUsed").value<bool>();
    isDay = settings->value("display/isDayMode").value<bool>();

    tool.loadSettings();
    if (tool.isSectionsNotZones){
        tool.sectionSetPositions();
        tool.sectionCalcWidths();
    } else {
        tool.sectionCalcMulti();
    }

    //disable youturn buttons
    headingFromSource = settings->value("gps/headingFromWhichSource").value<QString>();

    //load various saved settings or properties into the support classes
    ahrs.loadSettings();
    camera.loadSettings();
    pn.loadSettings();
    if(!isJobStarted)
        sim.loadSettings();
    vehicle.loadSettings();
    yt.loadSettings();
}
