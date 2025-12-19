// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import QtQuick.Shapes
//import Settings
import AOG
import "components" as Comp


Popup{
    id: headlandDesigner
    width: parent.width
    height: parent.height
    closePolicy: Popup.NoAutoClose

    //color: "ghostwhite"
    function show(){
        headlandDesigner.visible = true
        headlandCurve.isChecked = true
    }

    //onClosed: { console.debug("closed.") }
    onWidthChanged: if(aog.isJobStarted) HeadlandInterface.updateLines()
    onHeightChanged: if(aog.isJobStarted) HeadlandInterface.updateLines()

    Connections {
        target: HeadlandInterface

        function onHeadlandLineChanged() {
            if (HeadlandInterface.headlandLine.length > 0)
                headlandShapePath.p = HeadlandInterface.headlandLine
        }

        function onSliceLineChanged() {
            sliceShapePath.p = HeadlandInterface.sliceLine
        }

    }

    Connections {
        target: headlandRenderer

        function onWidthChanged() {
            HeadlandInterface.viewportWidth = headlandRenderer.width;
        }

        function onHeightChanged() {
            HeadlandInterface.viewportHeight = headlandRenderer.height;
        }

    }

    onVisibleChanged: {
        if(visible) {
            HeadlandInterface.load()
        } else {
            HeadlandInterface.close()
        }
    }

    Rectangle {
        id: leftSide
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: buttons.left
        anchors.rightMargin: 20
        layer.enabled: true
        layer.samples: 8

        Rectangle {//renderer goes here
            id: headlandRenderer
            objectName: "headlandRenderer"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            width: parent.width > parent.height ? parent.height : parent.width
            height: width  //1:1 aspect ratio
            //width: parent.width * .7
            color: "black"

            Rectangle {
                id: a_rect
                visible: HeadlandInterface.showa
                width: 24
                height: 24
                radius: 12
                color: "#ffc059"
                x: HeadlandInterface.apoint.x - 12
                y: HeadlandInterface.apoint.y - 12
                z: 1
            }

            Rectangle {
                id: b_rect
                visible: HeadlandInterface.showb
                width: 24
                height: 24
                radius: 12
                color:  "#80c0ff"
                x: HeadlandInterface.bpoint.x - 12
                y: HeadlandInterface.bpoint.y - 12
                z: 1
            }

            Rectangle {
                id: vehicle_point
                visible: true
                width: 24
                height: 24
                radius: 12
                color:  "#f33033"
                x: HeadlandInterface.vehiclePoint.x - 12
                y: HeadlandInterface.vehiclePoint.y - 12
            }

            Repeater {
                id: boundaryRepeater

                model: HeadlandInterface.boundaryLines.length

                Shape {
                    property int outerIndex: index
                    smooth: true

                    anchors.fill: parent

                    Connections {
                        target: HeadlandInterface

                        function onBoundaryLinesChanged() {
                            shapePath.draw_boundaries()
                        }
                    }

                    ShapePath {
                        id: shapePath
                        strokeColor: HeadlandInterface.boundaryLines[index].color
                        strokeWidth: HeadlandInterface.boundaryLines[index].width
                        fillColor: "transparent"
                        startX: p[0].x
                        startY: p[0].y
                        scale: Qt.size(1,1)
                        joinStyle: ShapePath.RoundJoin

                        property var p: [Qt.point(0,0), Qt.point(headlandRenderer.width, headlandRenderer.height)]

                        PathPolyline {
                            id: ps
                            path: shapePath.p
                        }


                        Component.onCompleted: draw_boundaries()


                        function draw_boundaries()
                        {
                        //    console.debug(HeadlandInterface.boundaryLines[index].points)
                            p = HeadlandInterface.boundaryLines[index].points
                        }
                    }
                }
            }

            Shape {
                id: headlandShape
                visible: HeadlandInterface.headlandLine.length > 0
                anchors.fill: parent
                ShapePath {
                    id: headlandShapePath
                    strokeColor: "#f1e817"
                    strokeWidth: 8
                    fillColor: "transparent"
                    startX: p[0].x
                    startY: p[0].y
                    joinStyle: ShapePath.RoundJoin

                    property var p: [
                        Qt.point(0,0),
                        Qt.point(20,100),
                        Qt.point(200,150)
                    ]

                    PathPolyline {
                        id: headlandShapePolyine
                        path: headlandShapePath.p
                    }
                }
            }

            Shape {
                id: sliceShape
                visible: HeadlandInterface.sliceCount != 0
                anchors.fill: parent
                ShapePath {
                    id: sliceShapePath
                    strokeColor: headlandAB.checked ? "#f31700" : "#21f305"
                    strokeWidth: 8
                    fillColor: "transparent"
                    startX: p[0].x
                    startY: p[0].y
                    joinStyle: ShapePath.RoundJoin

                    property var p: [
                        Qt.point(0,0),
                        Qt.point(100,20),
                    ]

                    PathPolyline {
                        id: sliceShapePolyLine
                        path: sliceShapePath.p
                    }
                }
            }

            MouseArea {
                id: headlandMouseArea
                anchors.fill: parent

                property int fromX: 0
                property int fromY: 0

                onClicked: {
                    if (cboxIsZoom.checked && HeadlandInterface.zoom === 1) {
                        HeadlandInterface.sX = ((parent.width / 2 - mouseX) / parent.width) * 1.1
                        HeadlandInterface.sY = ((parent.height / 2 - mouseY) / -parent.height) * 1.1
                        //console.debug("width,mouse, sx,sy",parent.width / 2, mouseX, mouseY, HeadlandInterface.sX,HeadlandInterface.sY);
                        HeadlandInterface.zoom = 0.1
                        HeadlandInterface.updateLines()
                    } else {
                        HeadlandInterface.mouseClicked(mouseX, mouseY)
                        if (HeadlandInterface.zoom != 1.0) {
                            HeadlandInterface.zoom = 1.0;
                            HeadlandInterface.sX = 0;
                            HeadlandInterface.sY = 0;
                            HeadlandInterface.updateLines()
                        }
                    }
                }

                onPressed: {
                    //save a copy of the coordinates
                    fromX = mouseX
                    fromY = mouseY
                }

                onPositionChanged: {
                    //not currently used in formheadland.cpp
                    HeadlandInterface.mouseDragged(fromX, fromY, mouseX, mouseY)
                    fromX = mouseX
                    fromY = mouseY
                }

                //onWheel: {}
            }
        }
    }

    GridLayout{
        id: buttons
        //anchors.left: headlandRenderer.right
        //width: distanceSpinBox.implicitWidth * 2
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        flow: Grid.LeftToRight // NOTE!! this is not how I normally do it
                               //but it seems to make the most sense here
        columns: 2
        rows: 9
        ButtonGroup{
            buttons: [ headlandAB, headlandCurve ]
        }

        Comp.IconButtonTransparent{
            objectName: "btnBLength"
            icon.source: prefix + "/images/APlusPlusB.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                HeadlandInterface.blength()
            }
        }
        Comp.IconButtonTransparent{
            objectName: "btnBShrink"
            icon.source: prefix + "/images/APlusMinusB.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                HeadlandInterface.bshrink()
            }
        }
        Comp.IconButtonTransparent{
            objectName: "btnALength"
            icon.source: prefix + "/images/APlusPlusA.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                HeadlandInterface.alength()
            }
        }
        Comp.IconButtonTransparent{
            objectName: "btnAShrink"
            icon.source: prefix + "/images/APlusMinusA.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                HeadlandInterface.ashrink()
            }
        }
        Comp.IconButtonColor{
            id: headlandCurve
            objectName: "rbtnCurve"
            checkable: true
            //isChecked: true
            icon.source: prefix + "/images/ABTrackCurve.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: HeadlandInterface.curveLine = true
        }
        Comp.IconButtonColor{
            id: headlandAB
            objectName: "rbtnLine"
            checkable: true
            icon.source: prefix + "/images/ABTrackAB.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: HeadlandInterface.curveLine = false
        }
        Comp.SpinBoxM {
            id: distanceSpinBox
            objectName: "nudSetDistance"
            from: 0
            to: 2000
            boundValue: numTracks.value * SettingsManager.vehicle_toolWidth
            Layout.alignment: Qt.AlignCenter
            Comp.TextLine {
                anchors.top: parent.bottom;
                text: "( "+ Utils.m_unit_abbrev()+" )"
            }
            onValueChanged: {
                HeadlandInterface.lineDistance = value
            }
        }
        Comp.SpinBoxCustomized{
            id: numTracks
            from: 0
            to: 10
            value: 0
            Layout.alignment: Qt.AlignCenter
            Comp.TextLine {
                anchors.top: parent.bottom;
                text: qsTr("Tool: ")+ Utils.m_to_ft_string(SettingsManager.vehicle_toolWidth)
            }
        }
        Comp.IconButtonColor{
            id: cboxIsZoom
            objectName: "cboxIsZoom"
            checkable: true
            icon.source: prefix + "/images/ZoomOGL.png"
            Layout.alignment: Qt.AlignCenter
        }

        Comp.IconButtonTransparent{
            objectName: "btnSlice"
            icon.source: prefix + "/images/HeadlandSlice.png"
            enabled: (HeadlandInterface.sliceCount > 0)
            Layout.alignment: Qt.AlignCenter
            onClicked: HeadlandInterface.slice()
        }
        Comp.IconButtonTransparent{
            objectName: "btnBndLoop"
            icon.source: prefix + "/images/HeadlandBuild.png"
            onClicked: HeadlandInterface.createHeadland()
            Layout.alignment: Qt.AlignCenter
        }
        Comp.IconButtonTransparent{
            objectName: "btnDeletePoints"
            icon.source: prefix + "/images/HeadlandReset.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: HeadlandInterface.deletePoints()
        }
        Comp.IconButtonTransparent{
            objectName: "btnUndo"
            icon.source: prefix + "/images/back-button.png"
            enabled: (HeadlandInterface.backupCount > 0)
            Layout.alignment: Qt.AlignCenter
            onClicked: HeadlandInterface.undo()
        }
        Comp.IconButtonTransparent{
            objectName: "cBoxIsSectionControlled"
            icon.source: prefix + "/images/HeadlandSectionOn.png"
            iconChecked: prefix + "/images/HeadlandSectionOff.png"
            checkable: true
            // Threading Phase 1: Headland section control
            isChecked: SettingsManager.headland_isSectionControlled
            Layout.alignment: Qt.AlignCenter
            onCheckedChanged: HeadlandInterface.isSectionControlled(checked)
        }
        Comp.IconButtonTransparent{
            icon.source: prefix + "/images/SwitchOff.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                MainWindowState.isHeadlandOn = false
                headlandDesigner.visible = false
            }
        }
        Comp.IconButtonTransparent{
            icon.source: prefix + "/images/OK64.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                HeadlandInterface.saveExit()
                MainWindowState.isHeadlandOn = true
                headlandDesigner.visible = false
            }
        }
    }
}
