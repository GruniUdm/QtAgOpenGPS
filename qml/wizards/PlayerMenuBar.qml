// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Settings
import QtMultimedia

Item {
    id: root

    required property MediaPlayer mediaPlayer
    required property VideoOutput videoOutput
    property string cam1:"rtsp://192.168.0.138:1945/"
    property bool autoload: false
    height: menuBar.height

    signal closePlayer
    onAutoloadChanged: autoload===true?loadCam1():0
    function loadUrl(url) {
        mediaPlayer.stop()
        mediaPlayer.source = url
        mediaPlayer.play()
    }

    function loadCam1() {
        cam1 = Settings.cam_camLink
        mediaPlayer.stop()
        mediaPlayer.source = cam1
        mediaPlayer.play()
    }


    function showOverlay(overlay){
        closeOverlays();
        overlay.visible = true;
    }

    Popup {
        id: urlPopup
        anchors.centerIn: Overlay.overlay

        RowLayout {
            id: rowOpenUrl
            Label {
                text: qsTr("URL:");
            }

            TextInput {
                id: urlText
                focus: true
                Layout.minimumWidth: 400
                wrapMode: TextInput.WrapAnywhere
                Keys.onReturnPressed: { loadUrl(text); urlText.text = ""; urlPopup.close() }
            }

            Button {
                text: "Load"
                onClicked: { Settings.cam_camLink = urlText.text; loadUrl(urlText.text); urlText.text = ""; urlPopup.close() }
            }
        }
        onOpened: { urlPopup.forceActiveFocus() }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        onAccepted: {
            mediaPlayer.stop()
            mediaPlayer.source = fileDialog.currentFile
            mediaPlayer.play()
        }
    }

    MenuBar {
        id: menuBar
        anchors.left: parent.left
        anchors.right: parent.right

        Menu {
            title: qsTr("&File")
            Action {
                text: qsTr("&Open")
                onTriggered: fileDialog.open()
            }
            Action {
                text: qsTr("&URL");
                onTriggered: urlPopup.open()
            }

            Action {
                text: qsTr("&cam1");
                onTriggered: loadCam1()
            }

            Action {
                text: qsTr("&Exit");
                onTriggered: closePlayer()
            }
        }

    }
}

