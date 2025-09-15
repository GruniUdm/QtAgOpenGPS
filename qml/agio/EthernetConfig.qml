import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
// 
import "components" as Comp

Drawer{

    width: 270 * theme.scaleWidth
    height: mainWindow.height
    modal: true
    id: ethernetConfig

    visible: false
    onVisibleChanged: {
        load_settings()
    }

    function load_settings(){
        spIP1.value = AgIOService.setUDP_IP1
        spIP2.value = AgIOService.setUDP_IP2
        spIP3.value = AgIOService.setUDP_IP3
    }

    Rectangle {
        id: ethIP
        visible: true
        color: theme.backgroundColor
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        border.width: 2
        border.color: theme.blackDayWhiteNight

        ColumnLayout {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: bottomButtons.top
            anchors.bottomMargin: 10 * theme.scaleHeight
            anchors.leftMargin: 5 * theme.scaleWidth
            anchors.rightMargin: 5 * theme.scaleWidth

            Comp.Text {
                Layout.alignment: Qt.AlignCenter
                Layout.columnSpan: 3
                text: qsTr("Ethernet IP")
            }
            Comp.Text {
                Layout.alignment: Qt.AlignCenter
                Layout.columnSpan: 3
                text: qsTr("IP Address")
            }

            Comp.SpinBoxCustomized{
                id: spIP1
                width: 30 * theme.scaleWidth
                from: 0
                to: 255
                Layout.alignment: Qt.AlignCenter
            }
            Comp.SpinBoxCustomized{
                id: spIP2
                width: 30 * theme.scaleWidth
                from: 0
                to: 255
                Layout.alignment: Qt.AlignCenter
            }
            Comp.SpinBoxCustomized{
                id: spIP3
                width: 30 * theme.scaleWidth
                from: 0
                to: 255
                Layout.alignment: Qt.AlignCenter
            }
        }


        Comp.Spacer{}

        RowLayout {
            id: bottomButtons
            width: column.width
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10
            Comp.IconButtonText {
                id: ethIPSet
                text: qsTr("IP Set")
                icon.source: "../images/SubnetSend.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: {
                    AgIOService.setUDP_IP1 = spIP1.value
                    AgIOService.setUDP_IP2 = spIP2.value
                    AgIOService.setUDP_IP3 = spIP3.value
                    
                    console.log("🌐 Configuring subnet:", AgIOService.setUDP_IP1 + "." + AgIOService.setUDP_IP2 + "." + AgIOService.setUDP_IP3 + ".x")
                    
                    // Direct call to AgIOService (replaces old  AgIOService.btnSendSubnet_clicked)
                    AgIOService.configureSubnet()
                    
                    timedMessage.addMessage(2000, "IP Address Change", ("IP address changed to " +
                                                                        AgIOService.setUDP_IP1 + "." + AgIOService.setUDP_IP2 + "." + AgIOService.setUDP_IP3 + "!"))
                }
                implicitWidth: btnOK.width
                implicitHeight: btnOK.height
            }
            Comp.IconButtonTransparent {
                id: btnOK
                Layout.alignment: Qt.AlignCenter
                icon.source: "../images/OK64.png"
                onClicked: ethernetConfig.visible = false
            }
        }
    }
}
