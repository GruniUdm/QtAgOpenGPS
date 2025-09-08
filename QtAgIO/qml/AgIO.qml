// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Main menu when we click the "Field" button on main screen. "New, Drive In, Open, Close, etc"
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import Settings
import QtQuick.Window
import AgIO

import "components" as Comp


Drawer {
    id: mainWindowAgIO
    width: 270 * theme.scaleWidth
    height: mainWindow.height
    visible: false
    modal: true

    Comp.Message {
        id: message
    }
    NTrip{
        id: ntrip
        visible: false
    }
    EthernetConfig {
        id: ethernetConfig
        visible: false
        // onVisibleChanged: {
        //     if(visible)
        //         ethernetConfig.load_settings()
        // }
    }
    Comp.SerialTerminalAgio {
        id: gnssConfig
        visible: false
        portBaud: agiosettings.setGnss_BaudRate
        portName: agiosettings.setGnss_SerialPort
        saveConfig: false
        onSaveConfigChanged: {
            agiosettings.setGnss_SerialPort = gnssConfig.portName
            agiosettings.setGnss_BaudRate = gnssConfig.portBaud
            }
    }
    Comp.SerialTerminalAgio {
        id: imuConfig
        visible: false
        portBaud: agiosettings.setImu_BaudRate
        portName: agiosettings.setImu_SerialPort
        saveConfig: false
        onSaveConfigChanged: {
            agiosettings.setImu_SerialPort = imuConfig.portName
            agiosettings.setImu_BaudRate = imuConfig.portBaud
            }
    }
    Comp.SerialTerminalAgio {
        id: autosteerConfig
        visible: false
        portBaud: agiosettings.setSteer_BaudRate
        portName: agiosettings.setSteer_SerialPort
        saveConfig: false
        onSaveConfigChanged: {
            agiosettings.setSteer_SerialPort = steerConfig.portName
            agiosettings.setSteer_BaudRate = steerConfig.portBaud
            }
    }
    AgIOInterface {
        id: agio
        objectName: "agio"
    }
    GPSInfo {
        id: gpsInfo
        visible: false
    }
    AgDiag {
        id: agdiag
    }

    SettingsWindow{
        id: settingsWindow
        visible: false
    }
    AdvancedMenu{
        id: advancedMenu
        function showMenu(){
            advancedMenu.visible = true
        }
    }
    BluetoothMenu{
        id: bluetoothMenu
        visible: false
    }
    UnitConversion {
        id: utils
    }

    contentItem: Rectangle{
        id: windowAgIO
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        height: mainWindowAgIO.height

        //border.color: "lightblue"
        //border.width: 2
        color: "black"

        Comp.ScrollViewExpandableColumn{
            id: mainWindowAgIOColumn
            anchors.fill: parent
            IconButtonTextBeside{
                //objectName: bluetooth
                text: qsTr("Bluetooth")
                icon.source: "../images/BlueTooth.png"
                color: agio.bluetoothConnected ? "green" : "red"
                onClicked: {
                    settingsWindow.close()
                    if(!utils.isTrue(agiosettings.setBluetooth_isOn)){ //start bt if off
                        agiosettings.setBluetooth_isOn = true
                        agio.startBluetoothDiscovery()
                        console.log("ssb")
                    }
                    bluetoothMenu.visible = true
                }
            }
            Comp.IconButtonTextBeside {
                //objectName: btnModuleIMU
                isChecked: false
                text: qsTr("IMU")
                icon.source: "../images/B_IMU.png"
                color: agio.imuConnected ? "green" : "red"
                onClicked: imuConfig.visible = !imuConfig.visible
            }
            Comp.IconButtonTextBeside {
                //objectName: btnModuleSteer
                isChecked: false
                text: qsTr("Steer")
                icon.source: "../images/Com_AutosteerModule.png"
                color: agio.steerConnected ? "green" : "red"
                onClicked: autosteerConfig.visible = !autosteerConfig.visible
            }
            Comp.IconButtonTextBeside {
                //objectName: btnModuleGPS
                isChecked: false
                text: qsTr("GPS")
                icon.source: "../images/B_GPS.png"
                color: agio.gpsConnected ? "green" : "red"
                onClicked: gnssConfig.visible = !gnssConfig.visible
            }
            Comp.IconButtonTextBeside {
                //objectName: btnModuleMachine
                isChecked: false
                text: qsTr("Machine")
                icon.source: "../images/B_Machine.png"
                color: agio.machineConnected ? "green" : "red"
            }
            Comp.IconButtonTextBeside {
                //objectName: btnModuleBlockage
                isChecked: false
                text: qsTr("Blockage")
                icon.source: "../images/B_Blockage.png"
                color: agio.blockageConnected ? "green" : "red"
                visible: Settings.seed_blockageIsOn
            }
            Comp.IconButtonTextBeside {
                //objectName: btnEthernetStatus
                isChecked: false
                text: qsTr("Ethernet")
                icon.source: "../images/B_UDP.png"
                color: agio.ethernetConnected ? "green" : "red"
                onClicked: ethernetConfig.visible = !ethernetConfig.visible
            }
            Comp.IconButtonTextBeside {
                isChecked: false
                text: qsTr("Nmea")
                icon.source: "../images/Nmea.png"
                onClicked: gpsInfo.visible = !gpsInfo.visible
            }
            // Comp.IconButtonTextBeside {
            //     //objectName: btnSettings
            //     isChecked: false
            //     text: qsTr("Settings")
            //     icon.source: "../images/Settings48.png"
            //     onClicked: settingsWindow.visible = true
            // }
            Comp.IconButtonTextBeside {
                //objectName: btnNTRIP
                isChecked: false
                text: (agiosettings.setNTRIP_isOn === false ? "Off":
                    agio.ntripStatus === 0 ? "Invalid" :
                    agio.ntripStatus === 1 ? "Authorizing" :
                    agio.ntripStatus === 2 ? "Waiting" :
                    agio.ntripStatus === 3 ? "Send GGA" :
                    agio.ntripStatus === 4 ? "Listening NTRIP":
                    agio.ntripStatus === 5 ? "Wait GPS":
                    "Unknown")

                icon.source: "../images/NtripSettings.png"
                color:  (agiosettings.setNTRIP_isOn === false ? "red":
                    agio.ntripStatus === 0 ? "red" :
                    agio.ntripStatus === 1 ? "yellow" :
                    agio.ntripStatus === 2 ? "yellow" :
                    agio.ntripStatus === 3 ? "yellow" :
                    agio.ntripStatus === 4 ? "green":
                    agio.ntripStatus === 5 ? "red":
                    "red")
                onClicked: ntrip.visible = !ntrip.visible
            }
            }
        }
    }
