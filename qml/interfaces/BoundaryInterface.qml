// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Interface between formgps_ui_boundary.cpp and frontend boundary stuff
import QtQuick

Item {
    id: boundaryInterface

    //property bool isBndBeingMade: false //may not be needed here
    property double createBndOffset: 0
    property bool isDrawRightSide: true
    property bool isOutOfBounds: false //must be updated from mc.isOutOfBounds
    property bool isHeadlandOn: false

    property int count: 0  //updated from formgps_ui_boundary, formgps_saveopen

    property double area: 0
    property int pts: 0

    //whenever we manually add a point, we need to recalculate area
    //otherwise the main form should do this automatically as we drive
    //should update beingMadeArea and beingMadePts above
    signal calculate_area()
    signal update_list()
    signal start()
    signal stop() //save or cancel boundary being made
    signal add_point() //add the current location as a boundary point
    signal delete_last_point()
    signal pause()
    signal record()
    signal reset()
    signal delete_boundary(int which_boundary)
    signal set_drive_through(int which_boundary, bool through)
    signal delete_all()


    function setIsOutOfBoundsTrue() {
        isOutOfBounds = true;
        console.debug("setting out of bounds.")
    }

    property var boundary_list: [
        { index: 0, area: 75, drive_through: false},
        { index: 1, area: 5, drive_through: false},
        { index: 2, area: 6, drive_through: true}
    ]


}
