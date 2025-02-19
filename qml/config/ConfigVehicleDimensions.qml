// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Vehicle dimensions
import QtQuick
import QtQuick.Controls.Fusion
import Settings

import ".."
import "../components"
/*todo:
  switch to something with configImplementDimensions
  */

Rectangle{
    id: configTractorDimensions
    anchors.fill: parent
    color: aog.backgroundColor

    visible: false
    Image {
        id: dimImage
        source: Number(Settings.vehicle_vehicleType) === 0 ? prefix + "/images/RadiusWheelBase.png":
                Number(Settings.vehicle_vehicleType) === 1 ? prefix + "/images/RadiusWheelBaseHarvester.png" :
                Number(Settings.vehicle_vehicleType) === 2 ? prefix + "/images/RadiusWheelBase4WD.png":
                prefix + "/images/Config/ConSt_Mandatory.png"
        anchors.fill: parent
        anchors.margins: 15
    }
    SpinBoxCM{
        id: wheelbase
        anchors.bottom: dimImage.bottom
        anchors.right: dimImage.right
        anchors.rightMargin: dimImage.width * .65
        anchors.bottomMargin: dimImage.height *.15
        from: 20
        to: 787
        boundValue: Settings.vehicle_wheelbase
        onValueModified: Settings.vehicle_wheelbase = value
        text: qsTr("Wheelbase")
    }
    SpinBoxCM{
        id: trackWidth
        anchors.top: dimImage.top
        anchors.topMargin: dimImage.height *.25
        anchors.right: dimImage.right
        from: 50
        to: 9999
        boundValue: Settings.vehicle_trackWidth
        onValueModified: Settings.vehicle_trackWidth = value
        text: qsTr("Track")
    }
    SpinBoxCM{
        id: minTurningRadius
        anchors.bottom: dimImage.bottom
        anchors.bottomMargin: dimImage.height *.18
        anchors.right: dimImage.right
        from: 50
        to: 9999
        boundValue: Settings.vehicle_minTurningRadius
        onValueModified: Settings.vehicle_minTurningRadius = value
        text: qsTr("Turn Radius")
    }
    SpinBoxCM{
        id: hitchLength
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: dimImage.left
        from: 10
        to:3000
        boundValue: Settings.vehicle_hitchLength < 0 ? -Settings.vehicle_hitchLength : Settings.vehicle_hitchLength
        onValueModified: Settings.vehicle_hitchLength = -value
        visible: Settings.tool_isToolTrailing
    }
}
