import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
// 
import "components" as Comp
import "interfaces"

Window {
	title: "GPS Info"
	visible: true
	width: 500
	height: 500
    NTripInterface {
		id: ntrip
	}

	GridLayout {
		id: layout
        anchors.margins: 4
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
        anchors.bottom: sentences.top
        rows: 4
        flow: Grid.TopToBottom

		Comp.Text {
            font.pixelSize: 10
            text: qsTr("Lat: ") + (Number( AgIOService.latitude).toLocaleString(Qt.locale(), 'f', 7))
			Layout.alignment: Qt.AlignLeft
		}

		Comp.Text {
            font.pixelSize: 10
            text: qsTr("Lon: ") + (Number( AgIOService.longitude).toLocaleString(Qt.locale(), 'f', 7))
			Layout.alignment: Qt.AlignLeft
		}

        Comp.Text {
            font.pixelSize: 10
            text: qsTr("Speed KMH: ") + Math.round( AgIOService.speed * 100) / 100
            Layout.alignment: Qt.AlignLeft
        }
        //quality
        Comp.Text {
            font.pixelSize: 10
            text: AgIOService.fixQuality() + AgIOService.gpsQuality
            Layout.alignment: Qt.AlignLeft
        }

        //HDOP
        Comp.Text {
            font.pixelSize: 10
            text: "HDOP: " +  AgIOService.hdop
            Layout.alignment: Qt.AlignLeft
        }
        //# Sats
        Comp.Text {
            font.pixelSize: 10
            text: qsTr("# Sats: ", "Number of satellites") +  AgIOService.sats // note that
            Layout.alignment: Qt.AlignLeft
        }
        Comp.Text {
            font.pixelSize: 10
            text: qsTr("Altitude: ") + (Number( AgIOService.altitude).toLocaleString(Qt.locale(), 'f', 2))
			Layout.alignment: Qt.AlignLeft
		}
        //age
        Comp.Text {
            font.pixelSize: 10
            text: qsTr("Age: ") + (Number( AgIOService.age).toLocaleString(Qt.locale(), 'f', 1))
            Layout.alignment: Qt.AlignLeft
        }
        Comp.Text {
            font.pixelSize: 10
            text: qsTr("VTG: ") + (AgIOService.speed > 1 ? Math.round(AgIOService.heading) : "0")  // VTG = course over ground when moving
            Layout.alignment: Qt.AlignLeft
        }
        Comp.Text {
            font.pixelSize: 10
            text: qsTr("Dual: ") + "0"  // Dual heading = 0 for single antenna (no dual antenna baseline)
            Layout.alignment: Qt.AlignLeft
        }
        Comp.Text {
            font.pixelSize: 10
            text: qsTr("Roll: ") + AgIOService.rollGPS
			Layout.alignment: Qt.AlignLeft
		}
        Column{
            Comp.Text{
                font.pixelSize: 10
                text: qsTr("Hz: ") + Math.round( AgIOService.gpsHz * 100) / 100
                Layout.alignment: Qt.AlignLeft
            }
            Comp.Text{
                font.pixelSize: 10
                text: qsTr("Raw Hz: ") + Math.round( AgIOService.nowHz * 100) / 100
                Layout.alignment: Qt.AlignLeft
            }
        }

		Comp.Text {
            font.pixelSize: 10
            text: qsTr("Yaw: ") + Number( AgIOService.yawrate).toLocaleString(Qt.locale(), 'f', 1)
			Layout.alignment: Qt.AlignLeft
		}
		Comp.Text {
            font.pixelSize: 10
            text: qsTr("Roll: ") + (Number( AgIOService.imuRoll / 10).toLocaleString(Qt.locale(), 'f', 1))
			Layout.alignment: Qt.AlignLeft
		}
		Comp.Text {
            font.pixelSize: 10
            text: qsTr("Pitch: ") + (Number( AgIOService.imuPitch / 10).toLocaleString(Qt.locale(), 'f', 1))
			Layout.alignment: Qt.AlignLeft
		}
	}
    Comp.TitleFrame{
        id: sentences
        title: qsTr("NMEA Sentences")
        height: parent.height *.66 // 2/3 of parent
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        ColumnLayout {
            anchors.fill: parent
            id: strings
            Comp.Text{
                font.pixelSize: 10
                text: "GGA: " +  AgIOService.gga
                Layout.alignment: Qt.AlignLeft
            }
            Comp.Text{
                font.pixelSize: 10
                text: "VTG: " +  AgIOService.vtg
                Layout.alignment: Qt.AlignLeft
            }
            Comp.Text{
                font.pixelSize: 10
                text: "NDA: " +  AgIOService.panda
                Layout.alignment: Qt.AlignLeft
            }
            Comp.Text{
                font.pixelSize: 10
                text: "OGI: " +  AgIOService.paogi
                Layout.alignment: Qt.AlignLeft
            }
            Comp.Text{
                font.pixelSize: 10
                text: "HDT: " +  AgIOService.hdt
                Layout.alignment: Qt.AlignLeft
            }
            Comp.Text{
                font.pixelSize: 10
                text: "AVR: " +  AgIOService.avr
                Layout.alignment: Qt.AlignLeft
            }
            Comp.Text{
                font.pixelSize: 10
                text: "HPD: " +  AgIOService.hpd
                Layout.alignment: Qt.AlignLeft
            }
            Comp.Text{
                font.pixelSize: 10
                text: "SXT: " +  AgIOService.sxt
                Layout.alignment: Qt.AlignLeft
            }
            //Unknown sentences. Ones AOG just ignores. Most likely a wrong reciever config.
            Comp.Text{
                color: "red"
                font.pixelSize: 10
                text: qsTr("Unknown: ") +  AgIOService.unknownSentence
            }
        }
    }
}
