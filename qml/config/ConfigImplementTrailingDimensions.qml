// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Set the dimensions for a drawn implement
import QtQuick
import QtQuick.Controls.Fusion
import Settings
import AOG


import ".."
import "../components"

Rectangle{
    id: configImpDimWin
    anchors.fill: parent
    color: aog.backgroundColor
    visible: false
    Image{
        id: image1
        source: prefix + "/images/ToolHitchPageTrailing.png"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
		anchors.topMargin: 15 * theme.scaleHeight
		anchors.bottomMargin: 15 * theme.scaleHeight
		anchors.leftMargin: 15 * theme.scaleWidth
		anchors.rightMargin: 15 * theme.scaleWidth
        height: parent.height*.75
    }
    SpinBoxCM{
        id: toolTrailingHitchLength
        anchors.top: image1.bottom
        anchors.right: parent.right
        anchors.rightMargin: 400 * theme.scaleWidth
        from: 10
        to:3000
        boundValue: -Settings.tool_toolTrailingHitchLength
        onValueModified: Settings.tool_toolTrailingHitchLength = -value
        TextLine{
            text: qsTr("Units: ")+ Utils.cm_unit_abbrev()
            font.bold: true
            anchors.top: parent.bottom
        }
    }
}
