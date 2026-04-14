// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Loaded by PathOpen.qml. Contains the list of Paths
import QtQuick
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
        var distance = 0.0

        pathsModel.clear()
        for( var i=0; i < RecordedPathInterface.Path_list.length ;i++)  {
            distance = Utils.distanceLatLon(Backend.fixFrame.latitude, Backend.fixFrame.longitude, RecordedPathInterface.Path_list[i].latitude, RecordedPathInterface.Path_list[i].longitude)
            pathsModel.append( { index: i,
                                  name: RecordedPathInterface.Path_list[i].name,
                                  distance: distance,
                                  boundaryArea: RecordedPathInterface.Path_list[i].boundaryArea
                              } )
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
        if (sortBy === -1) {
            listModelSort( pathsModel, (a, b) => - a.name.localeCompare(b.name) )
        } else {
            listModelSort( pathsModel, (a, b) => a.name.localeCompare(b.name) )
        }

        currentIndex = -1
        currentPathName = ""
    }

    //TODO implement a model sort function

    Connections {
        target: RecordedPathInterface
        function onPath_listChanged() {
            tableView.update_model()
        }
    }

    property string currentPathName: ""
    property int adjustWidth: -10

    //Layout.minimumWidth: 200
    //Layout.minimumHeight: 200
    //Layout.preferredWidth: 400
    //Layout.preferredHeight: 400

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
                //anchors.verticalCenter: parent.verticalCenter

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

    delegate: Rectangle {
        id: pathDelegate
        height: childrenRect.height
        implicitWidth: tableView.width + tableView.adjustWidth

        required property double boundaryArea
        required property double distance
        required property string name
        required property int index

        color: ListView.isCurrentItem ? "light blue" : "light grey" //TODO: use AOGTheme item

        Text {
            id: pathName
            anchors.top: parent.top
            anchors.left: parent.left
            width: parent.width * 0.5

            anchors.topMargin: 5
            anchors.leftMargin: 5

            text: pathDelegate.name
            elide: Text.ElideRight
            font.pixelSize: 18
        }

        MouseArea {
            id: thisisdumb
            anchors.fill: parent
            onClicked: {
                tableView.currentIndex = pathDelegate.index
                tableView.currentPathName = pathDelegate.name
            }
        }
    }

    ListModel {
        id: pathsModel
        ListElement {
            index: 0
            name: "Path 1"
        }
        ListElement {
            index: 1
            name: "Path 2"
        }
        ListElement {
            index: 2
            name: "Path 3"
        }
    }
}
