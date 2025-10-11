// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Boundary Interface - Phase 6.0.20 Migration to Q_INVOKABLE + BINDABLE
// Signals removed - now using direct Q_INVOKABLE calls to FormGPS
import QtQuick

Item {
    id: boundaryInterface

    // Properties bindable to FormGPS via Qt 6.8 BINDABLE pattern
    property double createBndOffset: aog.createBndOffset
    property bool isDrawRightSide: aog.isDrawRightSide
    property bool isOutOfBounds: aog.isOutOfBounds
    property bool isHeadlandOn: aog.isHeadlandOn

    // Phase 6.0.20 - New state properties with automatic binding
    property bool isRecording: aog.boundaryIsRecording
    property double area: aog.boundaryArea
    property int pts: aog.boundaryPointCount

    property int count: 0  //updated from formgps_ui_boundary, formgps_saveopen

    // SIGNALS REMOVED - Phase 6.0.20
    // All actions now use Q_INVOKABLE direct calls:
    // - aog.boundaryCalculateArea()
    // - aog.boundaryUpdateList()
    // - aog.boundaryStart()
    // - aog.boundaryStop()
    // - aog.boundaryAddPoint()
    // - aog.boundaryDeleteLastPoint()
    // - aog.boundaryPause()
    // - aog.boundaryRecord()
    // - aog.boundaryReset()
    // - aog.boundaryDeleteBoundary(id)
    // - aog.boundarySetDriveThrough(id, bool)
    // - aog.boundaryDeleteAll()

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
