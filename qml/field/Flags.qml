// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Windows for creating new tracks
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
//import AOG

import ".."
import "../components" as Comp

Comp.MoveablePopup {
    function show(){
        flags.visible = true
    }
    id: flags
    x: 40
    y: 40
    height: 320  * theme.scaleHeight
    width: 250  * theme.scaleWidth

    property double dist: 0

    function update_model() {
        //var distance = 0.0
        if (contextFlag.ptlat>0) dist = Utils.distanceLatLon(aog.latitude, aog.longitude, contextFlag.ptlat, contextFlag.ptlon);
        else dist = 0;
        }

        Comp.TitleFrame{
        color: "#f2f2f2"
        border.color: "black"
        border.width: 2
        title: qsTr("Flags")
        anchors.fill: parent

        RowLayout{
            id: buttonsBottom
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 10 * theme.scaleWidth

            Comp.IconButtonColor{
                id: up
                icon.source: prefix + "/images/UpArrow64.png"
                implicitWidth: parent.width /4 - 5 * theme.scaleWidth
                implicitHeight: theme.buttonSize
                color: "#ffffff"
                onClicked:{ aog.btnNextFlag()
                update_model()}
            }
            Comp.IconButtonColor{
                id: down
                icon.source: prefix + "/images/DnArrow64.png"
                implicitWidth: parent.width /4 - 5 * theme.scaleWidth
                implicitHeight: theme.buttonSize
                color: "#ffffff"
                onClicked: {aog.btnPrevFlag()
                update_model()}
            }
            Comp.IconButtonColor{
                id: deleteflag
                icon.source: prefix + "/images/FlagDelete.png"
                implicitWidth: parent.width /4 - 5 * theme.scaleWidth
                implicitHeight: theme.buttonSize
                color: "#ffffff"
                onClicked: aog.btnDeleteFlag()
            }
            Comp.IconButtonColor{
                id: cancel
                icon.source: prefix + "/images/Cancel64.png"
                implicitWidth: parent.width /4 - 5 * theme.scaleWidth
                implicitHeight: theme.buttonSize
                color: "#ffffff"
                onClicked: {flags.visible = false;
                    aog.btnCancelFlag()}
            }
        }
            Rectangle{
                id: textEntry
                width:parent.width*0.75
                height: 50  * theme.scaleHeight
                anchors.top:parent.top
                anchors.topMargin: 20 * theme.scaleHeight
                anchors.horizontalCenter: parent.horizontalCenter
                color: aog.backgroundColor
                border.color: "darkgray"
                border.width: 1
                TextField{
                    id: newField
                    anchors.fill: parent
                    selectByMouse: true
                    placeholderText: focus || text ? "" : contextFlag.ptText
                    font.pointSize: 15
                }
                Text {
                    id: idText
                    anchors.top: textEntry.bottom
                    anchors.left: parent.left
                    //color: "red"
                    visible: true
                    text: qsTr("ID: ") + contextFlag.ptId
                    font.pointSize: 15
                }
                Text {
                    id: latText
                    anchors.top: idText.bottom
                    anchors.left: parent.left
                    //color: "red"
                    visible: true
                    text: qsTr("Lat: ") + (Number(contextFlag.ptlat).toLocaleString(Qt.locale(), 'f', 9))
                    onTextChanged: update_model()
                    font.pointSize: 15
                }
                Text {
                    id: lonText
                    anchors.top: latText.bottom
                    anchors.left: parent.left
                    //color: "red"
                    visible: true
                    text: qsTr("Lon: ") + (Number(contextFlag.ptlon).toLocaleString(Qt.locale(), 'f', 9))
                    onTextChanged: update_model()
                    font.pointSize: 15
                }
                Text {
                    anchors.top: textEntry.bottom
                    anchors.left: parent.left
                    //color: "red"
                    visible: false
                    text: aog.longitude
                    onTextChanged: update_model()
                }
                Text {
                    anchors.top: textEntry.bottom
                    anchors.right: parent.right
                    //color: "red"
                    visible: false
                    text: aog.latitude
                    onTextChanged: update_model()
                }
                Text {
                    id: distText
                    anchors.top: lonText.bottom
                    anchors.left: parent.left
                    //color: "red"
                    visible: true
                    text: qsTr("Dist: ") + Math.round(dist*100)/100 +" " + Utils.m_unit_abbrev()
                }
                Text {
                    id: errorMessage
                    anchors.top: newField.bottom
                    anchors.left: newField.left
                    color: "red"
                    visible: false
                    text: qsTr("This flag exists already; please choose another name.")
                }
            }
        }
    }
