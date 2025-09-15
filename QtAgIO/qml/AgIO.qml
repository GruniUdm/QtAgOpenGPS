// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Main menu when we click the "Field" button on main screen. "New, Drive In, Open, Close, etc"
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import QtQuick.Window
import AOG

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
        portBaud: AgIOSettings.setGnss_BaudRate
        portName: AgIOSettings.setGnss_SerialPort
        saveConfig: false
        onSaveConfigChanged: {
            AgIOSettings.setGnss_SerialPort = gnssConfig.portName
            AgIOSettings.setGnss_BaudRate = gnssConfig.portBaud
            }
    }
    Comp.SerialTerminalAgio {
        id: imuConfig
        visible: false
        portBaud: AgIOSettings.setImu_BaudRate
        portName: AgIOSettings.setImu_SerialPort
        saveConfig: false
        onSaveConfigChanged: {
            AgIOSettings.setImu_SerialPort = imuConfig.portName
            AgIOSettings.setImu_BaudRate = imuConfig.portBaud
            }
    }
    Comp.SerialTerminalAgio {
        id: autosteerConfig
        visible: false
        portBaud: AgIOSettings.setSteer_BaudRate
        portName: AgIOSettings.setSteer_SerialPort
        saveConfig: false
        onSaveConfigChanged: {
            AgIOSettings.setSteer_SerialPort = steerConfig.portName
            AgIOSettings.setSteer_BaudRate = steerConfig.portBaud
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
                color: AgIOService.bluetoothConnected ? "green" : "red"
                onClicked: {
                    settingsWindow.close()
                    if(!utils.isTrue(AgIOSettings.setBluetooth_isOn)){ //start bt if off
                        AgIOSettings.setBluetooth_isOn = true
                        AgIOService.startBluetoothDiscovery()
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
                color: AgIOService.imuConnected ? "green" : "red"
                onClicked: imuConfig.visible = !imuConfig.visible
            }
            Comp.IconButtonTextBeside {
                //objectName: btnModuleSteer
                isChecked: false
                text: qsTr("Steer")
                icon.source: "../images/Com_AutosteerModule.png"
                color: AgIOService.steerConnected ? "green" : "red"
                onClicked: autosteerConfig.visible = !autosteerConfig.visible
            }
            Comp.IconButtonTextBeside {
                //objectName: btnModuleGPS
                isChecked: false
                text: qsTr("GPS")
                icon.source: "../images/B_GPS.png"
                color: AgIOService.gpsConnected ? "green" : "red"
                onClicked: gnssConfig.visible = !gnssConfig.visible
            }
            Comp.IconButtonTextBeside {
                //objectName: btnModuleMachine
                isChecked: false
                text: qsTr("Machine")
                icon.source: "../images/B_Machine.png"
                color: AgIOService.machineConnected ? "green" : "red"
            }
            Comp.IconButtonTextBeside {
                //objectName: btnModuleBlockage
                isChecked: false
                text: qsTr("Blockage")
                icon.source: "../images/B_Blockage.png"
                color: AgIOService.blockageConnected ? "green" : "red"
                visible: Settings.seed_blockageIsOn
            }
            Comp.IconButtonTextBeside {
                //objectName: btnEthernetStatus
                isChecked: false
                text: qsTr("Ethernet")
                icon.source: "../images/B_UDP.png"
                color: AgIOService.ethernetConnected ? "green" : "red"
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
                text: (AgIOSettings.setNTRIP_isOn === false ? "Off":
                    AgIOService.ntripStatus === 0 ? "Invalid" :
                    AgIOService.ntripStatus === 1 ? "Authorizing" :
                    AgIOService.ntripStatus === 2 ? "Waiting" :
                    AgIOService.ntripStatus === 3 ? "Send GGA" :
                    AgIOService.ntripStatus === 4 ? "Listening NTRIP":
                    AgIOService.ntripStatus === 5 ? "Wait GPS":
                    "Unknown")

                icon.source: "../images/NtripSettings.png"
                color:  (AgIOSettings.setNTRIP_isOn === false ? "red":
                    AgIOService.ntripStatus === 0 ? "red" :
                    AgIOService.ntripStatus === 1 ? "yellow" :
                    AgIOService.ntripStatus === 2 ? "yellow" :
                    AgIOService.ntripStatus === 3 ? "yellow" :
                    AgIOService.ntripStatus === 4 ? "green":
                    AgIOService.ntripStatus === 5 ? "red":
                    "red")
                onClicked: ntrip.visible = !ntrip.visible
            }
            }
        }
    }
