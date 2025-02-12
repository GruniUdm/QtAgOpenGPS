// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
import QtQuick
import Settings

//This is a spinbox for displaying dimensions that are either
//cm or inches

//The from and to values are *cm*, but the final value output is in metres always

Item {
    id: spinBoxCM
    property int from //these are in cm
    property double value //meters
    property int to //centimeters
    property int stepSize: 1
    property bool editable: true
    property string text: ""

    property double boundValue

    signal valueModified()

    width: spinner.width
    height: spinner.height

    //set the spinner value without triggering valueChanged
    function setValue(value) {
        spinner.setValue(utils.cm_to_unit(value))
    }

    onBoundValueChanged: {
        value = boundValue
        spinner.value = utils.cm_to_unit(spinBoxCM.value)
    }

    Connections {
        target: Settings
        function onMenu_isMetricChanged() {
            spinner.value = utils.cm_to_unit(value)
        }
    }

    SpinBoxCustomized {
        id: spinner
		height: 30 * theme.scaleHeight
		width: 150 * theme.scaleWidth
        from: utils.cm_to_unit(spinBoxCM.from / 100.0)
        to: utils.cm_to_unit(spinBoxCM.to / 100.0)
        editable: spinBoxCM.editable
        text: spinBoxCM.text
        value: utils.cm_to_unit(spinBoxCM.value) // should be in metres!
        stepSize: spinBoxCM.stepSize
        anchors.fill: parent

        onValueModified: {
            spinBoxCM.value = utils.cm_from_unit(value)
            spinBoxCM.valueModified()
        }
    }
}
