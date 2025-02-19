// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Set line name
import QtQuick 2.0
import QtQuick.Controls.Fusion
import AOG
import "components" as Comp

Rectangle{
    id: line_Name
    width: 300
    height: 400

    property string title: ""

    signal accepted()
    signal rejected()
    property string abLineName //TODO: use this as input for editing the name

    function generate_ab_name(heading_degrees) {
        var name

        name = qsTr("AB ") + (heading_degrees).toLocaleString(Qt.locale(), 'f', 1) + "\u00B0 " + Utils.findDirection(heading_degrees)

        textInputBox.text = name
    }

    function set_name(name) {
        textInputBox.text = name
    }

    color: aog.backgroundColor
    border.width: 1
    border.color: aog.blackDayWhiteNight

    Comp.TopLine{
        id: topLine
        titleText: line_Name.title
    }

    TextField {
        id: textInputBox
        width: parent.width -20
        anchors.left: parent.left
        anchors.top: topLine.bottom
        anchors.margins: 10
        height: 80
        //color:"lightgray"
        objectName: "textInputBox"
        text: ""
        selectByMouse: true
    }

    Comp.IconButtonTransparent{
        objectName: "btnAddTime"
        anchors.top: textInputBox.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        icon.source: prefix + "/images/Time.png"
        onClicked: {
            var time = new Date().toLocaleTimeString(Qt.locale())
            textInputBox.text += " " + time
        }
        Text{
            anchors.right: parent.left
            anchors.rightMargin: 30
            anchors.verticalCenter: parent.verticalCenter
            text: "+"
            font.bold: true
            font.pixelSize: 35
        }
    }
    Comp.IconButtonTransparent{
        objectName: "btnCancel"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 20
        icon.source: prefix + "/images/Cancel64.png"
        onClicked:{
            parent.visible = false
            rejected()
        }
    }
    Comp.IconButtonTransparent{
        objectName: "btnOk"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 20
        icon.source: prefix + "/images/OK64.png"
        onClicked:{
            line_Name.visible = false
            abLineName = textInputBox.text
            line_Name.accepted()
        }
    }
}
