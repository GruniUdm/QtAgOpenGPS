import QtQuick
import QtQuick.Controls.Fusion

import "components" as Comp
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

    Comp.ScrollViewExpandableColumn{
        id: toolsColumn
        anchors.fill: parent

        Comp.ButtonColor{
            id: advancedOptions
            width: 180
            height: 50
            text: qsTr("Advanced")
            onClicked: {
                settingsWindow.close()
                advancedMenu.showMenu()
            }
        }
        Comp.ButtonColor{
            id: bluetooth
            width: 180
            height: 50
            text: qsTr("Bluetooth")
            onClicked: {
                settingsWindow.close()
                if(!utils.isTrue(settings.setBluetooth_isOn)){ //start bt if off
                    settings.setBluetooth_isOn = true
                }
                bluetoothMenu.show()
            }
        }
    }
}
