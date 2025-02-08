// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Pinout for hyd lift/sections
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts

import ".."
import "../components"

Rectangle{
    id: configRateControl
    anchors.fill: parent
    color: aog.backgroundColor
    visible: false

    onVisibleChanged: {
        if (visible) load_settings()
    }


    function load_settings() {

        rateMinPWM.value = settings.setRate_valveMinPWM
        rateMaxPWM.value = settings.setRate_valveMaxPWM
        moduleID.value = settings.setRate_moduleID
        rateKP.value = settings.setRate_pidKP
        rateKI.value = settings.setRate_pidKI
        rateKD.value = settings.setRate_pidKD
        ratePIDscale.value = settings.setRate_ratePIDscale
        cboxIsRateControlOn.checked = settings.setRate_RateControlOn
        rateSensor.value = settings.setRate_rateSensorCount
        setRate.value = settings.setRate_rateSET

        mandatory.visible = false

    }

    function save_settings() {


        settings.setRate_valveMinPWM = rateMinPWM.value
        settings.setRate_valveMaxPWM = rateMaxPWM.value
        settings.setRate_moduleID = moduleID.value
        settings.setRate_pidKP = rateKP.value
        settings.setRate_pidKI = rateKI.value
        settings.setRate_pidKD = rateKD.value
        settings.setRate_ratePIDscale = ratePIDscale.value
        settings.setRate_RateControlOn = cboxIsRateControlOn.checked
        settings.setRate_rateSensorCount = rateSensor.value
        settings.setRate_rateSET = setRate.value

        mandatory.visible = false

        //aog.doBlockageMonitoring()
    }
    Label{
        id: top
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 10
        text: qsTr("Rate Control")
    }

    GridLayout{
        flow: Grid.LeftToRight
        columns: 5
        rows: 3
        anchors.bottom: back.top
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 50 * theme.scaleWidth
        anchors.rightMargin: 20 * theme.scaleWidth


        SpinBoxCustomized{
            id: moduleID
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn.checked
            boundValue: settings.setRate_moduleID
            onValueModified: {
                settings.setRate_moduleID = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Module ID: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCustomized{
            id: rateKP
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn.checked
            boundValue: settings.setRate_pidKP
            onValueModified: {
                settings.setRate_pidKP = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("PID KP: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCustomized{
            id: rateKI
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn.checked
            boundValue: settings.setRate_pidKI
            onValueModified: {
                settings.setRate_pidKI = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("PID KI: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCustomized{
            id: rateKD
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn.checked
            boundValue: settings.setRate_pidKD
            onValueModified:{
                settings.setRate_pidKD = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("PID KD: ")
                font.bold: true
                anchors.top: parent.bottom
            }

        }
        SpinBoxCustomized{
            id: rateMinPWM
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn.checked
            boundValue: settings.setRate_valveMinPWM
            onValueModified: {
                settings.setRate_valveMinPWM = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Minimum PWM")
                font.bold: true
                anchors.top: parent.bottom
            }

        }
        SpinBoxCustomized{
            Layout.alignment: Qt.AlignRight
            id: rateMaxPWM
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn.checked
            boundValue: settings.setRate_valveMaxPWM
            onValueModified:{
                settings.setRate_valveMaxPWM = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Maximum PWM")
                font.bold: true
                anchors.top: parent.bottom
            }

        }
        SpinBoxCustomized{
            Layout.alignment: Qt.AlignRight
            id: ratePIDscale
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn.checked
            boundValue: settings.setRate_ratePIDscale
            onValueModified:{
                settings.setRate_ratePIDscale = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("PID scale")
                font.bold: true
                anchors.top: parent.bottom
            }

        }
        SpinBoxCustomized{
            Layout.alignment: Qt.AlignRight
            id: setRate
            from: 0
            to:1000
            editable: true
            enabled: cboxIsRateControlOn.checked
            boundValue: settings.setRate_rateSET
            onValueModified:{
                settings.setRate_rateSET = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Rate SET")
                font.bold: true
                anchors.top: parent.bottom
            }

        }
        SpinBoxCustomized{
            Layout.alignment: Qt.AlignRight
            id: rateSensor
            from: 0
            to:1000
            editable: true
            enabled: cboxIsRateControlOn.checked
            boundValue: settings.setRate_rateSensorCount
            onValueModified:{
                settings.setRate_rateSensorCount = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Sensor Count")
                font.bold: true
                anchors.top: parent.bottom
            }

        }


    }


    IconButtonTransparent{
        id: back
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        enabled: cboxIsRateControlOn.checked
        icon.source: prefix + "/images/back-button.png"
        onClicked: {
            rateMinPWM.boundValue = 100
            rateMaxPWM.boundValue = 255
            rateKP.boundValue = 10
            rateKI.boundValue = 0
            rateKD.boundValue = 0
            ratePIDscale.boundValue = 0
            rateSensor.boundValue = 600
            setRate.boundValue = 100
        }
    }
    IconButtonTransparent{
        id: loadSetBlockage
        anchors.bottom: parent.bottom
        anchors.left: back.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        enabled: cboxIsRateControlOn.checked
        icon.source: prefix + "/images/UpArrow64.png"
        onClicked: {
            load_settings()
            mandatory.visible = true
        }
    }
    IconButtonColor{
        id: cboxIsRateControlOn
        height: loadSetBlockage.height
        anchors.bottom: parent.bottom
        anchors.left: loadSetBlockage.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        icon.source: prefix + "/images/SwitchOff.png"
        iconChecked: prefix + "/images/SwitchOn.png"
        checkable: true
        onClicked: mandatory.visible = true
        }
    IconButtonTransparent{
        id: ratePWMauto
        height: loadSetBlockage.height
        anchors.bottom: parent.bottom
        anchors.left: cboxIsRateControlOn.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        icon.source: prefix + "/images/AutoStop.png"
        onClicked: aog.rate_pwm_auto()
}
    IconButtonTransparent{
        id: ratePWMUP
        height: loadSetBlockage.height
        anchors.bottom: parent.bottom
        anchors.left: ratePWMauto.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        icon.source: prefix + "/images/UpArrow64.png"
        onClicked: aog.rate_bump_pwm(true)
        Label{
            anchors.bottom: parent.top
            anchors.bottomMargin: 20 * theme.scaleHeight
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Manual PWM")
        }
}
    IconButtonTransparent{
        id: ratePWMDN
        height: loadSetBlockage.height
        anchors.bottom: parent.bottom
        anchors.left: ratePWMUP.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        icon.source: prefix + "/images/DnArrow64.png"
        onClicked: aog.rate_bump_pwm(false)
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
