// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Menu when we create new field
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Controls.Material

import ".."
import "../components"

Dialog {
    id: fieldNew
    height: 300  * theme.scaleHeight
    width:700  * theme.scaleWidth
    anchors.centerIn: parent
    visible: false
    function show(){
        parent.visible = true
    }

    onVisibleChanged: {
        if (visible)
            newField.text = ""
    }
    TopLine{
        id: topLine
        titleText: qsTr("New Field")
    }

    //color: "lightgray"
    Rectangle{
        id: textEntry
        width:parent.width*0.75
        height: 50  * theme.scaleHeight
        anchors.top:parent.top
        anchors.topMargin: 50
        anchors.horizontalCenter: parent.horizontalCenter
        color: aog.backgroundColor
        border.color: "darkgray"
        border.width: 1
        Text {
            id: newFieldLabel
            anchors.left: parent.left
            anchors.bottom: parent.top
            font.bold: true
            font.pixelSize: 15
            text: qsTr("Enter Field Name")
        }
        TextField{
            id: newField
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: newFieldLabel.bottom
            selectByMouse: true
            placeholderText: qsTr("New Field Name")
            onTextChanged: {
                for (var i=0; i < fieldInterface.field_list.length ; i++) {
                    if (text === fieldInterface.field_list[i].name) {
                        errorMessage.visible = true
                        break
                    } else
                        errorMessage.visible = false
                }
            }
        }
        Text {
            id: errorMessage
            anchors.top: newField.bottom
            anchors.left: newField.left
            color: "red"
            visible: false
            text: qsTr("This field exists already; please choose another name.")
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
                anchors.right: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: "+"
            }
            onClicked: {
                var date = new Date();
                var year = date.getFullYear();
                var month = String(date.getMonth() + 1).padStart(2, '0');
                var day = String(date.getDate()).padStart(2, '0');
                newField.text += " " + `${year}-${month}-${day}`
            }

        }
        IconButtonTransparent{
            objectName: "btnAddTime"
            icon.source: prefix + "/images/JobNameTime.png"
            Text{
                anchors.right: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: "+"
            }
            onClicked: {
                var date = new Date();
                var hours = String(date.getHours()).padStart(2, '0');
                var minutes = String(date.getMinutes()).padStart(2, '0');
                newField.text += " " + `${hours}-${minutes}`
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
                fieldNew.visible = false
                newField.text = ""
            }
            icon.source: prefix + "/images/Cancel64.png"
        }
        IconButtonTransparent{
            enabled: newField.text !== "" && errorMessage.visible === false;
            objectName: "btnSave"
            icon.source: prefix + "/images/OK64.png"

            onClicked: {
                fieldNew.visible = false
                fieldInterface.field_new(newField.text.trim())
            }
        }
    }
}
