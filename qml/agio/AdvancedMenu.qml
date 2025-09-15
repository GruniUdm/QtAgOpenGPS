import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
//import AOG

import "components" as Comp
import "interfaces"

Drawer {
    id: advancedMenu
    width: 270 * theme.scaleWidth
    height: mainWindow.height
    visible: false
    modal: true
    function show(){
        parent.visible = true
    }

    // Comp.TopLine{
    //     id: topLine
    //     titleText: qsTr("Advanced settings")
    //     onBtnCloseClicked:  advancedMenu.close()
    // }


    Rectangle{
        id: content
        visible: true
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        border.width: 2
        border.color: theme.blackDayWhiteNight
        color: theme.backgroundColor
        ColumnLayout{
            id: contGrid
            anchors.fill: parent
            anchors.margins: 10
            //flow: Grid.TopToBottom
            //columns: 2
            Comp.CheckBoxCustomized{
                id: ckUDPListenOnly
                text: "UDP Listen Only"
                checked: false
                onCheckedChanged:  AgIOService.btnUDPListenOnly_clicked(ckUDPListenOnly.checkState)
                //Layout.alignment: Qt.AlignLeft
            }
            Comp.CheckBoxCustomized {
                id: ckNtripDebug
                text: "Console NTRIP Debug"
                checked: false
                onCheckedChanged:  AgIOService.ntripDebug(ckNtripDebug.checkState)
                //Layout.alignment: Qt.AlignRight
            }
            Comp.CheckBoxCustomized {
                id: ckBluetoothDebug
                text: "Console Bluetooth Debug"
                checked: false
                onCheckedChanged:  AgIOService.bluetoothDebug(ckBluetoothDebug.checkState)
                //Layout.alignment: Qt.AlignLeft
            }
        }
    }
}

