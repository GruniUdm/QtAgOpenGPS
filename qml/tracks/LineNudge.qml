// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
// 
// Little popup window where we can nudge/snap to pivot
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
//import Settings
import AOG
import Interface

import ".."
import "../components" as Comp

Comp.MoveablePopup{
    id: lineNudge
    x: 40
    y: 40
    height: 400
    width: 200
    function show(){
        lineNudge.visible = true
    }

    Rectangle{
        id: rootRect
        color: "#b3ccff"
        anchors.fill: parent
        Text{
            id: nudgeDist
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.leftMargin: 8
            text: "< 3"
        }
        Comp.IconButtonTransparent{
            id: closeBtn
            icon.source: prefix + "/images/WindowClose.png"
            implicitHeight: 40
            implicitWidth: 40
            anchors.top: parent.top
            anchors.right: parent.right
            onClicked: lineNudge.visible = false
        }
        Rectangle{
            id: whiteRect
            anchors.top: closeBtn.bottom
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.right: parent.right
            anchors.rightMargin: 5
            color: "white"
            ColumnLayout{
                id: column
                anchors.fill: parent
                RowLayout{
                    Layout.alignment: Qt.AlignCenter
                    implicitWidth: parent.width
                    Comp.IconButtonTransparent{
                        icon.source: prefix + "/images/SnapLeftHalf.png"
                        onClicked: TracksInterface.nudge((Settings.vehicle_toolWidth - Settings.vehicle_toolOverlap)/-2)
                        Layout.alignment: Qt.AlignLeft
                    }
                    Comp.IconButtonTransparent{
                        icon.source: prefix + "/images/SnapRightHalf.png"
                        onClicked: TracksInterface.nudge((Settings.vehicle_toolWidth - Settings.vehicle_toolOverlap)/2)
                        Layout.alignment: Qt.AlignRight
                    }
                }
                RowLayout{
                    Layout.alignment: Qt.AlignCenter
                    implicitWidth: parent.width
                    Comp.IconButtonTransparent{
                        icon.source: prefix + "/images/SnapLeft.png"
                        Layout.alignment: Qt.AlignLeft
                        onClicked: TracksInterface.nudge(Settings.as_snapDistance/-100) //spinbox returns cm, convert to metres
                    }
                    Comp.IconButtonTransparent{
                        icon.source: prefix + "/images/SnapRight.png"
                        Layout.alignment: Qt.AlignRight
                        onClicked: TracksInterface.nudge(Settings.as_snapDistance/100) //spinbox returns cm, convert to metres
                    }
                }
                Comp.SpinBoxCM{
                    id: offset
                    Layout.alignment: Qt.AlignCenter

                    from: 1
                    to: 1000
                    boundValue: Settings.as_snapDistance
                    onValueModified: Settings.as_snapDistance = value
                }

                RowLayout{
                    Layout.alignment: Qt.AlignCenter
                    implicitWidth: parent.width
                    Comp.IconButtonTransparent{
                        icon.source: prefix + "/images/SnapToPivot.png"
                        Layout.alignment: Qt.AlignLeft
                        onClicked: TracksInterface.nudge_center()
                    }
                    Comp.IconButtonTransparent{
                        icon.source: prefix + "/images/SteerZero.png"
                        Layout.alignment: Qt.AlignRight
                        onClicked: TracksInterface.nudge_zero()
                    }
                }
            }
        }
    }
}
