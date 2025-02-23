// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import QtQuick.Shapes
import Settings
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

    property int sliceCount: 0
    property int backupCount: 0
    property bool curveLine: true
    property double lineDistance: 0

    property double zoom: 1
    property double sX: 0
    property double sY: 0

    //property double maxFieldDistance;
    //property double fieldCenterX;
    //property double fieldCenterY;
    property point apoint: Qt.point(0,0)
    property point bpoint: Qt.point(0,0)
    property bool showa: false
    property bool showb: false
    property color acolor: "red"
    property color bcolor: "blue"

    property point vehiclePoint: Qt.point(-100,-100)

    signal load()
    signal update_lines();

    signal save_exit()
    signal mouseClicked(int x, int y)
    //signal mouseDragged(int fromX, int fromY, int toX, int toY)
    //signal zoom(bool checked)
    signal close()
    signal slice()
    signal deletePoints()
    signal create_headland()
    signal undo()
    signal ashrink()
    signal alength()
    signal bshrink()
    signal blength()
    signal headlandOff()
    signal isSectionControlled(bool wellIsIt)

    //onClosed: { console.debug("closed.") }
    onWidthChanged: if(aog.isJobStarted) update_lines()
    onHeightChanged: if(aog.isJobStarted) update_lines()

    property var boundaryLines: [
        {
            index: 0,
            color: "#FF0000",
            width: 4,
            points: [
                Qt.point(50, 50),
                Qt.point(100, 50),
                Qt.point(100, 100),
                Qt.point(50, 100),
                Qt.point(50, 50)
            ]
        },
        {
            index: 1,
            color: "#00FF00",
            width: 4,
            points: [
                Qt.point(25, 25),
                Qt.point(75, 25),
                Qt.point(75, 75),
                Qt.point(25, 75),
                Qt.point(25, 25)
            ]
        }
    ]

    property var headlandLine: [
        Qt.point(0,0),
        Qt.point(100,20)
    ]

    property var sliceLine: [
        Qt.point(0,0),
        Qt.point(20,100)
    ]


    onHeadlandLineChanged: {
        if (headlandLine.length > 0)
            headlandShapePath.p = headlandLine
    }

    onSliceLineChanged: {
        sliceShapePath.p = sliceLine
    }

    onVisibleChanged: {
        if(visible) {
            load()
        } else {
            close()
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
                visible: headlandDesigner.showa
                width: 24
                height: 24
                radius: 12
                color: "#ffc059"
                x: headlandDesigner.apoint.x - 12
                y: headlandDesigner.apoint.y - 12
                z: 1
            }

            Rectangle {
                id: b_rect
                visible: headlandDesigner.showb
                width: 24
                height: 24
                radius: 12
                color:  "#80c0ff"
                x: headlandDesigner.bpoint.x - 12
                y: headlandDesigner.bpoint.y - 12
                z: 1
            }

            Rectangle {
                id: vehicle_point
                visible: true
                width: 24
                height: 24
                radius: 12
                color:  "#f33033"
                x: headlandDesigner.vehiclePoint.x - 12
                y: headlandDesigner.vehiclePoint.y - 12
            }

            Repeater {
                id: boundaryRepeater

                model: boundaryLines.length

                Shape {
                    property int outerIndex: index
                    smooth: true

                    anchors.fill: parent
                    Connections {
                        target: headlandDesigner
                        function onBoundaryLinesChanged() {
                            shapePath.draw_boundaries()
                        }
                    }

                    ShapePath {
                        id: shapePath
                        strokeColor: boundaryLines[index].color
                        strokeWidth: boundaryLines[index].width
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
                        //    console.debug(boundaryLines[index].points)
                            p = boundaryLines[index].points
                        }
                    }
                }
            }

            Shape {
                id: headlandShape
                visible: headlandLine.length > 0
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
                visible: sliceCount != 0
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
                    if (cboxIsZoom.checked && headlandDesigner.zoom === 1) {
                        sX = ((parent.width / 2 - mouseX) / parent.width) * 1.1
                        sY = ((parent.height / 2 - mouseY) / -parent.height) * 1.1
                        //console.debug("width,mouse, sx,sy",parent.width / 2, mouseX, mouseY, sX,sY);
                        zoom = 0.1
                        headlandDesigner.update_lines()
                    } else {
                        headlandDesigner.mouseClicked(mouseX, mouseY)
                        if (zoom != 1.0) {
                            zoom = 1.0;
                            sX = 0;
                            sY = 0;
                            headlandDesigner.update_lines()
                        }
                    }
                }

                onPressed: {
                    //save a copy of the coordinates
                    fromX = mouseX
                    fromY = mouseY
                }

                onPositionChanged: {
                    headlandDesigner.mouseDragged(fromX, fromY, mouseX, mouseY)
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
                headlandDesigner.blength()
            }
        }
        Comp.IconButtonTransparent{
            objectName: "btnBShrink"
            icon.source: prefix + "/images/APlusMinusB.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                headlandDesigner.bshrink()
            }
        }
        Comp.IconButtonTransparent{
            objectName: "btnALength"
            icon.source: prefix + "/images/APlusPlusA.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                headlandDesigner.alength()
            }
        }
        Comp.IconButtonTransparent{
            objectName: "btnAShrink"
            icon.source: prefix + "/images/APlusMinusA.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                headlandDesigner.ashrink()
            }
        }
        Comp.IconButtonColor{
            id: headlandCurve
            objectName: "rbtnCurve"
            checkable: true
            //isChecked: true
            icon.source: prefix + "/images/ABTrackCurve.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: curveLine = true
        }
        Comp.IconButtonColor{
            id: headlandAB
            objectName: "rbtnLine"
            checkable: true
            icon.source: prefix + "/images/ABTrackAB.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: curveLine = false
        }
        Comp.SpinBoxM {
            id: distanceSpinBox
            objectName: "nudSetDistance"
            from: 0
            to: 2000
            boundValue: numTracks.value * Settings.vehicle_toolWidth
            Layout.alignment: Qt.AlignCenter
            Comp.TextLine {
                anchors.top: parent.bottom;
                text: "( "+ Utils.m_unit_abbrev()+" )"
            }
            onValueChanged: {
                lineDistance = value
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
                text: qsTr("Tool: ")+ Utils.m_to_ft_string(Settings.vehicle_toolWidth)
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
            enabled: (sliceCount > 0)
            Layout.alignment: Qt.AlignCenter
            onClicked: slice()
        }
        Comp.IconButtonTransparent{
            objectName: "btnBndLoop"
            icon.source: prefix + "/images/HeadlandBuild.png"
            onClicked: create_headland()
            Layout.alignment: Qt.AlignCenter
        }
        Comp.IconButtonTransparent{
            objectName: "btnDeletePoints"
            icon.source: prefix + "/images/HeadlandReset.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: deletePoints()
        }
        Comp.IconButtonTransparent{
            objectName: "btnUndo"
            icon.source: prefix + "/images/back-button.png"
            enabled: (backupCount > 0)
            Layout.alignment: Qt.AlignCenter
            onClicked: undo()
        }
        Comp.IconButtonTransparent{
            objectName: "cBoxIsSectionControlled"
            icon.source: prefix + "/images/HeadlandSectionOff.png"
            iconChecked: prefix + "/images/HeadlandSectionOn.png"
            checkable: true
            isChecked: Settings.headland_isSectionControlled
            Layout.alignment: Qt.AlignCenter
            onCheckedChanged: isSectionControlled(checked)
        }
        Comp.IconButtonTransparent{
            icon.source: prefix + "/images/SwitchOff.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                headlandDesigner.headlandOff()
                headlandDesigner.visible = false
            }
        }
        Comp.IconButtonTransparent{
            icon.source: prefix + "/images/OK64.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: {
                save_exit()
                boundaryInterface.isHeadlandOn = true
                headlandDesigner.visible = false
            }
        }
    }
}
