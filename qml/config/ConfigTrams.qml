// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
//
import QtQuick
import QtQuick.Controls.Fusion
import Settings

import ".."
import "../components"

/*todo:
  not sure about this whole file. The IconButtonColor isn't connected to anything
  */
Item {
    anchors.fill: parent
    Rectangle{
        anchors.fill: parent
        color: aog.backgroundColor
        Image {
            source: prefix + "/images/Config/ConT_TramSpacing.png"
            id: imageTram
            anchors.right: parent.horizontalCenter
            anchors.top: parent.top
			anchors.topMargin: 10 * theme.scaleHeight
			anchors.bottomMargin: 10 * theme.scaleHeight
			anchors.leftMargin: 10 * theme.scaleWidth
			anchors.rightMargin: 10 * theme.scaleWidth
            width: 200 * theme.scaleWidth
            height: 200 * theme.scaleHeight
        }
        SpinBoxCM{
            id: tramWidth
            anchors.verticalCenter: imageTram.verticalCenter
            anchors.left: parent.horizontalCenter
			anchors.topMargin: 3 * theme.scaleHeight
			anchors.bottomMargin: 3 * theme.scaleHeight
			anchors.leftMargin: 3 * theme.scaleWidth
			anchors.rightMargin: 3 * theme.scaleWidth
            editable: true
            text: qsTr("Tram Width")
            from: 1
            value: Settings.tram_width
            Connections {
                target: Settings
                function onTram_widthChanged(){
                    tramWidth.value = Settings.tram_width
                }
            }
            to: 5000
            onValueChanged: Settings.tram_width = value
        }
        IconButtonColor{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 100 * theme.scaleHeight
            width: 200 * theme.scaleWidth
            height: 200 * theme.scaleHeight
            icon.source: prefix + "/images/Config/ConT_TramOverride.png"
        }
    }
}
