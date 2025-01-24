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
                text: "Languages"
                onClicked: console.log("")
                visible: false//todo
            }
            Comp.IconButtonTextBeside{
                text: "Directories"
                onClicked: console.log("")
                visible: false//todo
            }
            Comp.IconButtonTextBeside{
                text: "Colors"
                onClicked: console.log("")
            }
            Comp.IconButtonTextBeside{
                text: "Section Colors"
                onClicked: console.log("")
            }
            Comp.IconButtonTextBeside{
                text: "Enter Sim Coords"
                onClicked: {
                    console.log("showing")
                    hamburgerMenuRoot.visible = false
                    setSimCoords.show()
                }
            }
            Comp.IconButtonTextBeside{
                text: "Simulator On"
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
                text: "Reset All"
                onClicked: console.log("")
                visible: false//todo
            }
            Comp.IconButtonTextBeside{
                text: "HotKeys"
                onClicked: console.log("")
                visible: false//todo
            }
            Comp.IconButtonTextBeside{
                text: "About..."
                onClicked: console.log("")
                visible: false//todo
            }
            Comp.IconButtonTextBeside{
                text: "Help"
                onClicked: console.log("")
                visible: false//todo
            }
        }
    }
}
