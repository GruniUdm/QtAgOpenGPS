// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Drop down box
import QtQuick
import QtQuick.Controls.Fusion

Item{
    id: itemCombo
    property string text: ""
    property alias model: rootCombo.model
    property alias editable: rootCombo.editable
    property alias currentIndex: rootCombo.currentIndex
    property alias currentText: rootCombo.currentText

	signal activated()
    implicitHeight: 40 * theme.scaleHeight
    width: rootCombo.width
    TextLine{
        anchors.bottom: rootCombo.top
        anchors.left: rootCombo.left
        text: itemCombo.text
        font.bold: true
    }
    ComboBox{
        id: rootCombo
        editable: true
        implicitHeight: 40 * theme.scaleHeight
        implicitWidth: 150 * theme.scaleWidth
        anchors.bottom: parent.bottom

        model: ListModel{
            id: rootComboModel
        }
		onActivated: itemCombo.activated()
        onAccepted: {
            if (rootCombo.find(currentText) === -1){
                rootComboModel.append({text: editText})
                currentIndex = rootCombo.find(editText)
            }
        }
    }
}
