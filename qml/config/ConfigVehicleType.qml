// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Tractor/4wd/Harvester
import QtQuick
import QtQuick.Controls.Fusion
import Settings

import ".."
import "../components"

Rectangle{
    id: configTractor
    anchors.fill: parent
    visible: true
    color: aog.backgroundColor
    Text {
        id: text
        text: qsTr("<h1>Choose Vehicle Type</h1>")
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    ButtonGroup {
        buttons:  typesButtons.children
    }

    Row{
        id: typesButtons
        anchors.top: text.bottom
        anchors.horizontalCenter: parent.horizontalCenter


        IconButtonColor{
            id: harvester
            width:150  * theme.scaleWidth
            height:100 * theme.scaleHeight
            checkable: true
            icon.source: prefix + "/images/vehiclePageHarvester.png"
            isChecked: Settings.vehicle_vehicleType === 1
            onClicked: Settings.vehicle_vehicleType = 1
        }
        IconButtonColor{
            width:150 * theme.scaleWidth
            height:100 * theme.scaleHeight
            id: tractor
            icon.source: prefix + "/images/vehiclePageTractor.png"
            checkable: true
            isChecked: Settings.vehicle_vehicleType === 0
            onClicked: Settings.vehicle_vehicleType = 0
        }
        IconButtonColor{
            width:150 * theme.scaleWidth
            height:100 * theme.scaleHeight
            id: tr4wd
            icon.source: prefix + "/images/vehiclePage4WD.png"
            checkable: true
            isChecked: Settings.vehicle_vehicleType === 2
            onClicked: Settings.vehicle_vehicleType = 2
        }
    }
	IconButtonColor{//triangle, no vehicle image
		anchors.top: typesButtons.bottom
		anchors.horizontalCenter: typesButtons.horizontalCenter
		anchors.topMargin: 20
		width:150 * theme.scaleWidth
		height:100 * theme.scaleHeight
		icon.source: prefix + "/images/Config/Brand/TriangleVehicle.png"
		checkable: true
		isChecked: !Settings.display_isVehicleImage
		onCheckedChanged: {
			Settings.display_isVehicleImage = !checked
			console.log(Settings.display_isVehicleImage)
		}
	}
}
