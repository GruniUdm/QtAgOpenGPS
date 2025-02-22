// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Record Boudnary window
import QtQuick
import QtQuick.Layouts
import Qt.labs.folderlistmodel
import QtQuick.Controls.Fusion
import QtQuick.Controls.Material
import AOG

import ".."
import "../components"

MoveablePopup {
    id: boundaryRecord

    visible: false
    modal: false

    onVisibleChanged: {
        if (visible) {
            boundaryRecordBtn.checked = false
            boundaryInterface.start()
        }
    }
    TopLine{
        id: recordTopLine
        titleText: qsTr("Record Boundary")
    }

    Rectangle{
        id: recordBoundaryWindow
        anchors.top: recordTopLine.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "lightgray"
        border.color: "aqua"
        border.width: 2
        SpinBoxCM{
            height: 80  * theme.scaleHeight
            anchors.left: parent.left
            anchors.right: side2Record.left
            anchors.top: recordBoundaryWindow.top
            anchors.margins: 5
            objectName: "boundaryRecordOffset"
            text: qsTr("Centimeter")
            from: 0
            to: 1968
            boundValue: boundaryInterface.createBndOffset
            onValueChanged: boundaryInterface.createBndOffset = value
        }
        IconButtonTransparent{
            id: side2Record
            objectName: "btnBoundarySide2Record"
            icon.source: prefix + "/images/BoundaryRight.png"
            iconChecked: prefix + "/images/BoundaryLeft.png"
            checkable: true
            anchors.top: recordBoundaryWindow.top
            anchors.right: parent.right
            anchors.margins: 5  * theme.scaleHeight
            border: 1
            height: 80  * theme.scaleHeight
            width: 80  * theme.scaleWidth
            isChecked: ! boundaryInterface.isDrawRightSide
            onClicked: {
                if (checked)
                    boundaryInterface.isDrawRightSide = false
                else
                    boundaryInterface.isDrawRightSide = true
            }
        }

        Row{
            anchors.top: side2Record.bottom
            anchors.margins: 5  * theme.scaleHeight
            width: parent.width -10
            anchors.horizontalCenter: parent.horizontalCenter
            height: childrenRect.height
            spacing: 30
            IconButtonTransparent{
                border: 1
                id: boundaryDelete
                objectName: "recordBoundaryDelete"
                icon.source: prefix + "/images/BoundaryDelete.png"
                height: 80  * theme.scaleHeight
                width: 80  * theme.scaleWidth
                onClicked: boundaryInterface.reset()
            }
            Column{
                height: boundaryDelete.height
                width: boundaryDelete.width
                spacing: 20 * theme.scaleHeight
                Text{
                    text: qsTr("Area:") + " " + Utils.area_to_unit_string(boundaryInterface.area,1) + " " + Utils.area_unit()
                }
                Text{
                    text: qsTr("Points:") + " " + boundaryInterface.pts
                }
            }
        }

        Grid{
            id: bottom4buttons
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 5  * theme.scaleHeight
            height: children.height
            columns: 2
            rows: 2
            flow: Grid.LeftToRight
            spacing:	30  * theme.scaleHeight
            IconButtonTransparent{
                border: 1
                objectName: "btnBoundaryRecordAddPoint"
                icon.source: prefix + "/images/PointAdd.png"
                height: 80  * theme.scaleHeight
                width: 80  * theme.scaleWidth
                onClicked: boundaryInterface.add_point()
            }
            IconButtonTransparent{
                border: 1
                objectName: "btnBoundaryRecordDeleteLastPoint"
                icon.source: prefix + "/images/PointDelete.png"
                height: 80  * theme.scaleHeight
                width: 80  * theme.scaleWidth
                onClicked: boundaryInterface.delete_last_point()
            }
            IconButtonTransparent{
                id: boundaryRecordBtn
                objectName: "btnBoundaryRecordRecord"
                icon.source: prefix + "/images/BoundaryRecord.png"
                checkable: true
                height: 80  * theme.scaleHeight
                width: 80  * theme.scaleWidth
                border: 1

                onCheckedChanged: {
                    if (checked)
                        boundaryInterface.record()
                    else
                        boundaryInterface.pause()
                }
            }

            IconButtonTransparent{
                objectName: "btnBoundaryRecordSave"
                icon.source: prefix + "/images/OK64.png"
                onClicked: {
                    boundaryRecord.visible = false
                    boundaryInterface.stop()
                }
                height: 80  * theme.scaleHeight
                width: 80  * theme.scaleWidth
            }
        }
    }
}
