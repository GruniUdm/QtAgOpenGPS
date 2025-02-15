// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Top left button menu
import QtQuick 2.0
import QtQuick.Controls.Fusion

import "components" as Comp

Drawer{
    id: hamburgerMenuRoot
    width: 270 * theme.scaleWidth
    height: mainWindow.height
    visible: false
    modal: true

    contentItem: Rectangle{
        id: hamburgerMenuContent
        anchors.fill: parent
        height: fieldMenu.height
        color: "black"

        Comp.ScrollViewExpandableColumn{
            id: hamburgerMenuColumn
            anchors.fill: parent

            Comp.IconButtonTextBeside{
                text: qsTr("Languages")
                onClicked: console.log("")
                visible: false//todo
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Directories")
                onClicked: console.log("")
                visible: false//todo
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Colors")
                onClicked: console.log("")
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Section Colors")
                onClicked: console.log("")
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Enter Sim Coords")
                onClicked: {
                    console.log("showing")
                    hamburgerMenuRoot.visible = false
                    setSimCoords.show()
                }
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Simulator On")
                property bool isChecked: settings.setMenu_isSimulatorOn
                onIsCheckedChanged: {
                    checked = isChecked
                }

                checkable: true
                checked: isChecked
                onCheckedChanged: {
                    settings.setMenu_isSimulatorOn = checked
                }
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Reset All")
                onClicked: console.log("")
                visible: false//todo
            }
            Comp.IconButtonTextBeside{
                text: qsTr("HotKeys")
                onClicked: console.log("")
                visible: false//todo
            }
            Comp.IconButtonTextBeside{
                text: qsTr("About...")
                onClicked: console.log("")
                visible: false//todo
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Help")
                onClicked: console.log("")
                visible: false//todo
            }
        }
    }
}
