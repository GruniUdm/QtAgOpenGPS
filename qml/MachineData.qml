// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Displays the GPS Data on main screen.
import QtQuick
import QtQuick.Controls.Fusion
import "components" as Comp

Rectangle{
    id: machineData
    width: 200 * theme.scaleWidth
    height: childrenRect.height + 30
    color: "#4d4d4d"
    Column{
        id: column
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 15 * theme.scaleWidth
        Comp.TextLine{ color: "red"; font.pixelSize: 30; text: qsTr("RateSet ")+settings.setArdMac_user4}
        Comp.TextLine{ color: "red"; font.pixelSize: 30; text: qsTr("Rate ")+aog.actualRate}
    }

            Comp.IconButtonColor{
                id: rateUp
                checkable: true
                colorChecked: "lightgray"
                icon.source: prefix + "/images/RateUp.png"
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: 50 * theme.scaleWidth
                anchors.right: parent.right
                anchors.rightMargin: 15 * theme.scaleHeight
                anchors.top: column.bottom
                onClicked: settings.setArdMac_user4>250?settings.setArdMac_user4=255:settings.setArdMac_user4+=10
            }
            Comp.IconButtonTransparent{
                id: rateSettings
                checkable: true
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: 50 * theme.scaleWidth
                anchors.top: column.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                icon.source: prefix + "/images/Settings48.png"

        }
            Comp.IconButtonColor{
                id: rateDown
                checkable: true
                colorChecked: "lightgray"
                icon.source: prefix + "/images/RateDown.png"
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: 50 * theme.scaleWidth
                anchors.left: parent.left
                anchors.leftMargin: 15 * theme.scaleHeight
                anchors.top: column.bottom
                onClicked: settings.setArdMac_user4<10?settings.setArdMac_user4=0:settings.setArdMac_user4-=10
        }

            Row{
                height: 50 * theme.scaleHeight
                width: parent.width
                anchors.top: rateDown.bottom
                anchors.horizontalCenter: parent.horizontalCenter

            Comp.IconButtonTransparent{
                visible: rateSettings.checked
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: 50 * theme.scaleWidth
                anchors.horizontalCenter: parent.horizontalCenter
                icon.source: prefix + "/images/AutoStop.png"
                onClicked: aog.rate_pwm_auto()
        }
            Comp.IconButtonTransparent{
                visible: rateSettings.checked
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: 50 * theme.scaleWidth
                anchors.right: parent.right
                anchors.rightMargin: 15 * theme.scaleHeight
                icon.source: prefix + "/images/UpArrow64.png"
                onClicked: aog.rate_bump_pwm(true)
        }
            Comp.IconButtonTransparent{
                visible: rateSettings.checked
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: 50 * theme.scaleWidth
                anchors.left: parent.left
                anchors.leftMargin: 15 * theme.scaleHeight
                icon.source: prefix + "/images/DnArrow64.png"
                onClicked: aog.rate_bump_pwm(false)
        }
}
}
