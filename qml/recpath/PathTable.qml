// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Loaded by PathOpen.qml. Contains the list of Paths
import QtQuick
import QtQuick.Controls
import AOG
import "../" //bring in Utils


pragma ComponentBehavior: Bound

ListView {
    id: tableView


    //needed for qmlscene testing
    Component.onCompleted: update_model()
    clip: true

    property int sortBy: 1 //1 = name, 2 = distance, negative is reverse

    onSortByChanged: sort()

    function clear_selection() {
        currentIndex = -1
        currentPathName = ""
    }

    function update_model() {
        pathsModel.clear()
        
        // Scan for path files in fields directory
        var pathFiles = RecordedPathInterface.scanPathFiles()
        console.log("Path files found:", pathFiles.length)
        
        if (pathFiles && pathFiles.length > 0) {
            for( var i=0; i < pathFiles.length ; i++) {
                var fileName = pathFiles[i]
                if (fileName) {
                    pathsModel.append( { 
                        index: i,
                        name: String(fileName),
                        distance: 0,
                        boundaryArea: 0
                    } )
                }
            }
        } 
        sort()
    }

    function listModelSort(listModel, compare_function) {
        let indexes = [ ...Array(listModel.count).keys() ]

        indexes.sort( (a, b) => compare_function( listModel.get(a), listModel.get(b) ) )
        let sorted = 0

        while ( sorted < indexes.length && sorted === indexes[sorted] ) sorted++

        if ( sorted === indexes.length ) return

        for ( let i = sorted; i < indexes.length; i++ ) {
            listModel.move( indexes[i], listModel.count - 1, 1 )
            listModel.insert( indexes[i], { } )
        }

        listModel.remove( sorted, indexes.length - sorted )

        //fix stupid indexes
        for( let j = 0; j < pathsModel.count ; j++) {
            pathsModel.get(j).index = j
        }
    }

    function sort() {
        if (pathsModel.count <= 1) {
            currentIndex = -1
            currentPathName = ""
            return
        }
        
        if (sortBy === -1) {
            listModelSort( pathsModel, (a, b) => - a.name.localeCompare(b.name) )
        } else {
            listModelSort( pathsModel, (a, b) => a.name.localeCompare(b.name) )
        }

        currentIndex = -1
        currentPathName = ""
    }

    property string currentPathName: ""
    property int adjustWidth: -10

    keyNavigationEnabled: true

    model: pathsModel
    currentIndex: -1
    focus: true
    headerPositioning: ListView.OverlayHeader

    header: Rectangle {
        z: 2
        color: "white"
        implicitWidth: tableView.width + tableView.adjustWidth
        height: childrenRect.height

        Rectangle {
            id: nameHeaderRect

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: nameHeaderText.height + 10

            border.color: "black"
            border.width: 1
            color: "white"

            Text {
                id: nameHeaderText
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 5

                text: qsTr("Path name")
                font.pixelSize: 20
            }
            MouseArea {
                anchors.fill: parent

                onClicked: {
                    if (Math.abs(tableView.sortBy) != 1) {
                        tableView.sortBy = 1
                    } else {
                        tableView.sortBy = -tableView.sortBy
                    }
                }
            }
        }
    }

    spacing: 2

    delegate: ItemDelegate {
        required property int index
        required property string name
        
        width: tableView.width + tableView.adjustWidth

        text: name

        onClicked: {
            tableView.currentIndex = index
            tableView.currentPathName = name
        }
    }

    ListModel {
        id: pathsModel
    }
}
