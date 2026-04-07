// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// AB Smooth Curve dialog
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import AOG

import ".."
import "../components" as Comp

Comp.MoveablePopup {
    id: smoothABDialog
    x: 40
    y: 40
    height: 350  * theme.scaleHeight
    width: 250  * theme.scaleWidth

    property int smoothCount: 20

    function show() {
        smoothABDialog.visible = true
        smoothCount = 20
        smoothLabel.text = "**"
        TracksInterface.smoothAB(smoothCount * 2)
    }

    Comp.TopLine{
        id: topLine
        titleText: qsTr("Smooth AB Curve")
        onBtnCloseClicked: smoothABDialog.close()
    }



        ColumnLayout{
            anchors.top: topLine.bottom
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: 10

            RowLayout{
                Layout.alignment: Qt.AlignCenter
                Layout.bottomMargin: 10
                spacing: 20

                Comp.IconButtonTransparent{
                    icon.source: prefix + "/images/UpArrow64.png"
                    onClicked: {
                        smoothCount++
                        if (smoothCount > 100) smoothCount = 100
                        smoothLabel.text = smoothCount
                        TracksInterface.smoothAB(smoothCount * 2)
                    }
                }

                Comp.IconButtonTransparent{
                    icon.source: prefix + "/images/DnArrow64.png"
                    onClicked: {
                        smoothCount--
                        if (smoothCount < 2) smoothCount = 2
                        smoothLabel.text = smoothCount
                        TracksInterface.smoothAB(smoothCount * 2)
                    }
                }
            }

            Item { Layout.fillHeight: true }

            Comp.TextLine{
                id: smoothLabel
                font.pixelSize: 30
                text: "**"
                Layout.alignment: Qt.AlignCenter
                Layout.bottomMargin: 20
            }

            Item { Layout.fillHeight: true }

            RowLayout{
                Layout.alignment: Qt.AlignCenter
                Layout.bottomMargin: 10

                Comp.IconButtonTransparent{
                    icon.source: prefix + "/images/FileDontSave.png"
                    onClicked: {
                        smoothABDialog.visible = false
                        TracksInterface.smoothABSmoothSave()
                    }
                }

                Comp.IconButtonTransparent{
                    icon.source: prefix + "/images/FileSave.png"
                    onClicked: {
                        smoothABDialog.visible = false
                        TracksInterface.smoothABSmoothSave()
                        // TODO: Also save tracks to file
                    }
                }

                Comp.IconButtonTransparent{
                    icon.source: prefix + "/images/Cancel64.png"
                    onClicked: {
                        smoothABDialog.visible = false
                        TracksInterface.smoothABSmoothCancel()
                    }
                }
            }
        }


    function hide() {
        smoothABDialog.visible = false
        TracksInterface.smoothABSmoothCancel()
    }
}
