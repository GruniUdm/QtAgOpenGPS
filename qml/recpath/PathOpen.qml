// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Menu when we want to open a path
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import AOG

import ".."
import "../components"

Dialog {
    id:pathOpen
    visible: false
    height: 500  * theme.scaleHeight
    width:700  * theme.scaleWidth
    anchors.centerIn: parent
    modal: false
    background: Rectangle {
        color: aogInterface.backgroundColor
        radius: 10
     }
    function show(){
        parent.visible = true
    }

    property int sortBy: 1

    TopLine{
        id: topLine
        titleText: qsTr("Open Path")
    }

    PathTable {
        id: pathTable
        anchors.top: topLine.bottom
        anchors.bottom: grid3.top
        width: parent.width - 10
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 5
        anchors.rightMargin: 15
        anchors.bottomMargin: 10

        adjustWidth: - scrollbar.width

        sortBy: pathOpen.sortBy

        anchors.right: topLine.right

        ScrollBar.vertical: ScrollBar {
            id: scrollbar
            anchors.left: pathOpen.right
            anchors.rightMargin: 10
            width: 10
            policy: ScrollBar.AlwaysOn
            active: true
            contentItem.opacity: 1
        }
    }

    Rectangle{
        id: grid3
        z: 4
        width: parent.width - 10
        height: deletePath.height + 10
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.bottom: parent.bottom
        color: aogInterface.backgroundColor
        Row {
            anchors.fill: parent
            spacing: 4
            //flow: Grid.TopToBottom
            //rows: 1
            IconButtonTransparent {
                id: deletePath
                objectName: "btnDeletePath"
                icon.source: prefix + "/images/skull.png"
                text: qsTr("Delete Path")
                //radius: 0
                //color3: "white"
                //border: 1
                //height: 75
                enabled: pathTable.currentIndex > -1
                onClicked: {
                    PathInterface.deletePath(pathTable.currentPathName) // Qt 6.8 MODERN: Direct Q_INVOKABLE call
                    //backend should update the list
                }
            }
        }
        Row {
            spacing: 5
            anchors.right: parent.right
            IconButtonTransparent {
                id: sort
                icon.source: prefix + "/images/Sort.png"
                //color3: "white"
                //height: 75
                text: qsTr("Toggle Sort")
                //radius: 0

                //border: 1
                onClicked: {
                    pathTable.sortBy = (pathTable.sortBy % 3) + 1
                }
            }

            IconButtonTransparent {
                id: cancel
                objectName: "btnCancel"
                icon.source: prefix + "/images/Cancel64.png"
                text: qsTr("Cancel")
                //color3: "white"
                //radius: 0
                //border: 1
                //height: 75
                onClicked: {
                    pathTable.clear_selection()
                    pathOpen.close()
                    //closeDialog()
                }
            }
            IconButtonTransparent {
                id: useSelected
                objectName: "btnUseSelected"
                icon.source: prefix + "/images/FileOpen.png"
                text: qsTr("Use Selected")
                //radius: 0
                //color3: "white"
                //border: 1
                //height: 75
                enabled: pathTable.currentIndex > -1
                onClicked: {
                    PathInterface.openPath(pathTable.currentPathName) // Qt 6.8 MODERN: Direct Q_INVOKABLE call
                    pathTable.clear_selection()
                    pathOpen.close()
                }
            }
        }
    }
}
