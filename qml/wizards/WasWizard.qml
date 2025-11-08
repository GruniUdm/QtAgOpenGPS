// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// On main GL
import QtQuick
import QtQuick.Controls.Fusion
import QtCharts 2.0
import QtQuick.Layouts
import AOG

import ".."
import "../components"

MoveablePopup {
    id: smartWasWizard
    height: 270  * theme.scaleHeight
    width: 270  * theme.scaleWidth
    visible: false
    modal: false
    onVisibleChanged: {if (visible){ aog.startDataCollection()} // Qt 6.8 MODERN: Direct Q_INVOKABLE call
                      else {aog.stopDataCollection()}} // Qt 6.8 MODERN: Direct Q_INVOKABLE call

    TopLine{
        id: topLine
        titleText: qsTr("Was Wizard")
    }

    Rectangle{
        id: recordBoundaryWindow
        anchors.top: topLine.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "lightgray"
        border.color: "black"
        border.width: 2
        ColumnLayout{
            id: topColumn
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: bottomRow.top
            spacing: 5
            anchors.topMargin: 15 * theme.scaleHeight
            anchors.bottomMargin: 15 * theme.scaleHeight
            Text {
                text: (aog.hasValidRecommendation?qsTr("Ready for Calibration"):(aog.sampleCount>=200?qsTr("Low Confidence"):qsTr("Collecting Data")))
                font.bold: true
                Layout.alignment: Qt.AlignCenter
            }
            Text {
                text: qsTr("Samples: " + aog.sampleCount)
                font.bold: true
                Layout.alignment: Qt.AlignCenter
            }
            Text {
                text: qsTr("Confidence: " +  Math.round(aog.confidenceLevel * 100) / 100  + " %")
                font.bold: true
                color: (aog.hasValidRecommendation?"darkGreen":(aog.sampleCount>=200?"orange":"red"))
                Layout.alignment: Qt.AlignCenter
            }
        }
        RowLayout{
            id: bottomRow
            //anchors.top: topColumn.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            spacing: 25 * theme.scaleWidth
            anchors.leftMargin: 25 * theme.scaleHeight
            anchors.rightMargin: 25 * theme.scaleHeight
            height: 100 * theme.scaleHeight

            IconButtonTransparent{
                icon.source: prefix + "/images/Trash.png"
                onClicked: aog.smartCalLabelClick() // Qt 6.8 MODERN: Direct Q_INVOKABLE call
                Layout.fillWidth: true
            }
            IconButtonTransparent{
                id: btnSmartZeroWAS
                icon.source: prefix + "/images/SteerZeroSmall.png"
                enabled: aog.hasValidRecommendation
                onClicked: aog.smartZeroWAS() // Qt 6.8 MODERN: Direct Q_INVOKABLE call
                Layout.fillWidth: true
            }
        }
    }
   }







