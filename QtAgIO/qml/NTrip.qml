import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import AgIO

import "components" as Comp
Drawer{

    width: 270 * theme.scaleWidth
    height: mainWindow.height
    modal: true
    id: ntrip

    //anchors.centerIn: parent
    visible: false
    // function show(){
    //     parent.visible = true
    // }

    // Comp.TopLine{
    //     id: topLine
    //     titleText: qsTr("NTRIP RTK settings")
    //     onBtnCloseClicked:  ntrip.close()
    // }

    Rectangle {
        id: source
        visible: true
        color: theme.backgroundColor
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: bottomButtons.top
        border.width: 2
        border.color: theme.blackDayWhiteNight

        /*Comp.TextLine {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: 100
            anchors.leftMargin: 100
            text: "Ntrip Stat: " +
                     (agio.ntripStatus === 0 ? "Invalid" :
                     agio.ntripStatus === 1 ? "Authorizing" :
                     agio.ntripStatus === 2 ? "Waiting" :
                     agio.ntripStatus === 3 ? "Send GGA" :
                     agio.ntripStatus === 4 ? "Listening NTRIP":
                     agio.ntripStatus === 5 ? "Wait GPS":
                     "Unknown")
        }*/

        //TODO: all the ntrip diagnostics stuff. I'm too lazy to do it now.
        /*Comp.ScrollableTextArea{
            id: rawTripTxt
            property int rawTripCount: agio.rawTripCount
            property double blah: 0
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: leftColumn.top
            anchors.right: rightColumn.left
            anchors.margins: 40
            onRawTripCountChanged: {
                rawTripTxt.append(rawTripCount)
            }
        }*/

		ColumnLayout {
			id: leftColumn
            anchors.top: parent.top
			anchors.left: parent.left
            width: parent.width
			anchors.bottom: parent.bottom
            spacing: 5
            anchors.topMargin: 15 * theme.scaleHeight
            anchors.bottomMargin: 15 * theme.scaleHeight
            Comp.Text {
				text: qsTr("Enter Broadcaster URL or IP")
                Layout.alignment: Qt.AlignHCenter
                //font.bold: true
                font.pixelSize: 18
                //anchors.bottom: ntripURL.top

			}
			TextField {
				id: ntripURL
                text: agiosettings.setNTRIP_url
                Layout.maximumWidth: 200 * theme.scaleWidth
                Layout.fillWidth : true
                height: 49
				selectByMouse: true
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Qt.AlignHCenter
				onTextChanged: 
                    if (text.length > 0) {
                        agiosettings.setNTRIP_url = text
                    }

			}
            Comp.ButtonColor {
				text: qsTr("Verify")
                onClicked: agio.setIPFromUrl(ntripURL.text)
                width: parent.width * .9
                Layout.alignment: Qt.AlignHCenter
            }

            Comp.Text {
                text: "IP: " + agiosettings.setNTRIP_ipAddress
                Layout.alignment: Qt.AlignHCenter
                //font.bold: true
                font.pixelSize: 18
			}


            Comp.Text {
				text: qsTr("Username")
                Layout.alignment: Qt.AlignHCenter
                //font.bold: true
                font.pixelSize: 18
                //anchors.bottom: ntripUser.top
			}
			TextField {
				id: ntripUser
                text: agiosettings.setNTRIP_userName
                Layout.maximumWidth: 200 * theme.scaleWidth
                Layout.fillWidth : true
				selectByMouse: true
                onTextChanged: agiosettings.setNTRIP_userName = text
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Qt.AlignHCenter
			}

            //Comp.Spacer{}

            Comp.Text {
				text: qsTr("Password")
                Layout.alignment: Qt.AlignHCenter
                //font.bold: true
                font.pixelSize: 18
                //anchors.bottom: ntripPass.top
			}
			TextField {
				id: ntripPass
                text: agiosettings.setNTRIP_userPassword
                onTextChanged: agiosettings.setNTRIP_userPassword = text
                Layout.maximumWidth: 200 * theme.scaleWidth
                Layout.fillWidth : true
                selectByMouse: true
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Qt.AlignHCenter
			}
            //Comp.Spacer{}
   //          Comp.ButtonColor {
            // 	text: qsTr("Get Source Table")
   //              width: parent.width * .9
            // }
            Comp.Text {
				text: qsTr("Mount:", "Ntrip Mountpoint")
                Layout.alignment: Qt.AlignHCenter
                //font.bold: true
                font.pixelSize: 18
                //anchors.bottom: ntripMount.top

			}
			TextField {
				id: ntripMount
                text: agiosettings.setNTRIP_mount
                onTextChanged: agiosettings.setNTRIP_mount = text
                Layout.maximumWidth: 200 * theme.scaleWidth
                Layout.fillWidth : true
				selectByMouse: true
                Layout.alignment: Qt.AlignHCenter
                horizontalAlignment: Qt.AlignHCenter
			}
            Comp.Spacer{}
            Comp.SpinBoxCustomized { //ntrip caster port number
                value: agiosettings.setNTRIP_casterPort
                onValueChanged: agiosettings.setNTRIP_casterPort = value
				from: 0
				to: 65535
                text: qsTr("Caster Port:")
                Layout.alignment: Qt.AlignHCenter
			}
            Comp.Text {
                text: qsTr("Default: 2101")
                color: "red"
                visible: agiosettings.setNTRIP_casterPort !== 2101
                Layout.alignment: Qt.AlignHCenter
                //font.bold: true
                font.pixelSize: 18
            }
        }
    }
	Row {
		height: 50* theme.scaleHeight
		id: bottomButtons
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		anchors.bottomMargin: 5 * theme.scaleHeight
		anchors.rightMargin: 5 * theme.scaleWidth
        spacing: 25 * theme.scaleWidth
        Comp.ButtonColor {
			id: ntripOn
            property bool statusChanged: false
			text: qsTr("NTRIP On")
			height: parent.height
            width: height * 3.5
            isChecked: utils.isTrue(agiosettings.setNTRIP_isOn)
            onClicked: {
                agiosettings.setNTRIP_isOn = !utils.isTrue(agiosettings.setNTRIP_isOn)
                ntripOn.statusChanged = true;
            }
		}
        Comp.IconButtonTransparent {
			id: cancel
            visible: false //not sure if we even want/need this
			height: parent.height
			width: height
            icon.source: "../images/Cancel64.png"
            onClicked: ntrip.visible = false
		}
        Comp.IconButtonTransparent {
			id: btnSave
			height: parent.height
            width: height
            icon.source: "../images/OK64.png"
            onClicked: {
                agio.configureNTRIP()
                ntrip.close()
                //restart if anything changed
                if(ntripOn.statusChanged)
                    message.addMessage("", "Restart of AgIO is Required - Restarting", true)
            }
        }
	}
}

