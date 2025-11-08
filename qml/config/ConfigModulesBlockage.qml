// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Pinout for hyd lift/sections
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
//import Settings
import AOG


import ".."
import "../components"

Rectangle{
    id: configModulesBlockage
    anchors.fill: parent
    color: aogInterface.backgroundColor
    visible: false

    onVisibleChanged: {
        if (visible) load_settings()
    }


    function load_settings() {

        // Threading Phase 1: Seed blockage configuration
        graincountMin.boundValue = SettingsManager.seed_blockCountMin
        graincountMax.boundValue = SettingsManager.seed_blockCountMax
        modulerows1.boundValue = SettingsManager.seed_blockRow1
        modulerows2.boundValue = SettingsManager.seed_blockRow2
        modulerows3.boundValue = SettingsManager.seed_blockRow3
        modulerows4.boundValue = SettingsManager.seed_blockRow4
        cboxIsBlockageOn.checked = SettingsManager.seed_blockageIsOn


        mandatory.visible = false

    }

    function save_settings() {


        // Threading Phase 1: Save seed blockage configuration
        SettingsManager.seed_blockCountMin = graincountMin.value
        SettingsManager.seed_blockCountMax = graincountMax.value
        SettingsManager.seed_blockRow1 = modulerows1.value
        SettingsManager.seed_blockRow2 = modulerows2.value
        SettingsManager.seed_blockRow3 = modulerows3.value
        SettingsManager.seed_blockRow4 = modulerows4.value
        SettingsManager.seed_blockageIsOn = cboxIsBlockageOn.checked
        SettingsManager.seed_numRows = Number(SettingsManager.seed_blockRow1 + SettingsManager.seed_blockRow2 + SettingsManager.seed_blockRow3 + SettingsManager.seed_blockRow4)
        blockageRows.setSizes()
        mandatory.visible = false

        aog.blockageMonitoring() // Qt 6.8 MODERN: Direct Q_INVOKABLE call
    }
    Label{
        id: top
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 10
        text: qsTr("Planter Monitor")
    }

    GridLayout{
        flow: Grid.LeftToRight
        columns: 4
        rows: 5
        anchors.bottom: back.top
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 10 * theme.scaleHeight
        anchors.bottomMargin: 10 * theme.scaleHeight
        anchors.leftMargin: 10 * theme.scaleWidth
        anchors.rightMargin: 10 * theme.scaleWidth


        SpinBoxCustomized{
            id: modulerows1
            from: 0
            to:255
            editable: true
            enabled: cboxIsBlockageOn.checked
            // Threading Phase 1: Module row 1 configuration
            boundValue: SettingsManager.seed_blockRow1
            onValueModified: {
                SettingsManager.seed_blockRow1 = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Rows on module 1: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCustomized{
            id: modulerows2
            from: 0
            to:255
            editable: true
            enabled: cboxIsBlockageOn.checked
            // Threading Phase 1: Module row 2 configuration
            boundValue: SettingsManager.seed_blockRow2
            onValueModified: {
                SettingsManager.seed_blockRow2 = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Rows on module 2: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCustomized{
            id: modulerows3
            from: 0
            to:255
            editable: true
            enabled: cboxIsBlockageOn.checked
            // Threading Phase 1: Module row 3 configuration
            boundValue: SettingsManager.seed_blockRow3
            onValueModified: {
                SettingsManager.seed_blockRow3 = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Rows on module 3: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCustomized{
            id: modulerows4
            from: 0
            to:255
            editable: true
            enabled: cboxIsBlockageOn.checked
            // Threading Phase 1: Module row 4 configuration
            boundValue: SettingsManager.seed_blockRow4
            onValueModified:{
                SettingsManager.seed_blockRow4 = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Rows on module 4: ")
                font.bold: true
                anchors.top: parent.bottom
            }

        }
        SpinBoxCustomized{
            id: graincountMin
            from: 0
            to:10000
            editable: true
            enabled: cboxIsBlockageOn.checked
            // Threading Phase 1: Seed block count minimum
            boundValue: SettingsManager.seed_blockCountMin
            onValueModified: {
                SettingsManager.seed_blockCountMin = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Minimum ")
                font.bold: true
                anchors.top: parent.bottom
            }
            Label{
                anchors.left: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: Utils.per_unit()
            }
        }
        SpinBoxCustomized{
            Layout.alignment: Qt.AlignRight
            id: graincountMax
            from: 0
            to:10000
            editable: true
            enabled: cboxIsBlockageOn.checked
            // Threading Phase 1: Seed block count maximum
            boundValue: SettingsManager.seed_blockCountMax
            onValueModified:{
                SettingsManager.seed_blockCountMax = value
                mandatory.visible = true
            }
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Maximum ")
                font.bold: true
                anchors.top: parent.bottom
            }
            Label{
                anchors.left: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: Utils.per_unit()
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
        enabled: cboxIsBlockageOn.checked
        icon.source: prefix + "/images/back-button.png"
        onClicked: {
            graincountMin.boundValue = 0
            graincountMax.boundValue = 0
            modulerows1.boundValue = 0
            modulerows2.boundValue = 0
            modulerows3.boundValue = 0
            modulerows4.boundValue = 0
            crops.currentIndex[0] = 0
            mandatory.visible = true

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
        enabled: cboxIsBlockageOn.checked
        icon.source: prefix + "/images/UpArrow64.png"
        onClicked: {
            load_settings()
            mandatory.visible = true
        }
    }
    IconButtonColor{
        id: cboxIsBlockageOn
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
