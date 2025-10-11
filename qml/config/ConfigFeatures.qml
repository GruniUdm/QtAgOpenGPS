// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Headland, trams, etc
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
//import Settings

import ".."
import "../components"
import "steercomponents"

Item {
    anchors.fill: parent
    objectName: "configFeatures"

    // Qt 6.8 QProperty + BINDABLE: Simple properties to allow setProperty() updates from C++
    property bool featureIsTramOn: true
    property bool featureIsHeadlandOn: true
    property bool featureIsBoundaryOn: true
    property bool featureIsRecPathOn: true
    property bool featureIsABSmoothOn: true
    property bool featureIsHideContourOn: false
    property bool featureIsWebCamOn: false
    property bool featureIsOffsetFixOn: false
    property bool featureIsYouTurnOn: true
    property bool featureIsLateralOn: true
    property bool featureIsNudgeOn: true
    property bool featureIsAgIOOn: true
    property bool soundAutoSteerSound: true
    property bool soundIsUturnOn: true
    property bool soundIsHydLiftOn: true
    Rectangle{
        anchors.fill: parent
        color: aogInterface.backgroundColor
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
                    isChecked: featureIsTramOn
                    onCheckedChanged: featureIsTramOn = checked
                }
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/HeadlandOn.png"
                    text: qsTr("Headland")
                    isChecked: featureIsHeadlandOn
                    onCheckedChanged: featureIsHeadlandOn = checked
                }
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/BoundaryOuter.png"
                    text: qsTr("Boundary")
                    isChecked: featureIsBoundaryOn
                    onCheckedChanged: featureIsBoundaryOn = checked
                }
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/RecPath.png"
                    text: qsTr("Rec Path")
                    isChecked: featureIsRecPathOn
                    onCheckedChanged: featureIsRecPathOn = checked
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
                    isChecked: featureIsABSmoothOn
                    onCheckedChanged: featureIsABSmoothOn = checked
                }
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/HideContour.png"
                    text: qsTr("Hide Contour")
                    isChecked: featureIsHideContourOn
                    onCheckedChanged: featureIsHideContourOn = checked
                }
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/Webcam.png"
                    text: qsTr("WebCam")
                    isChecked: featureIsWebCamOn
                    onCheckedChanged: featureIsWebCamOn = checked
                }
                DisplayAndFeaturesBtns{
                    icon.source: prefix + "/images/YouTurnReverse.png"
                    text: qsTr("Offset Fix")
                    isChecked: featureIsOffsetFixOn
                    onCheckedChanged: featureIsOffsetFixOn = checked
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
                    isChecked: featureIsYouTurnOn
                    onCheckedChanged: featureIsYouTurnOn = checked
                }
                DisplayAndFeaturesBtns{
                    id: lateral
                    icon.source: prefix + "/images/Images/z_LateralManual.png"
                    text: qsTr("Lateral")
                    isChecked: featureIsLateralOn
                    onCheckedChanged: featureIsLateralOn = checked
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
                text: qsTr("Nudge Controls")
                icon.source: prefix	+ "/images/SnapToPivot.png"
                isChecked: featureIsNudgeOn
                onCheckedChanged: featureIsNudgeOn = checked
            }
            DisplayAndFeaturesBtns{
                text: qsTr("Auto Start AgIO")
                icon.source: prefix	+ "/images/AgIO.png"
                isChecked: featureIsAgIOOn
                onCheckedChanged: featureIsAgIOOn = checked
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
                    isChecked: soundAutoSteerSound
                    onCheckedChanged: soundAutoSteerSound = checked
                }
                DisplayAndFeaturesBtns{
                    id: youTurnSound
                    text: qsTr("You Turn")
                    icon.source: prefix + "/images/Config/ConF_SteerSound.png"
                    isChecked: soundIsUturnOn
                    onCheckedChanged: soundIsUturnOn = checked
                }

                DisplayAndFeaturesBtns{
                    id: hydLiftSound
                    text: qsTr("Hyd Lift")
                    icon.source: prefix + "/images/Config/ConF_SteerSound.png"
                    isChecked: soundIsHydLiftOn
                    onCheckedChanged: soundIsHydLiftOn = checked
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
