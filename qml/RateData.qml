// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Displays the GPS Data on main screen.
import QtQuick
import QtQuick.Controls.Fusion
import "components" as Comp
import Settings
import AOG
Rectangle{
    id: rcData
    width: 200 * theme.scaleWidth
    height: childrenRect.height + 30
    color: "#4d4d4d"
    property double errRate: aog.actualRate;
    Column{
        id: column
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 15 * theme.scaleWidth
        Comp.TextLine{color:(aog.product0stat)?"green":"red"; font.pixelSize: 30; text: qsTr("Target ")+Settings.rate_Product0[10]}
        Comp.TextLine{color:((aog.actualRate < Settings.rate_Product0[10]*0.9)||(aog.actualRate > Settings.rate_Product0[10] * 1.1))?"red":"green"; font.pixelSize: 30; text: qsTr("Instant ")+Math.round(aog.actualRate, 0)}
        Comp.TextLine{ color:((aog.actualRate < Settings.rate_Product0[10]*0.9)||(aog.actualRate > Settings.rate_Product0[10] * 1.1))?"red":"green"; font.pixelSize: 30; text: qsTr("Current ")+Math.round(aog.smoothRate, 0); onTextChanged: {errormessage()}}
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
                onClicked: Settings.rate_Product0[10]>500?Settings.rate_Product0[10]=500:Settings.rate_Product0[10]+=10
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
                onClicked: Settings.rate_Product0[10]<10?Settings.rate_Product0[10]=0:Settings.rate_Product0[10]-=10
        }

            function errormessage() { if (aog.actualRate < Settings.rate_Product0[10]*0.9 & aog.product0stat & Settings.rate_Product0[2]){
                timedMessage.addMessage(2000, qsTr("Low rate. Increase speed!"));}
                else if (aog.actualRate > Settings.rate_Product0[10] * 1.1 & aog.product0stat & Settings.rate_Product0[2]){
                timedMessage.addMessage(2000, qsTr("High rate. Reduce speed!"));}
            }

}
