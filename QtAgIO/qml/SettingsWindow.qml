import QtQuick
import QtQuick.Controls.Fusion
import AgIO 1.0

Drawer{
    id: settingsWindow
    width: 200
    height: parent.height
    modal: true

    contentItem: Rectangle{
        id: settingsWindowContent
        anchors.fill: parent
        height: settingsWindow.height
        color: theme.blackDayWhiteNight
    }

    ScrollViewExpandableColumn{
        id: toolsColumn
        anchors.fill: parent

        ButtonColor{
            id: advancedOptions
            width: 180
            height: 50
            text: qsTr("Advanced")
            onClicked: {
                settingsWindow.close()
                advancedMenu.showMenu()
            }
        }
        ButtonColor{
            id: bluetooth
            width: 180
            height: 50
            text: qsTr("Bluetooth")
            onClicked: {
                settingsWindow.close()
                if(!utils.isTrue(agiosettings.setBluetooth_isOn)){ //start bt if off
                    agiosettings.setBluetooth_isOn = true
                    agio.startBluetoothDiscovery()
                    console.log("ssb")
                }
                bluetoothMenu.show()
            }
        }
    }
}
