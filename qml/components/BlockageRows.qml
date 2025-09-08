// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import Settings
//This is a the row of on-screen section-control buttonw

Rectangle {
    id: blockageRows
    objectName: "blockageRows"
    anchors.horizontalCenter: parent.horizontalCenter
    width: 15 * theme.scaleWidth * numRows
    //width: (800 * theme.scaleWidth / numRows) < 50 ? (15 * theme.scaleWidth * numRows) : (20 * theme.scaleWidth * numRows)
    //height: childrenRect.height * theme.scaleHeight

    color: "transparent"


    property int numRows:  Number(Settings.seed_blockRow1 + Settings.seed_blockRow2 + Settings.seed_blockRow3 + Settings.seed_blockRow4)
    property int countMin: Number(Settings.seed_blockCountMin*10000)
    property int countMax: Number(Settings.seed_blockCountMax*10000)
    property color offColor: "Crimson"
    property color offTextColor: "White"
    property color onColor: "DarkGoldenrod"
    property color onTextColor: "White"
    property color autoColor: "ForestGreen"
    property color autoTextColor: "White"
    property variant blockageRowCount: aog.blockageRowCount


    //methods
    function setColors() {
        //same colors for sections and zones
        if (Settings.display_isDayMode) {
            blockageRows.offColor = "Red"
            blockageRows.offTextColor = "Black"
            blockageRows.onColor = "Yellow"
            blockageRows.onTextColor = "Black"
            blockageRows.onColor = "Yellow"
            blockageRows.onTextColor = "Black"
        } else {
            blockageRows.offColor = "Crimson"
            blockageRows.offTextColor = "White"
            blockageRows.onColor = "DarkGoldenRod"
            blockageRows.onTextColor = "White"
            blockageRows.onColor = "DarkGoldenRod"
            blockageRows.onTextColor = "White"
        }
    }
    function setSizes() {
        //same colors for sections and zones
        numRows = Number(Settings.seed_blockRow1 + Settings.seed_blockRow2 + Settings.seed_blockRow3 + Settings.seed_blockRow4)
        countMin =  Number(Settings.seed_blockCountMin)
        countMax =  Number(Settings.seed_blockCountMax)
        }



    onNumRowsChanged: {
        rowModel.clear()
        for (var i = 0; i < numRows; i++) {
            if(i < blockageRows.blockageRowCount.length){
                rowModel.append( { rowNo: i } )
            }

        }
    }

    //callbacks, connections, and signals
    Component.onCompleted:  {
        setColors()
        setSizes()
        rowModel.clear()
        for (var i = 0; i < numRows; i++) {
            if(i < blockageRows.blockageRowCount.length){
                rowModel.append( { rowNo: i } )
            }
        }
    }

Connections {
    target: Settings
    function onDisplay_isDayModeChanged() {
        setColors()
    }
    function onSeed_blockrow1Changed() {
        setSizes()
    }
}

    ListModel {
        id: rowModel
    }

    Component {
        id: rowViewDelegate
        BlockageRow {
            width: (800 * theme.scaleWidth / numRows) < 50 ? (15 * theme.scaleWidth) : (20 * theme.scaleWidth)
            //height: (10 * theme.scaleWidth)
            height: (blockageRows.blockageRowCount[model.rowNo] * 40/(aog.blockage_max+1)+20)*theme.scaleHeight<45*theme.scaleHeight?(blockageRows.blockageRowCount[model.rowNo] * 40/(aog.blockage_max+1)+20)*theme.scaleHeight:40*theme.scaleHeight
            //anchors.bottom: parent.bottom
            //buttonText: (model.rowNo + 1).toFixed(0)
            // visible: (model.rowNo < numRows) ? true : false
            color: (blockageRows.blockageRowCount[model.rowNo] < countMin ? offColor : (blockageRows.blockageRowCount[model.rowNo] < countMax ? autoColor : onColor))
            //textColor: (blockageRows.blockageRowCount[model.rowNo]===0 ? offTextColor : (blockageRows.blockageRowCount[model.rowNo] === 1 ? autoTextColor : onTextColor))
        }
    }

    ListView {
        id: blockageRowList
        orientation: Qt.Horizontal
        reuseItems: true
        //width: rowViewDelegate.width
        width: 400
        height: 100 * theme.scaleHeight
        anchors.left: parent.left
        anchors.right: parent.right
        //anchors.top: parent.top
        model: rowModel
        //spacing: 1
        //boundsMovement: Flickable.StopAtBounds
        delegate: rowViewDelegate
    }
}
