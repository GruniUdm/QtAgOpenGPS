// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Config dimensions for rear 3pt mounted implement 
import QtQuick
import QtQuick.Controls.Fusion
import Settings

import ".."
import "../components"

Rectangle{
    id: configImpDimWin
    anchors.fill: parent
    color: aog.backgroundColor
    visible: false
    Image{
        id: image1
        source: prefix + "/images/ToolHitchPageRear.png"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 15
		anchors.topMargin: 7 * theme.scaleHeight
		anchors.leftMargin: 7 * theme.scaleWidth
		anchors.rightMargin: 7 * theme.scaleWidth
		anchors.bottomMargin: 7 * theme.scaleHeight
        height: parent.height*.75
    }
    SpinBoxCM{
        id: hitchLength
        anchors.top: image1.bottom
        anchors.right: parent.right
        anchors.rightMargin: parent.width * 0.6
        from: 10
        to:3000
        boundValue: Settings.vehicle_hitchLength < 0 ? -Settings.vehicle_hitchLength : Settings.vehicle_hitchLength
        onValueModified: Settings.vehicle_hitchLength = -value
        TextLine{
            text: qsTr("Units: ")+ utils.cm_unit_abbrev()
            font.bold: true
            anchors.top: parent.bottom
        }
    }
}
