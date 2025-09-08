// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Window to set the coordinates of the simulator
import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
//import Settings
import AOG


import ".."
import "../components"

Rectangle{
    id: setColors
    anchors.fill: mainWindow
    color: aog.backgroundColor
    visible: false
    function show(){
        setColors.visible = true
        console.log("showing in setcolors")
    }
    Image { // map image
        id: setColorsImage
        source: prefix + "/images/ColorBackGnd.png"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.margins: 50
        //width: parent.width *.9
        //fillMode: Image.PreserveAspectFit

    }

    ColorDialog{//color picker
        id: cpFieldColor
        onSelectedColorChanged: {
            //colorFieldDay=@Variant(\0\0\0\x43\x1\xff\xff\x91\x91\x91\x91\x91\x91\0\0)
            //colorFieldNight=@Variant(\0\0\0\x43\x1\xff\xff<<<<<<\0\0)
            //just use the Day setting. AOG has them locked to the same color anyways
            //Settings.display_colorFieldDay = cpFieldColor.selectedColor;
            if (btnDayNight.checked){Settings.display_colorFieldNight = cpFieldColor.selectedColor;}
            else {Settings.display_colorFieldDay = cpFieldColor.selectedColor;}

            //change the color on the fly. In AOG, we had to cycle the sections off
            //and back on. This does for us.
        }
    }
    RowLayout{
        id: buttontop
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10
        height: children.height
        width: parent.width *.3
        IconButtonColor {
            id: btnColorField
            icon.source: prefix + "/images/ColorField.png"
            onClicked: cpFieldColor.open()
            color: btnDayNight.checked?Settings.display_colorFieldNight:Settings.display_colorFieldDay
        }
        IconButtonColor {
            id: btnDayNight
            icon.source: prefix + "/images/Config/ConD_AutoDayNight.png"
            checkable: true
            color1: "blue"
            colorChecked1: "black"
            colorChecked2: "black"
            colorChecked3: "black"
            onVisibleChanged: btnDayNight.checked = false
        }
    }

    RowLayout{
        id: buttons
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        height: children.height
        width: parent.width *.3
        IconButtonTransparent{
            icon.source: prefix + "/images/Cancel64.png"
            onClicked: setColors.visible = false
        }
        IconButtonTransparent{
            icon.source: prefix + "/images/OK64.png"
            //anchors.bottom: parent.bottom
            //anchors.right: parent.right
            onClicked: {
                aog.settings_save()
                aog.settings_reload()
                setColors.visible = false
            }
        }
    }
    
}

