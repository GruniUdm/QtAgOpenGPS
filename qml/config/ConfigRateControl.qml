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
        if (visible) load_settings(0)
    }
    property int prodID: 0;

    function load_settings(ID) {
if (ID === 0) {
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
        productName.text = Settings.rate_ProductName[0];}
else if (ID === 1) {
        moduleID.value = Settings.rate_Product1[0]
        prodDensityBox.value = Settings.rate_Product1[1]
        cboxIsRateControlOn.checked = (Settings.rate_Product1[2]>0)?true:false
        rateKP.value = Settings.rate_Product1[3]
        rateKI.value = Settings.rate_Product1[4]
        rateKD.value = Settings.rate_Product1[5]
        rateMinPWM.value = Settings.rate_Product1[6]
        rateMaxPWM.value = Settings.rate_Product1[7]
        ratePIDscale.value = Settings.rate_Product1[8]
        rateSensor.value = Settings.rate_Product1[9]
        setRate.value = Settings.rate_Product1[10]
        cboxRateMode.currentIndex = Settings.rate_Product1[11]
        cboxRateControlType.currentIndex = Settings.rate_Product1[12];
        cboxRateCoverageUnits.currentIndex = Settings.rate_Product1[13];
        productName.text = Settings.rate_ProductName[1];}

else if (ID === 2) {
        moduleID.value = Settings.rate_Product2[0]
        prodDensityBox.value = Settings.rate_Product2[1]
        cboxIsRateControlOn.checked = (Settings.rate_Product2[2]>0)?true:false
        rateKP.value = Settings.rate_Product2[3]
        rateKI.value = Settings.rate_Product2[4]
        rateKD.value = Settings.rate_Product2[5]
        rateMinPWM.value = Settings.rate_Product2[6]
        rateMaxPWM.value = Settings.rate_Product2[7]
        ratePIDscale.value = Settings.rate_Product2[8]
        rateSensor.value = Settings.rate_Product2[9]
        setRate.value = Settings.rate_Product2[10]
        cboxRateMode.currentIndex = Settings.rate_Product2[11]
        cboxRateControlType.currentIndex = Settings.rate_Product2[12];
        cboxRateCoverageUnits.currentIndex = Settings.rate_Product2[13];
        productName.text = Settings.rate_ProductName[2];}
else if (ID === 3) {
        moduleID.value = Settings.rate_Product3[0]
        prodDensityBox.value = Settings.rate_Product3[1]
        cboxIsRateControlOn.checked = (Settings.rate_Product3[2]>0)?true:false
        rateKP.value = Settings.rate_Product3[3]
        rateKI.value = Settings.rate_Product3[4]
        rateKD.value = Settings.rate_Product3[5]
        rateMinPWM.value = Settings.rate_Product3[6]
        rateMaxPWM.value = Settings.rate_Product3[7]
        ratePIDscale.value = Settings.rate_Product3[8]
        rateSensor.value = Settings.rate_Product3[9]
        setRate.value = Settings.rate_Product3[10]
        cboxRateMode.currentIndex = Settings.rate_Product3[11]
        cboxRateControlType.currentIndex = Settings.rate_Product3[12];
        cboxRateCoverageUnits.currentIndex = Settings.rate_Product3[13];
        productName.text = Settings.rate_ProductName[3];}

        mandatory.visible = false
        }

    function save_settings(ID) {
if (ID === 0) {
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
        Settings.rate_ProductName[0] = productName.text;}
if (ID === 1) {
        Settings.rate_Product1[0] = moduleID.value;
        Settings.rate_Product1[1] = prodDensityBox.value
        Settings.rate_Product1[2] = cboxIsRateControlOn.checked?1:0
        Settings.rate_Product1[3] = rateKP.value
        Settings.rate_Product1[4] = rateKI.value
        Settings.rate_Product1[5] = rateKD.value
        Settings.rate_Product1[6] = rateMinPWM.value
        Settings.rate_Product1[7] = rateMaxPWM.value
        Settings.rate_Product1[8] = ratePIDscale.value
        Settings.rate_Product1[9] = rateSensor.value
        Settings.rate_Product1[10] = setRate.value
        Settings.rate_Product1[11] = Number(cboxRateMode.currentIndex)
        Settings.rate_Product1[12] = Number(cboxRateControlType.currentIndex)
        Settings.rate_Product1[13] = Number(cboxRateCoverageUnits.currentIndex)
        Settings.rate_ProductName[1] = productName.text;}
if (ID === 2) {
        Settings.rate_Product2[0] = moduleID.value;
        Settings.rate_Product2[1] = prodDensityBox.value
        Settings.rate_Product2[2] = cboxIsRateControlOn.checked?1:0
        Settings.rate_Product2[3] = rateKP.value
        Settings.rate_Product2[4] = rateKI.value
        Settings.rate_Product2[5] = rateKD.value
        Settings.rate_Product2[6] = rateMinPWM.value
        Settings.rate_Product2[7] = rateMaxPWM.value
        Settings.rate_Product2[8] = ratePIDscale.value
        Settings.rate_Product2[9] = rateSensor.value
        Settings.rate_Product2[10] = setRate.value
        Settings.rate_Product2[11] = Number(cboxRateMode.currentIndex)
        Settings.rate_Product2[12] = Number(cboxRateControlType.currentIndex)
        Settings.rate_Product2[13] = Number(cboxRateCoverageUnits.currentIndex)
        Settings.rate_ProductName[2] = productName.text;}
if (ID === 3) {
        Settings.rate_Product3[0] = moduleID.value;
        Settings.rate_Product3[1] = prodDensityBox.value
        Settings.rate_Product3[2] = cboxIsRateControlOn.checked?1:0
        Settings.rate_Product3[3] = rateKP.value
        Settings.rate_Product3[4] = rateKI.value
        Settings.rate_Product3[5] = rateKD.value
        Settings.rate_Product3[6] = rateMinPWM.value
        Settings.rate_Product3[7] = rateMaxPWM.value
        Settings.rate_Product3[8] = ratePIDscale.value
        Settings.rate_Product3[9] = rateSensor.value
        Settings.rate_Product3[10] = setRate.value
        Settings.rate_Product3[11] = Number(cboxRateMode.currentIndex)
        Settings.rate_Product3[12] = Number(cboxRateControlType.currentIndex)
        Settings.rate_Product3[13] = Number(cboxRateCoverageUnits.currentIndex)
        Settings.rate_ProductName[3] = productName.text;}
        mandatory.visible = false

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

    RowLayout{
        id: topRateButtons
        anchors.right: parent.right
        anchors.rightMargin: 20 * theme.scaleWidth
        anchors.leftMargin: 20 * theme.scaleWidth
        anchors.left: parent.left
        anchors.top: top.bottom
        anchors.bottomMargin: 10 * theme.scaleHeight
        height: children.height

        IconButton{
            id: rateProductPrev
            Layout.alignment: Qt.AlignLeft
            icon.source: prefix + "/images/ArrowLeft.png"
            onClicked: {
                if (prodID > 0) {prodID--} {
                    load_settings(prodID)
                }
            }
        }
        TextField{
            id: productName
            Layout.alignment: Qt.AlignCenter
            selectByMouse: true
            placeholderText: qsTr("Product Name")
        }
        IconButtonTransparent{
            id: rateProductNext
            icon.source: prefix + "/images/ArrowRight.png"
            Layout.alignment: Qt.AlignRight
            onClicked: {
                if (prodID < 4) {prodID++} {
                    load_settings(prodID)
                }
            }
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
                Settings.rate_Product[0] = value
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
            boundValue: Settings.rate_Product[1]
            onValueModified: {
                Settings.rate_Product[1] = value
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
            boundValue: Settings.rate_Product[3]
            onValueModified: {
                Settings.rate_Product[3] = value
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
            boundValue: Settings.rate_Product[4]
            onValueModified: {
                Settings.rate_Product[4] = value
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
            boundValue: Settings.rate_Product[5]
            onValueModified:{
                Settings.rate_Product[5] = value
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
            boundValue: Settings.rate_Product[6]
            onValueModified: {
                Settings.rate_Product[6] = value
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
            boundValue: Settings.rate_Product[7]
            onValueModified:{
                Settings.rate_Product[7] = value
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
            boundValue: Settings.rate_Product[8]
            onValueModified:{
                Settings.rate_Product[8] = value
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
            boundValue: Settings.rate_Product[9]
            onValueModified:{
                Settings.rate_Product[9] = value
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
            load_settings(moduleID.value)
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
        onClicked: aog.rate_pwm_auto(moduleID.value)
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
        onClicked: aog.rate_bump_pwm(true, moduleID.value)
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
        onClicked: aog.rate_bump_pwm(false, moduleID.value)
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
        onClicked: save_settings(moduleID.value)


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
