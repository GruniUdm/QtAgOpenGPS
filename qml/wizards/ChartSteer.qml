// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// On main GL
import QtQuick
import QtQuick.Controls.Fusion
import QtCharts 2.0
import QtQuick.Layouts

import ".."
import "../components"

MoveablePopup {
    id: chartSteer
    height: 300  * theme.scaleHeight
    width: 500  * theme.scaleWidth
    visible: false
    modal: false
    property int yval:0
    property int xval1:0
    property int xval2:0
    property int axismin:0
    property int axismax:0
    property string chartName
    property string lineName1
    property string lineName2
    x: 400 * theme.scaleWidth

    Timer {
        id:txt
        interval:50; running: true; repeat: true
        onTriggered: {
                yval++;
            chart.series(0).append(yval, xval1);
            chart.series(1).append(yval, xval2);
           if(yval >20)
            {
            chart.axisX().max = yval
            chart.axisX().min = yval-50
            }

                    }
    }
    TopLine{
        id: steerChartTopLine
        titleText: qsTr(chartName)
        onBtnCloseClicked:  chartSteer.close()
    }

    Rectangle{
        id: steerChartWindow
        anchors.top: steerChartTopLine.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "black"



            IconButtonTextBeside{
            id: btnChartplus
            anchors.right: parent.right
            anchors.top: parent.top
            width: 40 * theme.scaleWidth
            height: 50 * theme.scaleHeight
            text: "   +"
            z: 2
            onClicked: {axismin > - 1 ? -1 : axismin = axismin+2
                        axismax < 1 ? 1 : axismax = axismax-2}
            }


            IconButtonTextBeside{
            id: btnChartauto
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            width: 40 * theme.scaleWidth
            height: 50 * theme.scaleHeight
            text: "   A"
            z: 2
            onClicked: {xval1 > axismax ? axismax = (xval1 + 2) : 0
                        xval2 > axismax ? axismax = (xval2 + 2) : 0
                        xval1 < axismin ? axismin = (xval1 - 2) : 0
                        xval2 < axismin ? axismin = (xval2 - 2) : 0
            }
            }

            IconButtonTextBeside{
            id: btnChartminus
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: 40 * theme.scaleWidth
            height: 50 * theme.scaleHeight
            text: "   -"
            z: 2
            onClicked: {axismin = axismin-2
                        axismax = axismax+2}
            }

    ChartView {
            id: chart
            animationOptions: ChartView.NoAnimation
            theme: ChartView.ChartThemeDark
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            legend.visible: true
            //legend.alignment: Qt.AlignBottom
            antialiasing: true

            LineSeries {
                id: lineSeries1
                name: lineName1
                axisX: axisX
                axisY: axisY
                useOpenGL: chartView.openGL
            }
            LineSeries {
                id: lineSeries2
                name: lineName2
                axisX: axisX
                axisYRight: axisY
                useOpenGL: chartView.openGL
            }

            ValueAxis {
                id: axisX
                min: 0
                max: 10
            }

            ValueAxis {
                id: axisX2
                min: 0
                max: 10
            }

            ValueAxis {
                id: axisY
                min: axismin
                max: axismax
            }

            Component.onCompleted: {
            chart.removeAllSeries();
            var series1 = chart.createSeries(ChartView.SeriesLineSeries, lineName1, axisX, axisY);
            var series2 = chart.createSeries(ChartView.SeriesLineSeries, lineName2,axisX, axisY);
                        }
            }


     }
}
