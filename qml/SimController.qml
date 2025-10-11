// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Sim controller panel on main screen
import QtQuick
import QtQuick.Controls.Fusion
//import Settings
import "components" as Comp
import "../"

Rectangle{
    color: aog.isOutOfBounds ? "darksalmon" : "gray"
    height: 60 * theme.scaleHeight
    width: 650 * theme.scaleWidth
    z: 100
	function changedSteerDir(isRight){
		if(isRight){
			steerSlider.value = steerSlider.value + 10
		}else{
			steerSlider.value = steerSlider.value - 10
		}
	}

    Row{
        spacing: 4 * theme.scaleWidth
        width: childrenRect.width
        height: 50 * theme.scaleHeight
        anchors.centerIn: parent
        Button{
			id: resetButton
            text: qsTr("Reset")
            font.pointSize: 11
            height: parent.height
            width: 65 * theme.scaleWidth
            onClicked: aog.sim_reset()
        }
        Button{
            text: aog.steerAngleActual
            font.pointSize: 11
            height: parent.height
            width: 65 * theme.scaleWidth
            onClicked: steerSlider.value = 300
        }
        Comp.SliderCustomized {
            id: steerSlider
            objectName: "simSteer"
            multiplicationValue: 10
            height: 50 * theme.scaleHeight
			width: 200 * theme.scaleWidth
            from: 0
            to: 600
            value: 300
        }
        Comp.IconButtonTransparent{
            height: parent.height
            width: 65 * theme.scaleWidth
            icon.source: prefix + "/images/DnArrow64.png"
            onClicked: aog.sim_bump_speed(false)
        }
        Comp.IconButtonTransparent{
            height: parent.height
            width: 65 * theme.scaleWidth
            icon.source: prefix + "/images/AutoStop.png"
            onClicked: aog.sim_zero_speed()
        }
        Comp.IconButtonTransparent{
            height: parent.height
            width: 65 * theme.scaleWidth
            icon.source: prefix + "/images/UpArrow64.png"
            onClicked: aog.sim_bump_speed(true)
        }
        Comp.IconButtonTransparent{
            height: parent.height
            width: 65 * theme.scaleWidth
            icon.source: prefix + "/images/YouTurn80.png"
            onClicked: {
                aog.rotateSim() // Qt 6.8 MODERN: Direct Q_INVOKABLE call
                aog.isBtnAutoSteerOn = false; // Qt 6.8 FIX: Use property setter, not method call
            }
        }
    }
}
