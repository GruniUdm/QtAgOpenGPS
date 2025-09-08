// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Display settings. Field texture, immerial/metric, etc
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
//import Settings

import ".."
import "steercomponents"
import "../components"
Item {
    anchors.fill: parent
    Rectangle{
        id: configDisplay
        anchors.fill: parent
        color: aog.backgroundColor
        GridLayout{
            id:buttonsMain
            anchors.top: parent.top
            anchors.topMargin: 10 * theme.scaleHeight
            anchors.bottom: units.top
            anchors.left: parent.left
            anchors.right: parent.right
            columns:3
			rows: 5
            rowSpacing: 10
            flow: Grid.TopToBottom
            DisplayAndFeaturesBtns{
                id: fieldTexture
                text: qsTr("Field Texture")
                icon.source: prefix + "/images/Config/ConD_FloorTexture.png"
                isChecked: Settings.display_isTextureOn
                onCheckedChanged: Settings.display_isTextureOn = checked
            }
            DisplayAndFeaturesBtns{
                id: autoDayNight
                isChecked: Settings.display_autoDayNight
                onCheckedChanged: Settings.display_autoDayNight = !checked
                text: qsTr("Auto Day Night")
                icon.source: prefix + "/images/Config/ConD_AutoDayNight.png"
            }
            DisplayAndFeaturesBtns{
                id:startFullScreen
                text: qsTr("Start FullScreen")
                icon.source: prefix + "/images/Config/ConD_FullScreenBegin.png"
                isChecked: Settings.display_isStartFullscreen
                onCheckedChanged: Settings.display_isStartFullscreen = checked
            }
            DisplayAndFeaturesBtns{
                id:grid
                text: qsTr("Grid")
                icon.source: prefix + "/images/Config/ConD_Grid.png"
                isChecked: Settings.menu_isGridOn
                onCheckedChanged: Settings.menu_isGridOn = checked
            }
            DisplayAndFeaturesBtns{
                id:sky
                text:qsTr("Sky")
                icon.source: prefix + "/images/Config/ConD_Sky.png"
                isChecked: Settings.menu_isSkyOn
                onCheckedChanged: Settings.menu_isSkyOn = checked
            }
            DisplayAndFeaturesBtns{
                id:brightness
                text:qsTr("Brightness")
                icon.source: prefix + "/images/BrightnessUp.png"
                isChecked: Settings.display_isBrightnessOn
                onCheckedChanged: Settings.display_isBrightnessOn = checked
            }
            DisplayAndFeaturesBtns{
                id:lightBar
                text:qsTr("Lightbar")
                icon.source: prefix + "/images/Config/ConD_LightBar.png"
                isChecked: Settings.menu_isLightBarOn
                onCheckedChanged: Settings.menu_isLightBarOn = checked
            }
            DisplayAndFeaturesBtns{
                id:logNMEA
                text: qsTr("Log NMEA")
                icon.source: prefix + "/images/Config/ConD_LogNMEA.png"
            }
            DisplayAndFeaturesBtns{
                id: guideLines
                text: qsTr("GuideLines")
                icon.source: prefix + "/images/Config/ConD_ExtraGuides.png"
                isChecked: Settings.menu_isSideGuideLines
                onCheckedChanged: Settings.menu_isSideGuideLines = checked
            }
            DisplayAndFeaturesBtns{
                id:svennArrow
                text: qsTr("Svenn Arrow")
                icon.source: prefix + "/images/SvennArrow.png"
                isChecked: Settings.display_isSvennArrowOn
                onCheckedChanged: Settings.display_isSvennArrowOn = checked
            }
            DisplayAndFeaturesBtns{
                id: compass
                text: qsTr("Compass")
                icon.source: prefix + "/images/Images/z_Compass.png"
                isChecked: Settings.menu_isCompassOn
                onCheckedChanged: Settings.menu_isCompassOn = checked
            }
            /*DisplayAndFeaturesBtns{ //appears there is no setting for this
                id: speedo
                text: qsTr("Speedometer")
                icon.source: prefix + "/images/Images/z_Speedo.png"
                isChecked: Settings.menu_isSpeedoOn
                onCheckedChanged: Settings.menu_isSpeedoOn
            }*/
        }
		TextLine{
			id: unitsTitle
			text: qsTr("Units")
			anchors.bottom: units.top
			anchors.horizontalCenter: units.horizontalCenter
		}
        Row{
            id:units
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: childrenRect.width
            height: childrenRect.height
            ButtonGroup {
                id: metricimp
                buttons: [metric, imperial]
            }

            IconButtonColor{
                id:metric
                icon.source: prefix + "/images/Config/ConD_Metric.png"
                //text: qsTr("Metric")
                property bool settingsChecked: Settings.menu_isMetric
                checkable: true
                isChecked: Settings.menu_isMetric
                onCheckedChanged: Settings.menu_isMetric = checked
            }
            IconButtonColor{
                id:imperial
                icon.source: prefix + "/images/Config/ConD_Imperial.png"
                text: ""
                checkable: true
                isChecked: !Settings.menu_isMetric
                onCheckedChanged: Settings.menu_isMetric = !checked
            }
        }
    }
}
