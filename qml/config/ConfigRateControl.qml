// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Pinout for hyd lift/sections
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import Settings
import AOG
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

        moduleID.value = Settings.rate_Product0[0]
        prodDensityBox.value = Settings.rate_Product0[1]
        cboxIsRateControlOn.checked = (Settings.rate_Product0[2]>0)?true:false
        rateKP.value = Settings.rate_Product0[3]
        rateKI.value = Settings.rate_Product0[4]
        rateKD.value = Settings.rate_Product0[5]
        rateMinPWM.value = Settings.rate_Product0[6]
        rateMaxPWM.value = Settings.rate_Product0[7]
        ratePIDscale.value = Settings.rate_Product0[8]
        rateSensor.value = Settings.rate_Product0[9]
        setRate.value = Settings.rate_Product0[10]
        cboxRateMode.currentIndex = Settings.rate_Product0[11]
        cboxRateControlType.currentIndex = Settings.rate_Product0[12];
        cboxRateCoverageUnits.currentIndex = Settings.rate_Product0[13];
        productName0.text = Settings.rate_ProductName[0];

        moduleID1.value = Settings.rate_Product1[0]
        prodDensityBox1.value = Settings.rate_Product1[1]
        cboxIsRateControlOn1.checked = (Settings.rate_Product1[2]>0)?true:false
        rateKP1.value = Settings.rate_Product1[3]
        rateKI1.value = Settings.rate_Product1[4]
        rateKD1.value = Settings.rate_Product1[5]
        rateMinPWM1.value = Settings.rate_Product1[6]
        rateMaxPWM1.value = Settings.rate_Product1[7]
        ratePIDscale1.value = Settings.rate_Product1[8]
        rateSensor1.value = Settings.rate_Product1[9]
        setRate1.value = Settings.rate_Product1[10]
        cboxRateMode1.currentIndex = Settings.rate_Product1[11]
        cboxRateControlType1.currentIndex = Settings.rate_Product1[12];
        cboxRateCoverageUnits1.currentIndex = Settings.rate_Product1[13];
        productName1.text = Settings.rate_ProductName[1];

        mandatory.visible = false
        mandatory1.visible = false

    }

    function save_settings() {

        Settings.rate_Product0[0] = moduleID.value;
        Settings.rate_Product0[1] = prodDensityBox.value
        Settings.rate_Product0[2] = cboxIsRateControlOn.checked?1:0
        Settings.rate_Product0[3] = rateKP.value
        Settings.rate_Product0[4] = rateKI.value
        Settings.rate_Product0[5] = rateKD.value
        Settings.rate_Product0[6] = rateMinPWM.value
        Settings.rate_Product0[7] = rateMaxPWM.value
        Settings.rate_Product0[8] = ratePIDscale.value
        Settings.rate_Product0[9] = rateSensor.value
        Settings.rate_Product0[10] = setRate.value
        Settings.rate_Product0[11] = Number(cboxRateMode.currentIndex)
        Settings.rate_Product0[12] = Number(cboxRateControlType.currentIndex)
        Settings.rate_Product0[13] = Number(cboxRateCoverageUnits.currentIndex)
        Settings.rate_ProductName[0] = productName0.text;

        Settings.rate_Product1[0] = moduleID1.value;
        Settings.rate_Product1[1] = prodDensityBox1.value
        Settings.rate_Product1[2] = cboxIsRateControlOn1.checked?1:0
        Settings.rate_Product1[3] = rateKP1.value
        Settings.rate_Product1[4] = rateKI1.value
        Settings.rate_Product1[5] = rateKD1.value
        Settings.rate_Product1[6] = rateMinPWM1.value
        Settings.rate_Product1[7] = rateMaxPWM1.value
        Settings.rate_Product1[8] = ratePIDscale1.value
        Settings.rate_Product1[9] = rateSensor1.value
        Settings.rate_Product1[10] = setRate1.value
        Settings.rate_Product1[11] = Number(cboxRateMode1.currentIndex)
        Settings.rate_Product1[12] = Number(cboxRateControlType1.currentIndex)
        Settings.rate_Product1[13] = Number(cboxRateCoverageUnits1.currentIndex)
        Settings.rate_ProductName[1] = productName1.text;
        mandatory.visible = false
        mandatory1.visible = false
        aog.modules_send_PGN32502();

    }

    Label{
        id: top
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 10
        font.bold: true
        text: qsTr("Rate Control")
    }
    Rectangle{
        id: zoneProduct0
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: top.bottom
        anchors.bottom: parent.bottom
        color: aog.backgroundColor
        visible: true
    RowLayout{
        id: topRateButtons
        anchors.right: parent.right
        anchors.rightMargin: 20 * theme.scaleWidth
        anchors.leftMargin: 20 * theme.scaleWidth
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottomMargin: 10 * theme.scaleHeight
        height: children.height

        IconButton{
            id: rateProductPrev
            Layout.alignment: Qt.AlignLeft
            icon.source: prefix + "/images/ArrowLeft.png"
            onClicked: {zoneProduct0.visible = false; zoneProduct1.visible = true;}
        }
        TextField{
            id: productName0
            Layout.alignment: Qt.AlignCenter
            selectByMouse: true
            placeholderText: qsTr("Product Name")
        }
        IconButtonTransparent{
            id: rateProductNext
            icon.source: prefix + "/images/ArrowRight.png"
            Layout.alignment: Qt.AlignRight
            onClicked: {zoneProduct0.visible = false; zoneProduct1.visible = true;}
        }
    }
    GridLayout{
        flow: Grid.LeftToRight
        columns: 5
        rows: 3
        anchors.bottom: back.top
        anchors.top: topRateButtons.bottom
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
            boundValue: Settings.rate_Product0[0]
            onValueModified: {
                Settings.rate_Product0[0] = value
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
            id: prodDensityBox
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn.checked
            boundValue: Settings.rate_Product0[1]
            onValueModified: {
                Settings.rate_Product0[1] = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("ProdDensity: ")
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
            boundValue: Settings.rate_Product0[3]
            onValueModified: {
                Settings.rate_Product0[3] = value
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
            boundValue: Settings.rate_Product0[4]
            onValueModified: {
                Settings.rate_Product0[4] = value
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
            boundValue: Settings.rate_Product0[5]
            onValueModified:{
                Settings.rate_Product0[5] = value
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
            boundValue: Settings.rate_Product0[6]
            onValueModified: {
                Settings.rate_Product0[6] = value
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
            boundValue: Settings.rate_Product0[7]
            onValueModified:{
                Settings.rate_Product0[7] = value
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
            boundValue: Settings.rate_Product0[8]
            onValueModified:{
                Settings.rate_Product0[8] = value
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
            boundValue: Settings.rate_Product0[10]
            onValueModified:{
                Settings.rate_Product0[10] = value
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
            boundValue: Settings.rate_Product0[9]
            onValueModified:{
                Settings.rate_Product0[9] = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Sensor Count")
                font.bold: true
                anchors.top: parent.bottom
            }

        }

        ComboBoxCustomized {
            id: cboxRateControlType
            enabled: cboxIsRateControlOn.checked
            editable: false
            model: ListModel {
                ListElement {text: qsTr("Standard")}
                ListElement {text: qsTr("Combo Close")}
                ListElement {text: qsTr("Motor")}
                ListElement {text: qsTr("Combo Timed")}
                ListElement {text: qsTr("Fan")}
            }
            text: qsTr("Control Type")
            onActivated: mandatory.visible = true
    }

            ComboBoxCustomized {
                id: cboxRateMode
                enabled: cboxIsRateControlOn.checked
                editable: false
                model: ListModel {
                    ListElement {text: qsTr("Section UPM")}
                    ListElement {text: qsTr("Constant UPM")}
                    ListElement {text: qsTr("Applied rate")}
                    ListElement {text: qsTr("Target rate")}
                }
                text: qsTr("Mode")
                onActivated: mandatory.visible = true
        }
            ComboBoxCustomized {
                id: cboxRateCoverageUnits
                enabled: cboxIsRateControlOn.checked
                editable: false
                model: ListModel {
                    ListElement {text: qsTr("Acres")}
                    ListElement {text: qsTr("Hectare")}
                    ListElement {text: qsTr("Minutes")}
                    ListElement {text: qsTr("Hours")}
                }
                text: qsTr("Coverage Units")
                onActivated: mandatory.visible = true
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
        onClicked: aog.rate_pwm_auto(0)
        enabled: cboxIsRateControlOn.checked
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
        onClicked: aog.rate_bump_pwm(true, 0)
        enabled: cboxIsRateControlOn.checked
        Label{
            anchors.bottom: parent.top
            anchors.bottomMargin: 20 * theme.scaleHeight
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Manual PWM ") + aog.actualRatePWM
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
        onClicked: aog.rate_bump_pwm(false, 0)
        enabled: cboxIsRateControlOn.checked
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
    Rectangle{
        id: zoneProduct1
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: top.bottom
        anchors.bottom: parent.bottom
        color: aog.backgroundColor
        visible: false
    RowLayout{
        id: topRateButtons1
        anchors.right: parent.right
        anchors.rightMargin: 20 * theme.scaleWidth
        anchors.leftMargin: 20 * theme.scaleWidth
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottomMargin: 10 * theme.scaleHeight
        height: children.height

        IconButton{
            id: rateProductPrev1
            Layout.alignment: Qt.AlignLeft
            icon.source: prefix + "/images/ArrowLeft.png"
            onClicked: {zoneProduct1.visible = false; zoneProduct0.visible = true;}
        }
        TextField{
            id: productName1
            Layout.alignment: Qt.AlignCenter
            selectByMouse: true
            placeholderText: qsTr("Product Name")
        }
        IconButtonTransparent{
            id: rateProductNext1
            icon.source: prefix + "/images/ArrowRight.png"
            Layout.alignment: Qt.AlignRight
            onClicked: {zoneProduct1.visible = false; zoneProduct0.visible = true;}
        }
    }
    GridLayout{
        flow: Grid.LeftToRight1
        columns: 5
        rows: 3
        anchors.bottom: back1.top
        anchors.top: topRateButtons1.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 50 * theme.scaleWidth
        anchors.rightMargin: 20 * theme.scaleWidth


        SpinBoxCustomized{
            id: moduleID1
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn1.checked
            boundValue: Settings.rate_Product1[0]
            onValueModified: {
                Settings.rate_Product1[0] = value
                mandatory1.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Module ID: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCustomized{
            id: prodDensityBox1
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn1.checked
            boundValue: Settings.rate_Product1[1]
            onValueModified: {
                Settings.rate_Product1[1] = value
                mandatory1.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("ProdDensity: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCustomized{
            id: rateKP1
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn1.checked
            boundValue: Settings.rate_Product1[3]
            onValueModified: {
                Settings.rate_Product1[3] = value
                mandatory1.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("PID KP: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCustomized{
            id: rateKI1
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn1.checked
            boundValue: Settings.rate_Product1[4]
            onValueModified: {
                Settings.rate_Product1[4] = value
                mandatory1.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("PID KI: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCustomized{
            id: rateKD1
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn1.checked
            boundValue: Settings.rate_Product1[5]
            onValueModified:{
                Settings.rate_Product0[1] = value
                mandatory1.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("PID KD: ")
                font.bold: true
                anchors.top: parent.bottom
            }

        }
        SpinBoxCustomized{
            id: rateMinPWM1
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn1.checked
            boundValue: Settings.rate_Product1[6]
            onValueModified: {
                Settings.rate_Product1[6] = value
                mandatory1.visible = true
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
            id: rateMaxPWM1
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn1.checked
            boundValue: Settings.rate_Product1[7]
            onValueModified:{
                Settings.rate_Product1[7] = value
                mandatory1.visible = true
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
            id: ratePIDscale1
            from: 0
            to:255
            editable: true
            enabled: cboxIsRateControlOn1.checked
            boundValue: Settings.rate_Product1[8]
            onValueModified:{
                Settings.rate_Product1[8] = value
                mandatory1.visible = true
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
            id: setRate1
            from: 0
            to:1000
            editable: true
            enabled: cboxIsRateControlOn1.checked
            boundValue: Settings.rate_Product1[10]
            onValueModified:{
                Settings.rate_Product1[10] = value
                mandatory1.visible = true
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
            id: rateSensor1
            from: 0
            to:1000
            editable: true
            enabled: cboxIsRateControlOn1.checked
            boundValue: Settings.rate_Product1[9]
            onValueModified:{
                Settings.rate_Product1[9] = value
                mandatory1.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Sensor Count")
                font.bold: true
                anchors.top: parent.bottom
            }

        }

        ComboBoxCustomized {
            id: cboxRateControlType1
            enabled: cboxIsRateControlOn1.checked
            editable: false
            model: ListModel {
                ListElement {text: qsTr("Standard")}
                ListElement {text: qsTr("Combo Close")}
                ListElement {text: qsTr("Motor")}
                ListElement {text: qsTr("Combo Timed")}
                ListElement {text: qsTr("Fan")}
            }
            text: qsTr("Control Type")
            onActivated: mandatory1.visible = true
    }

            ComboBoxCustomized {
                id: cboxRateMode1
                enabled: cboxIsRateControlOn1.checked
                editable: false
                model: ListModel {
                    ListElement {text: qsTr("Section UPM")}
                    ListElement {text: qsTr("Constant UPM")}
                    ListElement {text: qsTr("Applied rate")}
                    ListElement {text: qsTr("Target rate")}
                }
                text: qsTr("Mode")
                onActivated: mandatory1.visible = true
        }
            ComboBoxCustomized {
                id: cboxRateCoverageUnits1
                enabled: cboxIsRateControlOn1.checked
                editable: false
                model: ListModel {
                    ListElement {text: qsTr("Acres")}
                    ListElement {text: qsTr("Hectare")}
                    ListElement {text: qsTr("Minutes")}
                    ListElement {text: qsTr("Hours")}
                }
                text: qsTr("Coverage Units")
                onActivated: mandatory1.visible = true
        }



    }


    IconButtonTransparent{
        id: back1
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        enabled: cboxIsRateControlOn1.checked
        icon.source: prefix + "/images/back-button.png"
        onClicked: {
            rateMinPWM1.boundValue = 100
            rateMaxPWM1.boundValue = 255
            rateKP1.boundValue = 10
            rateKI1.boundValue = 0
            rateKD1.boundValue = 0
            ratePIDscale1.boundValue = 0
            rateSensor1.boundValue = 600
            setRate1.boundValue = 100
        }
    }
    IconButtonTransparent{
        id: loadSetBlockage1
        anchors.bottom: parent.bottom
        anchors.left: back1.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        enabled: cboxIsRateControlOn1.checked
        icon.source: prefix + "/images/UpArrow64.png"
        onClicked: {
            load_settings()
            mandatory1.visible = true
        }
    }
    IconButtonColor{
        id: cboxIsRateControlOn1
        height: loadSetBlockage1.height
        anchors.bottom: parent.bottom
        anchors.left: loadSetBlockage1.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        icon.source: prefix + "/images/SwitchOff.png"
        iconChecked: prefix + "/images/SwitchOn.png"
        checkable: true
        onClicked: mandatory1.visible = true
        }
    IconButtonTransparent{
        id: ratePWMauto1
        height: loadSetBlockage1.height
        anchors.bottom: parent.bottom
        anchors.left: cboxIsRateControlOn1.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        icon.source: prefix + "/images/AutoStop.png"
        onClicked: aog.rate_pwm_auto1(1)
        enabled: cboxIsRateControlOn1.checked
}
    IconButtonTransparent{
        id: ratePWMUP1
        height: loadSetBlockage1.height
        anchors.bottom: parent.bottom
        anchors.left: ratePWMauto1.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        icon.source: prefix + "/images/UpArrow64.png"
        onClicked: aog.rate_bump_pwm(true, 1)
        enabled: cboxIsRateControlOn1.checked
        Label{
            anchors.bottom: parent.top
            anchors.bottomMargin: 20 * theme.scaleHeight
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Manual PWM ") + aog.actualRatePWM1
        }
}
    IconButtonTransparent{
        id: ratePWMDN1
        height: loadSetBlockage1.height
        anchors.bottom: parent.bottom
        anchors.left: ratePWMUP1.right
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottomMargin: 20 * theme.scaleHeight
        anchors.rightMargin: 20 * theme.scaleHeight
        anchors.leftMargin: 20 * theme.scaleHeight
        icon.source: prefix + "/images/DnArrow64.png"
        onClicked: aog.rate_bump_pwm(false, 1)
        enabled: cboxIsRateControlOn1.checked
}

    IconButtonTransparent{
        id: btnPinsSave1
        anchors.right: mandatory1.left
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
        id: mandatory1
        anchors.right: parent.right
        anchors.verticalCenter: btnPinsSave1.verticalCenter
        anchors.rightMargin: 20 * theme.scaleWidth
        visible: false
        source: prefix + "/images/Config/ConSt_Mandatory.png"
        height: back1.width
    }
}

}
