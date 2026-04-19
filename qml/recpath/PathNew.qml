// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Menu when we create new Path
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Fusion
import QtQuick.Dialogs
import AOG

import ".."
import "../components"

Dialog {
    id: pathNew
    height: 300  * theme.scaleHeight
    width:700  * theme.scaleWidth
    anchors.centerIn: parent
    visible: false
    background: Rectangle {
        color: aogInterface.backgroundColor
        radius: 10
    }
    function show(){
        parent.visible = true
    }

    onVisibleChanged: {
        if (visible)
            newPath.text = ""
    }
    TopLine{
        id: topLine
        titleText: qsTr("New Path")
        onBtnCloseClicked: pathNew.visible = false
    }

    //color: "lightgray"
    Rectangle{
        id: textEntry
        width:parent.width*0.75
        height: 50  * theme.scaleHeight
        anchors.top:parent.top
        anchors.topMargin: 50
        anchors.horizontalCenter: parent.horizontalCenter
        color: aogInterface.backgroundColor
        border.color: "darkgray"
        border.width: 1
        Text {
            id: newPathLabel
            anchors.left: parent.left
            anchors.bottom: parent.top
            font.bold: true
            font.pixelSize: 15
            text: qsTr("Enter Path Name")
        }
        TextField{
            id: newPath
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: newPathLabel.bottom
            height: 49  * theme.scaleHeight
            selectByMouse: true
            placeholderText: qsTr("New Path Name")
        }
        Text {
            id: errorMessage
            anchors.top: newPath.bottom
            anchors.left: newPath.left
            color: "red"
            visible: false
            text: qsTr("Enter a valid name")
        }
    }
    Row{
        anchors.left: parent.left
        anchors.top: textEntry.bottom
        anchors.margins: 30
        spacing: 30
        IconButtonTransparent{
            objectName: "btnAddDate"
            id: marker
            icon.source: prefix + "/images/JobNameCalendar.png"
            Text{
                rightPadding: 10
                anchors.right: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: "+"
            }
            onClicked: {
                var date = new Date();
                var year = date.getFullYear();
                var month = String(date.getMonth() + 1).padStart(2, '0');
                var day = String(date.getDate()).padStart(2, '0');
                newPath.text += " " + `${year}-${month}-${day}`
            }

        }
        IconButtonTransparent{
            objectName: "btnAddTime"
            icon.source: prefix + "/images/JobNameTime.png"
            Text{
                rightPadding: 10
                anchors.right: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: "+"
            }
            onClicked: {
                var date = new Date();
                var hours = String(date.getHours()).padStart(2, '0');
                var minutes = String(date.getMinutes()).padStart(2, '0');
                newPath.text += " " + `${hours}-${minutes}`
            }
        }
    }

    Row{
        id: saveClose
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: children.width
        height: children.height
        spacing: 10
        IconButtonTransparent{
            onClicked: {
                pathNew.visible = false
                newPath.text = ""
            }
            icon.source: prefix + "/images/Cancel64.png"
        }
        IconButtonTransparent{
            enabled: newPath.text !== "" && errorMessage.visible === false;
            objectName: "btnSave"
            icon.source: prefix + "/images/OK64.png"

            onClicked: {
                pathNew.visible = false
                aog.pathNew(newPath.text.trim())
            }
        }
    }
}
