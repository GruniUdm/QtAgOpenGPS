// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Pinout for hyd lift/sections
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import Settings

import ".."
import "../components"

Rectangle{
    id: configModulesPins
    anchors.fill: parent
    color: aog.backgroundColor
    visible: false

    onVisibleChanged: {
        if (visible) load_settings()
    }


    function load_settings() {

        if (Settings.relay_pinConfig.length < 24) {
            for( var i=Settings.relay_pinConfig.length ; i < 24 ; i ++) {
                console.debug("adding to setRelay_pinConfig.")
                Settings.relay_pinConfig[i] = 0
            }
        }

        pin1.currentIndex = Settings.relay_pinConfig[0]
        pin2.currentIndex = Settings.relay_pinConfig[1]
        pin3.currentIndex = Settings.relay_pinConfig[2]
        pin4.currentIndex = Settings.relay_pinConfig[3]
        pin5.currentIndex = Settings.relay_pinConfig[4]
        pin6.currentIndex = Settings.relay_pinConfig[5]
        pin7.currentIndex = Settings.relay_pinConfig[6]
        pin8.currentIndex = Settings.relay_pinConfig[7]
        pin9.currentIndex = Settings.relay_pinConfig[8]
        pin10.currentIndex = Settings.relay_pinConfig[9]
        pin11.currentIndex = Settings.relay_pinConfig[10]
        pin12.currentIndex = Settings.relay_pinConfig[11]
        pin13.currentIndex = Settings.relay_pinConfig[12]
        pin14.currentIndex = Settings.relay_pinConfig[13]
        pin15.currentIndex = Settings.relay_pinConfig[14]
        pin16.currentIndex = Settings.relay_pinConfig[15]
        pin17.currentIndex = Settings.relay_pinConfig[16]
        pin18.currentIndex = Settings.relay_pinConfig[17]
        pin19.currentIndex = Settings.relay_pinConfig[18]
        pin20.currentIndex = Settings.relay_pinConfig[19]
        pin21.currentIndex = Settings.relay_pinConfig[20]
        pin22.currentIndex = Settings.relay_pinConfig[21]
        pin23.currentIndex = Settings.relay_pinConfig[22]
        pin24.currentIndex = Settings.relay_pinConfig[23]

        mandatory.visible = false

    }

    function save_settings() {

        Settings.relay_pinConfig[0] = pin1.currentIndex
        Settings.relay_pinConfig[1] = pin2.currentIndex
        Settings.relay_pinConfig[2] = pin3.currentIndex
        Settings.relay_pinConfig[3] = pin4.currentIndex
        Settings.relay_pinConfig[4] = pin5.currentIndex
        Settings.relay_pinConfig[5] = pin6.currentIndex
        Settings.relay_pinConfig[6] = pin7.currentIndex
        Settings.relay_pinConfig[7] = pin8.currentIndex
        Settings.relay_pinConfig[8] = pin9.currentIndex
        Settings.relay_pinConfig[9] = pin10.currentIndex
        Settings.relay_pinConfig[10] = pin11.currentIndex
        Settings.relay_pinConfig[11] = pin12.currentIndex
        Settings.relay_pinConfig[12] = pin13.currentIndex
        Settings.relay_pinConfig[13] = pin14.currentIndex
        Settings.relay_pinConfig[14] = pin15.currentIndex
        Settings.relay_pinConfig[15] = pin16.currentIndex
        Settings.relay_pinConfig[16] = pin17.currentIndex
        Settings.relay_pinConfig[17] = pin18.currentIndex
        Settings.relay_pinConfig[18] = pin19.currentIndex
        Settings.relay_pinConfig[19] = pin20.currentIndex
        Settings.relay_pinConfig[20] = pin21.currentIndex
        Settings.relay_pinConfig[21] = pin22.currentIndex
        Settings.relay_pinConfig[22] = pin23.currentIndex
        Settings.relay_pinConfig[23] = pin24.currentIndex

        mandatory.visible = false
    }

    GridLayout{
        flow: Grid.LeftToRight
        columns: 5
        rows: 5
        anchors.bottom: back.top
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
		anchors.topMargin: 10 * theme.scaleHeight
		anchors.bottomMargin: 10 * theme.scaleHeight
		anchors.leftMargin: 10 * theme.scaleWidth
		anchors.rightMargin: 10 * theme.scaleWidth
        PinButtons{
            id: pin1
            pinNumber: "Pin 1"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin2
            pinNumber: "Pin 2"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin3
            pinNumber: "Pin 3"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin4
            pinNumber: "Pin 4"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin5
            pinNumber: "Pin 5"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin6
            pinNumber: "Pin 6"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin7
            pinNumber: "Pin 7"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin8
            pinNumber: "Pin 8"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin9
            pinNumber: "Pin 9"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin10
            pinNumber: "Pin 10"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin11
            pinNumber: "Pin 11"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin12
            pinNumber: "Pin 12"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin13
            pinNumber: "Pin 13"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin14
            pinNumber: "Pin 14"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin15
            pinNumber: "Pin 15"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin16
            pinNumber: "Pin 16"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin17
            pinNumber: "Pin 17"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin18
            pinNumber: "Pin 18"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin19
            pinNumber: "Pin 19"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin20
            pinNumber: "Pin 20"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin21
            pinNumber: "Pin 21"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin22
            pinNumber: "Pin 22"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin23
            pinNumber: "Pin 23"
            onCurrentIndexChanged: mandatory.visible = true
        }
        PinButtons{
            id: pin24
            pinNumber: "Pin 24"
            onCurrentIndexChanged: mandatory.visible = true
        }
    }
    IconButtonTransparent{
        id: back
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 20
        icon.source: prefix + "/images/back-button.png"
        onClicked: {
            pin1.currentIndex = 0
            pin2.currentIndex = 0
            pin3.currentIndex = 0
            pin4.currentIndex = 0
            pin5.currentIndex = 0
            pin6.currentIndex = 0
            pin7.currentIndex = 0
            pin8.currentIndex = 0
            pin9.currentIndex = 0
            pin10.currentIndex = 0
            pin11.currentIndex = 0
            pin12.currentIndex = 0
            pin13.currentIndex = 0
            pin14.currentIndex = 0
            pin15.currentIndex = 0
            pin16.currentIndex = 0
            pin17.currentIndex = 0
            pin18.currentIndex = 0
            pin19.currentIndex = 0
            pin20.currentIndex = 0
            pin21.currentIndex = 0
            pin22.currentIndex = 0
            pin23.currentIndex = 0
            pin24.currentIndex = 0
        }
    }
    IconButtonTransparent{
        anchors.bottom: parent.bottom
        anchors.left: back.right
		anchors.topMargin: 20 * theme.scaleHeight
		anchors.bottomMargin: 20 * theme.scaleHeight
		anchors.rightMargin: 20 * theme.scaleHeight
		anchors.leftMargin: 20 * theme.scaleHeight
        icon.source: prefix + "/images/UpArrow64.png"
        onClicked: load_settings()
    }
    IconButtonTransparent{
        id: btnPinsSave
        anchors.right: mandatory.left
		anchors.topMargin: 20 * theme.scaleHeight
		anchors.bottomMargin: 20 * theme.scaleHeight
		anchors.rightMargin: 20 * theme.scaleHeight
		anchors.leftMargin: 20 * theme.scaleHeight
        anchors.bottom: parent.bottom
        icon.source: prefix + "/images/ToolAcceptChange.png"
        Text{
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.left
            anchors.rightMargin: 5
            text: qsTr("Send + Save")
        }
        onClicked: save_settings()
    }

    Image{
        id: mandatory
        anchors.right: parent.right
        anchors.verticalCenter: btnPinsSave.verticalCenter
        anchors.rightMargin: 20 * theme.scaleWidth
        visible: false
        source: prefix + "/images/Config/ConSt_Mandatory.png"
        height: back.width
    }
}
