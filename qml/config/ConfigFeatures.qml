// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Headland, trams, etc
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import Settings

import ".."
import "../components"
import "steercomponents"

Item {
    anchors.fill: parent
    Rectangle{
        anchors.fill: parent
        color: aog.backgroundColor
        TitleFrame{
            id: fieldMenuText
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 30 * theme.scaleHeight
            anchors.rightMargin: 30 * theme.scaleWidth
            anchors.leftMargin: 30 * theme.scaleWidth
            anchors.bottomMargin: 30 * theme.scaleHeight
            title: "Field Menu"
            font.bold: true
            width: tramAll.width
            anchors.bottom: parent.bottom
            ColumnLayout{
                id: fieldMenuColumn
                anchors.fill: parent
                DisplayAndFeaturesBtns{
                    id: tramAll
                    icon.source: prefix + "/images/TramAll.png"
                    text: qsTr("Tram Lines")
                    isChecked: Settings.feature_isTramOn
                    onCheckedChanged: Settings.feature_isTramOn = checked
                }
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/HeadlandOn.png"
                    text: qsTr("Headland")
                    isChecked: Settings.feature_isHeadlandOn
                    onCheckedChanged: Settings.feature_isHeadlandOn = checked
                }
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/BoundaryOuter.png"
                    text: qsTr("Boundary")
                    isChecked: Settings.feature_isBoundaryOn
                    onCheckedChanged: Settings.feature_isBoundaryOn = checked
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/RecPath.png"
                    text: qsTr("Rec Path")
                    isChecked: Settings.feature_isRecPathOn
                    onCheckedChanged: Settings.feature_isRecPathOn = checked
                }
            }
        }
        TitleFrame{
            id: toolsMenuText
            width: tramAll.width
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: fieldMenuText.right
            anchors.topMargin: 30 * theme.scaleHeight
            anchors.rightMargin: 30 * theme.scaleWidth
            anchors.leftMargin: 30 * theme.scaleWidth
            anchors.bottomMargin: 30 * theme.scaleHeight
            title: qsTr("Tools Menu")
            font.bold: true
            ColumnLayout{
                id: toolsMenuColumn
                width: children.width
                height: parent.height
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/ABSmooth.png"
                    text: qsTr("AB Smooth")
                    isChecked: Settings.feature_isABSmoothOn
                    onCheckedChanged: Settings.feature_isABSmoothOn = checked
                }
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/HideContour.png"
                    text: qsTr("Hide Contour")
                    isChecked: Settings.feature_isHideContourOn
                    onCheckedChanged: Settings.feature_isHideContourOn = checked
                }
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/Webcam.png"
                    text: qsTr("WebCam")
                    isChecked: Settings.feature_isWebCamOn
                    onCheckedChanged: Settings.feature_isWebCamOn = checked
                }
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/YouTurnReverse.png"
                    text: qsTr("Offset Fix")
                    isChecked: Settings.feature_isOffsetFixOn
                    onCheckedChanged: Settings.feature_isOffsetFixOn = checked
                }
            }
        }

        TitleFrame{
            id: screenButtons
            anchors.top: parent.top
            anchors.left: toolsMenuText.right
            anchors.topMargin: 30 * theme.scaleHeight
            anchors.rightMargin: 30 * theme.scaleWidth
            anchors.leftMargin: 30 * theme.scaleWidth
            anchors.bottomMargin: 30 * theme.scaleHeight
            width: screenButtonsRow.width *2  + screenButtonsRow.spacing
            height: screenButtonsRow.height
            title: "Screen Buttons"
            RowLayout{
                id: screenButtonsRow
                spacing: 20 * theme.scaleWidth
                width: children.width
                height: children.height
                anchors.top: parent.top
                anchors.topMargin: 20
                anchors.left: parent.left
                DisplayAndFeaturesBtns{
                    id: uturn
                    icon.source: prefix + "/images/Images/z_TurnManual.png"
                    text: qsTr("U-Turn")
                    isChecked: Settings.feature_isYouTurnOn
                    onCheckedChanged: Settings.feature_isYouTurnOn = checked
                }
                DisplayAndFeaturesBtns{
                    id: lateral
                    icon.source: prefix + "/images/Images/z_LateralManual.png"
                    text: qsTr("Lateral")
                    isChecked: Settings.feature_isLateralOn
                    onCheckedChanged: Settings.feature_isLateralOn = checked
                }
            }
        }
        ColumnLayout{//Bottom, Right menu, Autostart AgIO
            anchors.top: screenButtons.bottom
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: screenButtons.horizontalCenter
            anchors.topMargin: 20 * theme.scaleHeight
            anchors.bottomMargin: 20 * theme.scaleHeight
            DisplayAndFeaturesBtns{
                text: qsTr("Auto Start AgIO")
                icon.source: prefix	+ "/images/AgIO.png"
                isChecked: Settings.feature_isAgIOOn
                onCheckedChanged: Settings.feature_isAgIOOn = checked
            }
        }

        TitleFrame{
            id: sound
            anchors.top: parent.top
            anchors.right: parent.right
            width: tramAll.width
            anchors.bottom: parent.bottom
            anchors.bottomMargin: parent.height *.3
            anchors.topMargin: 100 * theme.scaleHeight
            anchors.rightMargin: 30 * theme.scaleWidth
            anchors.leftMargin: 30 * theme.scaleWidth
            title: qsTr("Sound")
            ColumnLayout{
                width: children.width
                height: parent.height

                DisplayAndFeaturesBtns{
                    id: autoSteerSound
                    text: qsTr("Auto Steer")
                    icon.source: prefix + "/images/Config/ConF_SteerSound.png"
                    isChecked: Settings.sound_autoSteerSound
                    onCheckedChanged: Settings.sound_autoSteerSound = checked
                }
                DisplayAndFeaturesBtns{
                    id: youTurnSound
                    text: qsTr("You Turn")
                    icon.source: prefix + "/images/Config/ConF_SteerSound.png"
                    isChecked: Settings.sound_isUturnOn
                    onCheckedChanged: Settings.sound_isUturnOn = checked
                }

                DisplayAndFeaturesBtns{
                    id: hydLiftSound
                    text: qsTr("Hyd Lift")
                    icon.source: prefix + "/images/Config/ConF_SteerSound.png"
                    isChecked: Settings.sound_isHydLiftOn
                    onCheckedChanged: Settings.sound_isHydLiftOn = checked
                }
                /*DisplayAndFeaturesBtns{
                id: boundaryApproachSound
                    Layout.alignment: Qt.AlignCenter
                visible: false // not implemented
                text: qsTr("Boundary Approach")
                icon.source: prefix + "/images/Config/ConF_SteerSound.png"
                isChecked: .setSound_isAutoSteerOn
                onCheckedChanged: Settings.sound_autoSteerSound = checked
            }*/
            }
        }
    }
}
