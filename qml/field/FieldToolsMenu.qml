// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Menu displayed when the "Field Tools" button is clicked
import QtQuick
import QtQuick.Controls.Fusion
import Settings

import ".."
import "../components"

Drawer {
    id: fieldToolsMenu

    visible: false
    width: 270 * theme.scaleWidth
    height: mainWindow.height
    modal: true

    contentItem: Rectangle {
        id: fieldToolsMenuRect
        anchors.bottom: parent.bottom
        height: parent.height
        anchors.top: parent.top
        anchors.left: parent.left

        color: "black"

        ScrollViewExpandableColumn {
            anchors.fill: fieldToolsMenuRect
            id: fieldToolsMenuColumn
            height: fieldToolsMenuRect.height
            spacing: 0
            IconButtonTextBeside{
                text: "Boundary"
                icon.source: prefix + "/images/MakeBoundary.png"
                //width: 300
                visible: Settings.feature_isBoundaryOn
                onClicked: {
                    fieldToolsMenu.visible = false
                    boundaryMenu.show()
                }
            }
            IconButtonTextBeside{
                text: "Headland"
                icon.source: prefix + "/images/HeadlandMenu.png"
                //width: 300
                visible: Settings.feature_isHeadlandOn
                onClicked: {
                    fieldToolsMenu.visible = false
                    if (boundaryInterface.count > 0) {
                        headlandDesigner.show()
                    }else{
                        timedMessage.addMessage(2000, qsTr("No Boundaries"), qsTr("Create A Boundary First"))
                    }
                }
            }
            IconButtonTextBeside{
                text: "Headland (Build)"
                icon.source: prefix + "/images/Headache.png"
                visible: Settings.feature_isHeadlandOn
                //width: 300
                onClicked: {
                    fieldToolsMenu.visible = false
                    headacheDesigner.show()
                }
            }
            IconButtonTextBeside{
                text: "Tram Lines"
                icon.source: prefix + "/images/TramLines.png"
                //width: 300
                visible: Settings.feature_isTramOn
                onClicked: tramLinesEditor.visible = true
            }
            IconButtonTextBeside{
                text: "Recorded Path"
                icon.source: prefix + "/images/RecPath.png"
                //width: 300
                visible: Settings.feature_isHeadlandOn
                onClicked:{
                    fieldToolsMenu.visible = false
                    recPath.show()
                }
            }
        }
    }
}
