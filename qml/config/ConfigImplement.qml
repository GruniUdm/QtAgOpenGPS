// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Implement type(drawn, front 3pt, etc)
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion
import Settings
//import QtQuick.Controls.Styles 1.4

import ".."
import "../components"

Rectangle{
	anchors.fill: parent
	visible: true
    color: aog.backgroundColor
    TextLine{
		id: text
		anchors.top: parent.top
		anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("<h1>Attachment Style</h1>")
	}
	GridLayout{
		anchors.top: text.bottom
		anchors.horizontalCenter: parent.horizontalCenter
        width: 650 * theme.scaleWidth
        height: 450 * theme.scaleHeight
		rows:2
		columns: 2
        flow:Grid.TopToBottom

        ButtonGroup {
            buttons: [ i3pt, i3ptfront, itrailed, iTBT]
        }

		IconButtonColor{
            implicitWidth:300 * theme.scaleWidth
            implicitHeight:200 * theme.scaleHeight
			id: i3pt
            icon.source: prefix + "/images/ToolChkRear.png"
            checkable: true
            isChecked: Settings.tool_isToolRearFixed
            onClicked: {
                Settings.tool_isToolRearFixed = true
                Settings.tool_isToolFront = false
                Settings.tool_isToolTrailing = false
                Settings.tool_isTBT = false
            }
		}

		IconButtonColor{
            implicitWidth:300 * theme.scaleWidth
            implicitHeight:200 * theme.scaleHeight
			id: i3ptfront
            icon.source: prefix + "/images/ToolChkFront.png"
            checkable: true
            isChecked: Settings.tool_isToolFront
            onClicked: {
                Settings.tool_isToolRearFixed = false
                Settings.tool_isToolFront = true
                Settings.tool_isToolTrailing = false
                Settings.tool_isTBT = false
            }
        }

		IconButtonColor{
            implicitWidth:300 * theme.scaleWidth
            implicitHeight:200 * theme.scaleHeight
			id: itrailed
            icon.source: prefix + "/images/ToolChkTrailing.png"
            checkable: true
            isChecked: Settings.tool_isToolTrailing && ! Settings.tool_isTBT
            onClicked: {
                Settings.tool_isToolRearFixed = false
                Settings.tool_isToolFront = false
                Settings.tool_isToolTrailing = true
                Settings.tool_isTBT = false
            }
        }
		IconButtonColor{
            implicitWidth:300 * theme.scaleWidth
            implicitHeight:200 * theme.scaleHeight
			id: iTBT
            icon.source: prefix + "/images/ToolChkTBT.png"
            checkable: true
            isChecked: Settings.tool_isTBT
            onClicked: {
                Settings.tool_isToolRearFixed = false
                Settings.tool_isToolFront = false
                Settings.tool_isToolTrailing = true
                Settings.tool_isTBT = true
            }
        }
	}
}
