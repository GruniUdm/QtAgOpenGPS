// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Top left button menu
import QtQuick
import QtQuick.Controls.Fusion
import Settings

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
                onClicked: languagesMenu.visible = !languagesMenu.visible
                visible: true
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Directories")
                onClicked: console.log("")
                visible: false//todo
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Set Colors")
                onClicked: {
                    console.log("showing")
                    hamburgerMenuRoot.visible = false
                    setColors.show()
                }
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
                property bool isChecked: Settings.menu_isSimulatorOn
                onIsCheckedChanged: {
                    checked = isChecked
                }
                checkable: true
                checked: isChecked
                onCheckedChanged: {
                    Settings.menu_isSimulatorOn = checked
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
    Drawer {
        id: languagesMenu
        width: 270 * theme.scaleWidth
        height: mainWindow.height
        modal: true

        contentItem: Rectangle{
            id: languagesMenuContent
            anchors.fill: parent
            height: languagesMenu.height
            color: aog.blackDayWhiteNight
        }

        Grid {
            id: grid2
            height: childrenRect.height
            width: childrenRect.width
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: 5
            spacing: 10
            flow: Grid.TopToBottom
            rows: 5
            columns: 1

            Comp.IconButtonTextBeside{
                text: qsTr("English")
                onClicked: {
                    hamburgerMenuRoot.visible = false
                    Settings.menu_language = "en"
                    aog.settings_save()}
            }
            Comp.IconButtonTextBeside{
                text: qsTr("French")
                onClicked: {
                    hamburgerMenuRoot.visible = false
                    Settings.menu_language = "fr"
                    aog.settings_save()}
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Russian")
                onClicked: {
                    hamburgerMenuRoot.visible = false
                    Settings.menu_language = "ru"
                    aog.settings_save()}
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Srpski")
                onClicked: {
                    hamburgerMenuRoot.visible = false
                    Settings.menu_language = "sr"
                    aog.settings_save()}
            }
            Comp.IconButtonTextBeside{
                text: qsTr("Deutch")
                onClicked: {
                    hamburgerMenuRoot.visible = false
                    Settings.menu_language = "de"
                    aog.settings_save()}
            }
        }
    }
}
