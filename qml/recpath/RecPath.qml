// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Recorded path display buttons
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Fusion

import AOG
import "../components" as Comp

Comp.MoveablePopup{
    id: recPath
    objectName: "recPath"
    height: 500  * theme.scaleHeight
    width: 100  * theme.scaleWidth
    x: 40
    y: 40
    closePolicy: Popup.NoAutoClose
    function show(){
        recPath.visible = !recPath.visible
    }
    onVisibleChanged: if(visible)
                          RecordedPathInterface.recPathLoad()
                      else {
                          RecordedPathInterface.recPathClear()
                          playBtn.checked = false}

    Connections {
        target: RecordedPathInterface
        function onShowPathNewDialog() {
            pathNew.visible = true
        }
    }

    Comp.ScrollViewExpandableColumn{
        id: recPathColumn
        width: children.width
        height: parent.height-20
        anchors.centerIn: parent
        Comp.IconButtonTransparent{
            id: playBtn
            enabled: !recordPathBtn.checked
            checkable: true
            checked: RecordedPathInterface.isDriving
            icon.source: prefix + "/images/Play.png"
            iconChecked: prefix + "/images/boundaryStop.png"
            onClicked: RecordedPathInterface.recPathFollowStop()
        }
        Comp.IconButtonTransparent{
            id: resumeBtn
            enabled: !recordPathBtn.checked && !playBtn.checked
            icon.source: {
                switch(RecordedPathInterface.resumeState) {
                case 0: return prefix + "/images/pathResumeStart.png"
                case 1: return prefix + "/images/pathResumeLast.png"
                case 2: return prefix + "/images/pathResumeClose.png"
                default: return prefix + "/images/pathResumeStart.png"
                }
            }
            onClicked: RecordedPathInterface.recPathResumeStyle()
        }
        Comp.IconButtonTransparent{
            id: recordPathBtn
            enabled: !playBtn.checked
            checkable: true
            icon.source: prefix + "/images/BoundaryRecord.png"
            iconChecked: prefix + "/images/boundaryStop.png"
            onClicked: RecordedPathInterface.recPathRecordStop()
        }
        Comp.IconButtonTransparent{
            enabled: !recordPathBtn.checked && !playBtn.checked
            icon.source: prefix + "/images/FileExplorerWindows.png"
            onClicked: {
                recPathOpen.visible = true;
            }
        }
        Comp.IconButtonTransparent{
            icon.source: prefix + "/images/ABSwapPoints.png"
            onClicked: RecordedPathInterface.recPathSwapAB()
        }
    }
}
