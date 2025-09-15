import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Window
import QtQuick.Layouts
import QtQml.Models
//import AOG

import "components" as Comp

Drawer {
    id: bluetoothMenu
    width: 270 * theme.scaleWidth
    height: mainWindow.height
    visible: false
    modal: true



    Comp.TitleFrame{
        id: devicesTitleFrame
        title: qsTr("Connect to Device:")
        height: mainWindow.height*0.7
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        border.width: 2
        ListView{
            id: bluetoothDevices
            anchors.fill: parent

            // Custom ListModel to store Bluetooth devices
            model: ListModel {
                id: deviceListModel
            }
            delegate: RadioButton{
                width: bluetoothDevices.width
                id: control

                indicator: Rectangle{
                    anchors.fill: parent
                    color: control.down ? "blue" : devicesTitleFrame.color
                    visible: true
                    anchors.margins: 5
                    border.color: "black"
                    border.width: 1
                    radius: 3
                    Text{
                        text: model.name
                        anchors.centerIn: parent
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked:  AgIOService.bt_search(model.name)
                    }
                }
            }

            // Connections {
            //     target: bluetoothDeviceList
            // 
            //     // When the backend model changes, update the ListModel
            //     function onModelChanged() {
            //         bluetoothDevices.update_list()
            //     }
            // }
            function update_list(){
                // Clear the QML model
                deviceListModel.clear()

                // TODO: Populate with AgIOService.setBluetooth_deviceList data
                // for (let i = 0; i < AgIOService.setBluetooth_deviceList.length; ++i) {
                //     let name = AgIOService.setBluetooth_deviceList[i];
                //     deviceListModel.append({"name": name});
                // }

            }

            // Initial data load when the component is created
            Component.onCompleted: {
                update_list()
            }

        }

    }
    Comp.TitleFrame{
        id: knownTitleFrame
        title: qsTr("Known Devices (Click to remove)")
        anchors.left: parent.left
        anchors.top: devicesTitleFrame.bottom
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 10
        border.width: 2
        ListView{
            property var deviceList: AgIOService.setBluetooth_deviceList
            id: knownDevicesList
            anchors.fill: parent
            Connections {
                target: AgIOService
                function onSetBluetooth_deviceListChanged() {
                    var rawList = AgIOService.setBluetooth_deviceList;
                    knownDevicesList.model = Array.isArray(rawList) ? rawList : [rawList];
                    console.log("modelchanged")
                }
            }
            Component.onCompleted: {
                var rawList = AgIOService.setBluetooth_deviceList;
                knownDevicesList.model = Array.isArray(rawList) ? rawList : [rawList];
            }

            model: deviceList

            delegate: RadioButton{
                width: knownDevicesList.width
                id: knownControl

                indicator: Rectangle{
                    anchors.fill: parent
                    color: knownControl.down ? "blue" : devicesTitleFrame.color
                    visible: true
                    anchors.margins: 5
                    border.color: "black"
                    border.width: 1
                    radius: 3
                    Text{
                        text: modelData
                        anchors.centerIn: parent
                    }
                    MouseArea{
                        anchors.fill: parent
                        onClicked:  AgIOService.bt_remove_device(modelData)
                    }
                }
            }
        }
    }
}
