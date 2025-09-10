import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Controls.Fusion
import QtQuick.Layouts
////import AgIO

import ".."
import "../components"
Drawer{

    width: 270 * theme.scaleWidth
    height: mainWindow.height
    modal: true
    id: serialTerminalAgio

    property string portBaud: ""
    property string portName: ""
    property bool saveConfig: false

    onVisibleChanged: {
        if (visible) load_settings()
    }


    function load_settings() {

        if (portBaud === "9600") baudRate.currentIndex = 0
        else if (portBaud === "38400") baudRate.currentIndex = 1
        else if (portBaud === "115200") baudRate.currentIndex = 2
        else if (portBaud === "460800") baudRate.currentIndex = 3
        else if (portBaud === "921600") baudRate.currentIndex = 4
    }

    function save_settings() {
    portBaud = baudRate.currentText
    portName = serialPorts.currentText
    }

    Rectangle {
        id: gnssWindow
        visible: true
        color: theme.backgroundColor
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        border.width: 2
        border.color: theme.blackDayWhiteNight

        Connections {

            target: serialTerminal

            onGetData: {
                terminal.append("<"+data);
            }
        }

        // Row {
        //     spacing: 40

        //     Column {
        //         y: 40

        //         Flickable {
        //             clip: true
        //             width: Screen.width*0.8
        //             height: Screen.height*0.6

        //         TextArea.flickable: TextArea {

        //             id: terminal
        //             wrapMode: TextArea.Wrap
        //             placeholderText:  "Waiting..."
        //             width: Screen.width*0.8
        //             height: Screen.height*0.1
        //         }
        //         }

        //         TextField {
        //             id: dataToSend
        //             placeholderText:  "Data to send..."
        //             width: Screen.width*0.8
        //         }

        //         Button {
        //             id: sendBtn
        //             text: qsTr("Send")
        //             anchors.top: connectBtn.anchors.top
        //             onClicked: {
        //                 //terminal.append(">"+dataToSend.text)
        //                 //serialTerminal.writeToSerialPortSlot(dataToSend.text+"\r\n")
        //             }
        //         }
        //     }

        ColumnLayout {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10 * theme.scaleHeight
            spacing: 5

                    Flickable {
                        clip: true
                        width: 260 * theme.scaleWidth
                        height: 170 * theme.scaleHeight

                    TextArea.flickable: TextArea {

                        id: terminal
                        wrapMode: TextArea.Wrap
                        placeholderText:  "Waiting..."
                        width: 260 * theme.scaleWidth
                        height: 170 * theme.scaleHeight
                    }
                    }

                Text {
                    text: qsTr("Serial port: " ) + portName
                    Layout.alignment: Qt.AlignHCenter
                    font.pixelSize: 18
                }

                ComboBox {
                    id: serialPorts
                    Layout.maximumWidth: 200 * theme.scaleWidth
                    Layout.fillWidth : true
                    Layout.alignment: Qt.AlignHCenter
                    model: portsNameModel
                    //model: ["ttyHSL0", "ttyHSL1", "ttyHSL2", "ttyHSL3", "ttyHSL4", "ttyUSB0", "ttyUSB1"]
                }

                Text {
                    text: qsTr("Baud: ")
                    Layout.alignment: Qt.AlignHCenter
                    font.pixelSize: 18
                }

                ComboBox {
                    id: baudRate
                    Layout.maximumWidth: 200 * theme.scaleWidth
                    Layout.fillWidth : true
                    Layout.alignment: Qt.AlignHCenter
                    //model: baudsModel
                    model: ["9600", "38400", "115200", "460800", "921600"]
                }

                Button {
                    id: connectBtn
                    text: qsTr("Connect")
                    Layout.alignment: Qt.AlignHCenter
                    onClicked: {
                            save_settings();
                            saveConfig = true;
                            // if (serialTerminal.getConnectionStatusSlot() === false){
                            //     //serialTerminal.openSerialPortSlot("ttyHSL2","921600")
                            //     serialTerminal.openSerialPortSlot(serialPorts.currentText,baudRate.currentText)
                            //     connectBtn.text = "Disconnect"
                            // }else {

                            //     serialTerminal.closeSerialPortSlot();
                            //     connectBtn.text = "Connect"
                            // }
                        }
                }
            }
        }
    }

