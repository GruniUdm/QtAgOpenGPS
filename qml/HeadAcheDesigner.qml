import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import QtQuick.Shapes 1.15

Popup{
    id: headacheDesigner
    width: parent.width
    height: parent.height
    //color: "ghostwhite"
    closePolicy: Popup.NoAutoClose

    function show(){
        headacheDesigner.visible = true
    }

    property int headacheCount: 0
    property bool curveLine: false
    property double zoom: 1
    property double sX: 0
    property double sY: 0

    signal load()
    signal close()
    signal update_lines()
    signal save_exit()

    signal mouseClicked(int x, int y)
    signal mouseDragged(int fromX, int fromY, int toX, int toY)
    //signal zoom(bool checked)
    //signal deletePoint()
    //signal remove_headland()
    signal createHeadland()
    signal deleteHeadland()
    //signal deletePoints()
    signal ashrink()
    signal alength()
    signal bshrink()
    signal blength()
    signal headlandOff()
    signal cycleForward()
    signal cycleBackward()
    signal deleteCurve()
    signal cancelTouch()

    signal isSectionControlled(bool wellIsIt)

    onWidthChanged: if(aog.isJobStarted) update_lines()
    onHeightChanged: if(aog.isJobStarted) update_lines()
    onVisibleChanged: {
        if(visible) {
            load()
        } else {
            close()
        }
    }

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

    property var headacheLines: [
        {
            index: 0,
            color: "#FF0000",
            width: 4,
            points: [
                Qt.point(150, 150),
                Qt.point(200, 150),
                Qt.point(200, 200),
                Qt.point(150, 200),
                Qt.point(150, 150)
            ],
            dashed: false
        },
        {
            index: 1,
            color: "#00FF00",
            width: 4,
            points: [
                Qt.point(125, 125),
                Qt.point(175, 125),
                Qt.point(175, 175),
                Qt.point(125, 175),
                Qt.point(125, 125)
            ],
            dashed: true
        }
    ]

    property var headlandLine: [
        Qt.point(0,0),
        Qt.point(100,20)
    ]

    Rectangle {
        id: leftSide
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: buttons.left
        anchors.rightMargin: 20
        layer.enabled: true
        layer.samples: 8

        Rectangle{//renderer goes here
            id: headacheRenderer
            objectName: "headacheRenderer"
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            width: parent.width > parent.height ? parent.height : parent.width
            height: width  //1:1 aspect ratio

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

            Repeater {
                id: boundaryRepeater

                model: boundaryLines.length

                Shape {
                    property int outerIndex: index

                    anchors.fill: parent
                    Connections {
                        target: headacheDesigner
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
            Repeater {
                id: headlinesRepeater

                model: headacheLines.length

                Shape {
                    property int outerIndex: index

                    anchors.fill: parent
                    Connections {
                        target: headacheDesigner
                        function onHeadacheLinesChanged() {
                            headacheShapePath.draw_boundaries()
                        }
                    }

                    ShapePath {
                        id: headacheShapePath
                        strokeColor: headacheLines[index].color
                        strokeWidth: headacheLines[index].width
                        fillColor: "transparent"
                        startX: p[0].x
                        startY: p[0].y
                        scale: Qt.size(1,1)
                        joinStyle: ShapePath.RoundJoin

                        property var p: [Qt.point(headlandRenderer.width,0), Qt.point(0, headlandRenderer.height)]

                        PathPolyline {
                            id: headachePs
                            path: headacheShapePath.p
                        }


                        Component.onCompleted: draw_boundaries()


                        function draw_boundaries()
                        {
                            //console.debug(boundaryLines[index].points)
                            p = headacheLines[index].points
                            if(headacheLines[index].dashed)
                                headacheShapePath.strokeStyle = ShapePath.DashLine
                            else
                                headacheShapePath.strokeStyle = ShapePath.SolidLine
                        }
                    }
                }
            }

            MouseArea {
                id: headacheMouseArea
                anchors.fill: parent

                property int fromX: 0
                property int fromY: 0

                onClicked: {
                    if (cboxIsZoom.checked && headacheDesigner.zoom === 1) {
                        sX = ((parent.width / 2 - mouseX) / parent.width) * 1.1
                        sY = ((parent.width / 2 - mouseY) / -parent.width) * 1.1
                        zoom = 0.1
                    } else {
                        headacheDesigner.mouseClicked(mouseX, mouseY)
                    }
                }

                onPressed: {
                    //save a copy of the coordinates
                    fromX = mouseX
                    fromY = mouseY
                }

                onPositionChanged: {
                    headacheDesigner.mouseDragged(fromX, fromY, mouseX, mouseY)
                    fromX = mouseX
                    fromY = mouseY
                }

                //onWheel: {}
            }
        }
    }

    ColumnLayout{
        id: buttons
        //anchors.left: headacheRenderer.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        spacing: 30
        ButtonGroup{
            buttons: [ headacheAB, headacheCurve ]
        }
        GridLayout{
            id: top6btns
            flow: Grid.LeftToRight // NOTE!! this is not how I normally do it
            //but it seems to make the most sense here
            columns: 3
            rows: 2
            width: parent.width
            rowSpacing: buttons.spacing
            IconButtonTransparent{
                //objectName: "btnBLength"
                icon.source: "/images/APlusPlusB.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: headacheDesigner.blength()
            }
            IconButtonTransparent{
                //objectName: "btnBShrink"
                icon.source: "/images/APlusMinusB.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: HeadacheDesigner.bshrink()
            }
            IconButtonTransparent{
                //objectName: "cBoxIsSectionControlled"
                icon.source: "/images/HeadlandSectionOff.png"
                iconChecked: "/images/HeadlandSectionOn.png"
                checkable: true
                Layout.alignment: Qt.AlignCenter
                isChecked: settings.setHeadland_isSectionControlled
                onCheckedChanged: isSectionControlled(checked)
            }
            IconButtonTransparent{
                //objectName: "btnALength"
                icon.source: "/images/APlusPlusA.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: headacheDesigner.alength()
            }
            IconButtonTransparent{
                //objectName: "btnAShrink"
                icon.source: "/images/APlusMinusA.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: headacheDesigner.ashrink()
            }
            IconButtonTransparent{
                //objectName: "btnAShrink"
                icon.source: "/images/HeadlandDeletePoints.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: headacheDesigner.deletePoint()
            }
        }
        GridLayout{
            flow: Grid.LeftToRight
            rows: 3
            width: parent.width
            columns: 2
            rowSpacing: buttons.spacing
            IconButtonColor{
                id: headacheCurve
                //objectName: "rbtnLine"
                checkable: true
                isChecked: true
                Layout.alignment: Qt.AlignCenter
                icon.source: "/images/ABTrackCurve.png"
                onClicked: curveLine = true
            }
            IconButtonColor{
                id: headacheAB
                //objectName: "rbtnCurve"
                checkable: true
                Layout.alignment: Qt.AlignCenter
                icon.source: "/images/ABTrackAB.png"
                onClicked: curveLine = false
            }
            SpinBoxM {
                //objectName: "nudSetDistance"
                from: 0
                to: 2000
                boundValue: numTracks.value * settings.setVehicle_toolWidth
                Layout.alignment: Qt.AlignCenter
                TextLine{anchors.top: parent.bottom; text: "( "+ utils.m_unit_abbrev()+" )"}

                onValueChanged: lineDistance = value
            }
            SpinBoxCustomized{
                id: numTracks
                from: 0
                to: 10
                value: 0
                Layout.alignment: Qt.AlignCenter
                TextLine{anchors.top: parent.bottom; text: qsTr("Tool: ")+ utils.m_to_ft_string(settings.setVehicle_toolWidth)}
            }
            IconButtonColor{
                id: cboxIsZoom
                //objectName: "cboxIsZoom"
                checkable: true
                icon.source: "/images/ZoomOGL.png"
                Layout.alignment: Qt.AlignCenter
            }

            IconButtonTransparent{
                //objectName: "btnBndLoop"            rows: 2

                icon.source: "/images/HeadlandBuild.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: createHeadland()
            }
        }
        GridLayout{
            flow: Grid.LeftToRight
            rows: 2
            columns: 3
            rowSpacing: buttons.spacing
            width: parent.width
            IconButtonTransparent{
                //objectName: "btnDeletePoints"
                icon.source: "/images/HeadlandReset.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: deleteHeadland()
            }
            IconButtonTransparent{
                icon.source: "/images/ABLineCycleBk.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: cycleBackward()
            }
            IconButtonTransparent{
                icon.source: "/images/ABLineCycle.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: cycleForward()
            }
            IconButtonTransparent{
                icon.source: "/images/SwitchOff.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: {
                    headlandOff()
                    headacheDesigner.visible = false
                }
            }
            IconButtonTransparent{
                icon.source: "/images/Trash.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: deleteCurve()
            }
            IconButtonTransparent{
                icon.source: "/images/OK64.png"
                Layout.alignment: Qt.AlignCenter
                onClicked: {
                    save_exit()
                    boundaryInterface.isHeadlandOn = true
                    headacheDesigner.visible = false
                }
            }
        }
    }
}
