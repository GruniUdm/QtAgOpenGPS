import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
//
import "../components"
import "interfaces"
import AOG



Dialog {
    id: gpsInfo
    visible: false
    height: 520 * theme.scaleHeight
    width: 720 * theme.scaleWidth
    anchors.centerIn: parent
    modal: false
    background: Rectangle {
         color: aogInterface.backgroundColor
     }

    function show() {
        parent.visible = true
    }

    TopLine {
        id: topLine
        titleText: qsTr("Module and GPS Info")
        onBtnCloseClicked: gpsInfo.close()
    }

    NTripInterface {
        id: ntrip
    }

    AgIOInterface {
        id: agioInterface
    }

    ColumnLayout {
        anchors.top: topLine.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 4
        spacing: 4

        // Tab Bar
        TabBar {
            id: tabBar
            Layout.fillWidth: true
            anchors.bottomMargin: 10
            Layout.preferredHeight: 30 * theme.scaleHeight

            TabButton {
                text: qsTr("GPS Info")
                Layout.fillWidth: true
            }
            TabButton {
                text: qsTr("Satellites")
                Layout.fillWidth: true
            }
            TabButton {
                text: qsTr("NMEA")
                Layout.fillWidth: true
            }
        }

        // Tab Content
        StackLayout {
            id: tabContent
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            /// Tab 1: GPS Info
            GridLayout {
                id: layout
                anchors.fill: parent
                anchors.margins: 8
                columns: 4
                rows: 4
                columnSpacing: 12
                rowSpacing: 8

                Text {
                    font.pixelSize: 15
                    text: qsTr("Lon: ") + Number(Backend.fixFrame.longitude).toLocaleString(Qt.locale(), 'f', 7)
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: "HDOP: " + Backend.fixFrame.hdop
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("VTG: ") + Number(Backend.fixFrame.vtgHeading).toLocaleString(Qt.locale(), 'f', 1)
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("Yaw: ") + (Backend.fixFrame.imuHeading > 360 ? "-" : Number(Backend.fixFrame.imuHeading).toLocaleString(Qt.locale(), 'f', 1))
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("Lat: ") + Number(Backend.fixFrame.latitude).toLocaleString(Qt.locale(), 'f', 7)
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("# Sats: ") + AgIOService.satellites
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("Dual: ") + Number(Backend.fixFrame.dualHeading).toLocaleString(Qt.locale(), 'f', 1)
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("Roll: ") + Number(Backend.fixFrame.imuRoll).toLocaleString(Qt.locale(), 'f', 1)
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("Speed KMH: ") + Math.round(Backend.fixFrame.speedKph * 100) / 100
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("Altitude: ") + Number(Backend.fixFrame.altitude).toLocaleString(Qt.locale(), 'f', 2)
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("Roll: ") + Number(Backend.fixFrame.imuRoll).toLocaleString(Qt.locale(), 'f', 1)
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("Pitch: ") + Number(Backend.fixFrame.imuPitch).toLocaleString(Qt.locale(), 'f', 1)
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: {
                        switch(AgIOService.gpsQuality) {
                            case 0: return qsTr("GPS Quality: Invalid");
                            case 1: return qsTr("GPS Quality: GPS Single");
                            case 2: return qsTr("GPS Quality: DGPS");
                            case 3: return qsTr("GPS Quality: PPS");
                            case 4: return qsTr("GPS Quality: RTK Fix");
                            case 5: return qsTr("GPS Quality: RTK Float");
                            case 6: return qsTr("GPS Quality: Estimate");
                            case 7: return qsTr("GPS Quality: Man IP");
                            case 8: return qsTr("GPS Quality: Sim");
                            default: return qsTr("GPS Quality: ") + AgIOService.gpsQuality;
                        }
                    }
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("Age: ") + Number(Backend.fixFrame.age).toLocaleString(Qt.locale(), 'f', 1)
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("Hz: ") + Math.round(Backend.fixFrame.hz * 100) / 100
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
                Text {
                    font.pixelSize: 15
                    text: qsTr("Yaw Rate: ") + Number(Backend.fixFrame.yawRate).toLocaleString(Qt.locale(), 'f', 1)
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                }
            }

            // Tab 2: Satellites RSSI
            Rectangle {
                id: satDiagram
                color: theme.backgroundColor
                border.color: theme.borderColor
                border.width: 1
                Layout.fillWidth: true
                Layout.fillHeight: true

                Column {
                    anchors.fill: parent
                    anchors.margins: 4
                    spacing: 2

                    RowLayout {
                        width: parent.width
                        spacing: 4

                        Text {
                            font.pixelSize: 10
                            font.bold: true
                            color: theme.textColor
                            text: qsTr("Satellites: ") + AgIOService.satelliteModel.size
                        }

                        Text {
                            font.pixelSize: 10
                            color: "#489d22"
                            text: qsTr("In Use: ") + AgIOService.satelliteModel.inUseCount
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        Row {
                            spacing: 12

                            Rectangle {
                                width: 12
                                height: 12
                                radius: 2
                                color: "#489d22"
                            }
                            Text {
                                font.pixelSize: 9
                                color: theme.textColor
                                text: qsTr("In Use")
                            }

                            Rectangle {
                                width: 12
                                height: 12
                                radius: 2
                                color: "#f9a70e"
                            }
                            Text {
                                font.pixelSize: 9
                                color: theme.textColor
                                text: qsTr("In View")
                            }
                        }
                    }

                    // Bars and Scale Row (без изменений)
                    Row {
                        width: parent.width
                        height: parent.height - 24
                        spacing: 8

                        Rectangle {
                            id: barsArea
                            width: parent.width - 60
                            height: parent.height
                            color: "transparent"

                            Row {
                                anchors.fill: parent
                                anchors.leftMargin: 2
                                anchors.rightMargin: 2
                                spacing: 2

                                Repeater {
                                    id: repeater
                                    model: AgIOService.satelliteModel
                                    delegate: Item {
                                        property var satData: model
                                        readonly property int barMaxHeight: barsArea.height - 40

                                        width: barsArea.width / Math.max(AgIOService.satelliteModel.size, 1)
                                        height: parent.height

                                        Rectangle {
                                            anchors.horizontalCenter: parent.horizontalCenter
                                            anchors.bottom: rssiValue.top
                                            anchors.bottomMargin: 2
                                            width: parent.width - 4
                                            height: {
                                                if (model.rssi < 0) return 2;
                                                var h = barMaxHeight * Math.min(model.rssi, 70) / 70;
                                                return Math.max(h, 2);
                                            }
                                            color: {
                                                if (model.inUse) return "#489d22"
                                                else if (model.rssi >= 10) return "#f9a70e"
                                                else return "#c50000"
                                            }
                                            radius: 2
                                        }

                                        Text {
                                            id: rssiValue
                                            anchors.horizontalCenter: parent.horizontalCenter
                                            anchors.bottom: satId.top
                                            anchors.bottomMargin: 1
                                            font.pixelSize: 8
                                            color: theme.textColor
                                            text: model.rssi >= 0 ? model.rssi : "N/A"
                                        }

                                        Text {
                                            id: satId
                                            anchors.horizontalCenter: parent.horizontalCenter
                                            anchors.bottom: parent.bottom
                                            anchors.bottomMargin: 2
                                            font.pixelSize: 9
                                            font.bold: true
                                            color: model.inUse ? "#489d22" : theme.textColor
                                            text: model.id
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Tab 3: NMEA Sentences
            TitleFrame {
                id: nmeaFrame
                title: qsTr("NMEA Sentences")
                Layout.fillWidth: true
                Layout.fillHeight: true

                ScrollView {
                    anchors.fill: parent
                    clip: true

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.bottomMargin: 0
                        id: strings

                        Text {
                            font.pixelSize: 15
                            text: "GGA: " + AgIOService.gga
                            Layout.alignment: Qt.AlignLeft
                        }
                        Text {
                            font.pixelSize: 15
                            text: "VTG: " + AgIOService.vtg
                            Layout.alignment: Qt.AlignLeft
                        }
                        Text {
                            font.pixelSize: 15
                            text: "RMC: " + AgIOService.rmc
                            Layout.alignment: Qt.AlignLeft
                        }
                        Text {
                            font.pixelSize: 15
                            text: "NDA: " + AgIOService.panda
                            Layout.alignment: Qt.AlignLeft
                        }
                        Text {
                            font.pixelSize: 15
                            text: "OGI: " + AgIOService.paogi
                            Layout.alignment: Qt.AlignLeft
                        }
                        Text {
                            font.pixelSize: 15
                            text: "HDT: " + AgIOService.hdt
                            Layout.alignment: Qt.AlignLeft
                        }
                        Text {
                            font.pixelSize: 15
                            text: "AVR: " + AgIOService.avr
                            Layout.alignment: Qt.AlignLeft
                        }
                        Text {
                            font.pixelSize: 15
                            text: "HPD: " + AgIOService.hpd
                            Layout.alignment: Qt.AlignLeft
                        }
                        Text {
                            font.pixelSize: 15
                            text: "SXT: " + AgIOService.sxt
                            Layout.alignment: Qt.AlignLeft
                        }
                        Text {
                            color: "red"
                            font.pixelSize: 15
                            text: qsTr("Unknown: ") + AgIOService.unknownSentence
                            Layout.alignment: Qt.AlignLeft
                        }
                    }
                }
            }
        }
    }
}
