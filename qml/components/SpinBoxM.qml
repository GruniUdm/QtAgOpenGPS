// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
import QtQuick
import QtQuick.Controls
import Settings
import AOG

//This is a spinbox for displaying dimensions that are either
//cm or inches

//The from and to values are *cm*, but the final value output is in metres always

Item {
    id: spinBoxM
    property int from //these are in meters
    property double value //meters
    property int to //meters
    property int stepSize: 1
    property bool editable: true
    property string text: ""
	property int decimals: 0
	property int decimalFactor: 1

	onDecimalsChanged: {
		if (decimals > 0)
			decimalFactor = Math.pow(10, decimals)
		else
			decimalFactor = 1
	}

    property double boundValue

    signal valueModified()

    width: spinner.width
    height: spinner.height

    //set the spinner value without triggering valueChanged
    function setValue(value) {
        spinner.setValue(Utils.m_to_unit(value))
    }

    onBoundValueChanged: {
        spinner.value = Utils.m_to_unit(boundValue)
        value = boundValue
    }

    Connections {
        target: Settings
        function onMenu_isMetricChanged() {
            spinner.value = Utils.m_to_unit(value)
        }
    }

    SpinBoxCustomized{
        id: spinner
		decimals: spinBoxM.decimals
        from: Utils.m_to_unit(spinBoxM.from)
        to: Utils.m_to_unit(spinBoxM.to)
        editable: spinBoxM.editable
        value: Utils.m_to_unit(spinBoxM.value) // should be in metres!
        stepSize: spinBoxM.stepSize
        anchors.fill: parent


        onValueModified: {
            spinBoxM.value = Utils.m_from_unit(value)
            spinBoxM.valueModified()
        }
    }
}
