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
    id: configModulesBlockage
    anchors.fill: parent
    color: aog.backgroundColor
    visible: false

    onVisibleChanged: {
        if (visible) load_settings()
    }


    function load_settings() {

        graincountMin.boundValue = settings.setBlockcountMin
        graincountMax.boundValue = settings.setBlockcountMax
        modulerows1.boundValue = settings.setBlockrow1
        modulerows2.boundValue = settings.setBlockrow2
        modulerows3.boundValue = settings.setBlockrow3
        modulerows4.boundValue = settings.setBlockrow4


        mandatory.visible = false

    }

    function save_settings() {


        settings.setBlockcountMin = graincountMin.value
        settings.setBlockcountMax = graincountMax.value
        settings.setBlockrow1 = modulerows1.value
        settings.setBlockrow2 = modulerows2.value
        settings.setBlockrow3 = modulerows3.value
        settings.setBlockrow4 = modulerows4.value
        blockageRows.setSizes()
        mandatory.visible = false
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


        SpinBoxCM{
            id: modulerows1
            from: 0
            to:16
            boundValue: settings.setBlockrow1
            onValueModified: settings.setBlockrow1 = value
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Rows on module 1: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCM{
            id: modulerows2
            from: 0
            to:16
            boundValue: settings.setBlockrow2
            onValueModified: settings.setBlockrow2 = value
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Rows on module 2: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCM{
            id: modulerows3
            from: 0
            to:16
            boundValue: settings.setBlockrow3
            onValueModified: settings.setBlockrow3 = value
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Rows on module 3: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCM{
            id: modulerows4
            from: 0
            to:16
            boundValue: settings.setBlockrow4
            onValueModified: settings.setBlockrow4 = value
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Rows on module 4: ")
                font.bold: true
                anchors.top: parent.bottom
            }

        }
        SpinBoxCM{
            id: graincountMin
            from: 0
            to:1000
            boundValue: settings.setBlockcountMin
            onValueModified: settings.setBlockcountMin = value
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Grain countMin: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }
        SpinBoxCM{
            id: graincountMax
            from: 0
            to:1000
            boundValue: settings.setBlockcountMax
            onValueModified: settings.setBlockcountMax = value
            anchors.bottomMargin: 10 * theme.scaleHeight
            TextLine{
                text: qsTr("Grain countMax: ")
                font.bold: true
                anchors.top: parent.bottom
            }
        }


    }
    IconButtonTransparent{
        id: back
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 20
        icon.source: prefix + "/images/back-button.png"
        onClicked: {
            graincountMin.boundValue = 0
            graincountMax.boundValue = 0
            modulerows1.boundValue = 0
            modulerows2.boundValue = 0
            modulerows3.boundValue = 0
            modulerows4.boundValue = 0
            crops.currentIndex[0] = 0

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