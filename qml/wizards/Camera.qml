// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// On main GL
import QtQuick 2.7
import QtQuick.Controls 2.3
import QtMultimedia 5.8

import ".."
import "../components"

MoveablePopup {
    id: camera
    height: 300  * theme.scaleHeight
    width: 500  * theme.scaleWidth
    visible: false
    modal: false
    x: 400 * theme.scaleWidth

    TopLine{
        id: cameraTopLine
        titleText: qsTr("Cam1")
        onBtnCloseClicked:  camera.close()
    }


    Video {
        anchors.top: cameraTopLine.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        id: cam1Stream
        width: 505
        height: 336
        source: "rtsp://192.168.0.138:1945/"
        autoPlay: true
        opacity: 1.0
        fillMode: Image.Stretch
        muted: true
    }

        Timer {
            running: video.status == MediaPlayer.InvalidMedia; repeat: true; interval: 200;
            onTriggered: {
                video.play()
            }
        }



}
