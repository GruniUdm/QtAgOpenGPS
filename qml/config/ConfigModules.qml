// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Config Hyd lift timing etc
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion
import Settings

import ".."
import "../components"

/*todo:
 can't find setting for "invert relays"
 not sure about the "send button"
 */
Rectangle{
    id: configModules
    anchors.fill: parent
    color: aog.backgroundColor

    onVisibleChanged: {
        load_settings()
    }

    function load_settings()
    {
        if (visible) {
            var sett = Settings.ardMac_setting0

            if ((sett & 1) === 0 ) cboxMachInvertRelays.checked = false
            else cboxMachInvertRelays.checked = true

            if ((sett & 2) === 0 ) cboxIsHydOn.checked = false
            else cboxIsHydOn.checked = true

            nudRaiseTime.value = Settings.ardMac_hydRaiseTime
            nudLowerTime.value = Settings.ardMac_hydLowerTime

            nudUser1.value = Settings.ardMac_user1
            nudUser2.value = Settings.ardMac_user2
            nudUser3.value = Settings.ardMac_user3
            nudUser4.value = Settings.ardMac_user4

            unsaved.visible = false
        }
    }

    function save_settings() {
        var set = 1
        var reset = 2046
        var sett = 0

        if (cboxMachInvertRelays.checked) sett |= set
        else sett &= reset

        set <<=1
        reset <<= 1
        reset += 1

        if(cboxIsHydOn.checked) sett |= set
        else sett &= reset

        Settings.ardMac_setting0 = sett
        Settings.ardMac_hydRaiseTime = nudRaiseTime.value
        Settings.ardMac_hydLowerTime = nudLowerTime.value

        Settings.ardMac_user1 = nudUser1.value
        Settings.ardMac_user2 = nudUser2.value
        Settings.ardMac_user3 = nudUser3.value
        Settings.ardMac_user4 = nudUser4.value

        Settings.vehicle_hydraulicLiftLookAhead = nudHydLiftLookAhead.value
        Settings.ardMac_isHydEnabled = cboxIsHydOn.checked

        //set pgns
        //signal handler will do the folling:
        /*
            mf.p_238.pgn[mf.p_238.set0] = property_setArdMac_setting0
            mf.p_238.pgn[mf.p_238.raiseTime] = property_setArdMac_hydRaiseTime
            mf.p_238.pgn[mf.p_238.lowerTime] = property_setArdMac_hydLowerTime

            mf.p_238.pgn[mf.p_238.user1] = property_setArd_user1
            mf.p_238.pgn[mf.p_238.user2] = property_setArd_user2
            mf.p_238.pgn[mf.p_238.user3] = property_setArd_user3
            mf.p_238.pgn[mf.p_238.user4] = property_setArd_user3
            */

        aog.modules_send_238()
        //pboxSendMachine.Visible = false
    }

    Text {
        id: titleText
        text: qsTr("Machine Module")
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top:parent.top
        anchors.topMargin: 10 * theme.scaleHeight
        anchors.bottomMargin: 10 * theme.scaleHeight
        anchors.leftMargin: 10 * theme.scaleWidth
        anchors.rightMargin: 10 * theme.scaleWidth
    }
    Rectangle{
        id: hydConfig
        anchors.left: parent.left
        color: aog.backgroundColor
        border.color: aog.blackDayWhiteNight
        anchors.top: titleText.bottom
        anchors.topMargin: 20 * theme.scaleHeight
        anchors.bottom: cboxMachInvertRelays.top
        anchors.leftMargin: 10 * theme.scaleWidth
        width: parent.width * .66
        Text {
            id: hydText
            anchors.top: parent.top
            anchors.topMargin: 10 * theme.scaleHeight
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Hydraulic Lift Config")
        }
        GridLayout{
            anchors.fill: parent
            flow: Grid.LeftToRight
            anchors.leftMargin: 10 * theme.scaleHeight
            rows: 2
            columns: 3
            IconButtonColor{
                id: cboxIsHydOn
                height: 130 * theme.scaleHeight
                width: 170 * theme.scaleWidth
                icon.source: prefix + "/images/SwitchOff.png"
                iconChecked: prefix + "/images/SwitchOn.png"

                checkable: true
                onClicked: unsaved.visible = true
            }
            SpinBoxCustomized{
                id: nudRaiseTime
                from:1
                //boundValue: Settings.ardMac_hydRaiseTime
                //onValueChanged: Settings.ardMac_hydRaiseTime = value
                to: 255
                editable: true
                enabled: cboxIsHydOn.checked
                text: qsTr("Raise Time(secs)")
                onValueChanged: unsaved.visible = true
            }
            Image{
                source: prefix + "/images/Config/ConMa_LiftRaiseTime.png"
                width: 200 * theme.scaleWidth
                height: 200 * theme.scaleHeight
                fillMode: Image.Stretch
            }
            SpinBoxCustomized{
                id: nudHydLiftLookAhead
                from:1
                //boundValue: Settings.vehicle_hydraulicLiftLookAhead
                //onValueChanged: Settings.vehicle_hydraulicLiftLookAhead = value
                to: 20
                editable: true
                enabled: cboxIsHydOn.checked
                text: qsTr("Hyd Lift Look Ahead (secs)")
                onValueChanged: unsaved.visible = true
                decimals: 1
            }
            SpinBoxCustomized{
                id: nudLowerTime
                from:1
                //boundValue: Settings.ardMac_hydLowerTime
                //onValueChanged: Settings.ardMac_hydLowerTime = value
                to: 255
                editable: true
                enabled: cboxIsHydOn.checked
                text: qsTr("Lower Time(secs)")
                onValueChanged: unsaved.visible = true
            }
            Image{
                source: prefix + "/images/Config/ConMa_LiftLowerTime.png"
                width: 200 * theme.scaleWidth
                height: 200 * theme.scaleHeight
                fillMode: Image.Stretch
            }
        }
    }
    IconButtonColor{
        id: cboxMachInvertRelays
        anchors.horizontalCenter: hydConfig.horizontalCenter
        anchors.bottom: parent.bottom
        text: qsTr("Invert Relays")
        anchors.topMargin: 10 * theme.scaleHeight
        anchors.bottomMargin: 10 * theme.scaleHeight
        anchors.leftMargin: 10 * theme.scaleWidth
        anchors.rightMargin: 10 * theme.scaleWidth
        icon.source: prefix + "/images/Config/ConSt_InvertRelay.png"
        checkable: true
        enabled: cboxIsHydOn.checked
        onClicked: unsaved.visible = true
    }

    Rectangle{
        anchors.bottom: parent.bottom
        anchors.left: hydConfig.right
        anchors.right: parent.right
        anchors.top: titleText.bottom
        anchors.topMargin: 5 * theme.scaleHeight
        anchors.bottomMargin: 5 * theme.scaleHeight
        anchors.leftMargin: 5 * theme.scaleWidth
        anchors.rightMargin: 5 * theme.scaleWidth
        ColumnLayout{
            anchors.top: parent.top
            width: parent.width
            anchors.bottom: modulesSave.top
            anchors.bottomMargin: 20 * theme.scaleHeight
            anchors.horizontalCenter: parent.horizontalCenter
            SpinBoxCustomized{
                id: nudUser1
                from: 0
                //boundValue: Settings.ardMac_user1
                //onValueChanged: Settings.ardMac_user1 = value
                to: 255
                editable: true
                text: qsTr("User 1")
                onValueChanged: unsaved.visible = true
            }
            SpinBoxCustomized{
                id: nudUser2
                from: 0
                //boundValue: Settings.ardMac_user2
                //onValueChanged: Settings.ardMac_user2 = value
                to: 255
                editable: true
                text: qsTr("User 2")
                onValueChanged: unsaved.visible = true
            }
            SpinBoxCustomized{
                id: nudUser3
                from: 0
                //boundValue: Settings.ardMac_user3
                //onValueChanged: Settings.ardMac_user3 = value
                to: 255
                editable: true
                text: qsTr("User 3")
                onValueChanged: unsaved.visible = true
            }
            SpinBoxCustomized{
                id: nudUser4
                from: 0
                //boundValue: Settings.ardMac_user4
                //onValueChanged: Settings.ardMac_user4 = value
                to: 255
                editable: true
                text: qsTr("User 4")
                onValueChanged: unsaved.visible = true
            }
        }
        IconButtonTransparent{
            id: modulesSave
            objectName: "btnModulesSave"
            anchors.right: parent.right
            anchors.topMargin: 20 * theme.scaleHeight
            anchors.leftMargin: 20 * theme.scaleWidth
            anchors.bottomMargin: 20 * theme.scaleHeight
            anchors.rightMargin: 20 * theme.scaleWidth
            anchors.bottom: parent.bottom
            icon.source: prefix + "/images/ToolAcceptChange.png"
            onClicked: { save_settings() ; unsaved.visible = false }

            Text{
                id: modulesSaveLabel
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.left
                anchors.rightMargin: 5 * theme.scaleWidth
                text: qsTr("Send + Save")
            }
        }
        Image {
            id: unsaved
            width: 100 * theme.scaleWidth
            height: 100 * theme.scaleHeight
            anchors.right: modulesSave.left
            anchors.rightMargin: modulesSaveLabel.width + 5
            anchors.verticalCenter: modulesSave.verticalCenter
            visible: false
            source: prefix + "/images/Config/ConSt_Mandatory.png"
            fillMode: Image.Stretch
        }
    }
}
