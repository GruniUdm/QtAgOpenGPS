// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Displays the GPS Data on main screen.
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import "components" as Comp

Comp.MoveablePopup {
    id: rcDataPopup
    height: 220  * theme.scaleHeight
    width: 220  * theme.scaleWidth
    visible: false
    modal: false
Rectangle{
    id: rcData
    width: 220 * theme.scaleWidth
    height: 220 * theme.scaleHeight
    color: "#4d4d4d"
    property double errRate: aog.actualRate0;
    onVisibleChanged: { if (rcData.visible == true) show()}
    function show(){
        product1.isChecked = true
    }

    Comp.TopLine{
        id: rcDataTopLine
        onBtnCloseClicked:  rcDataPopup.visible  = false
        titleText: qsTr("RC")
    }
    ButtonGroup {
        buttons: buttonsTop.children
    }
    RowLayout{
        id: buttonsTop
        anchors.top: rcDataTopLine.bottom
        anchors.topMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 10 * theme.scaleWidth
        Comp.IconButtonColor{
            id: product1
            checkable: aog.product0stat
            //checked: true
            colorChecked: "green"
            icon.source: prefix + "/images/ratec1.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /4 - 5 * theme.scaleWidth
            visible: true
        }

        Comp.IconButtonColor{
            id: product2
            checkable: aog.product1stat
            colorChecked: "green"
            icon.source: prefix + "/images/ratec2.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /4 - 5 * theme.scaleWidth
            visible: true
        }
        Comp.IconButtonColor{
            id: product3
            checkable: aog.product2stat
            colorChecked: "green"
            icon.source: prefix + "/images/ratec3.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /4 - 5 * theme.scaleWidth
            visible: true
        }
        Comp.IconButtonColor{
            id: product4
            checkable: aog.product3stat
            colorChecked: "green"
            icon.source: prefix + "/images/ratec4.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /4 - 5 * theme.scaleWidth
            visible: true
        }
    }
    ButtonGroup {
        buttons: buttonsMl.children
    }
    RowLayout{
        id: buttonsMl
        visible: product1.checked
        anchors.top: buttonsTop.bottom
        anchors.topMargin: 17 * theme.scaleHeight
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 10 * theme.scaleWidth
        Rectangle{
            id: applied1
            property bool clicked: false;
            color: "#4d4d4d"
            border.color: "black"
            radius: 10
            Layout.alignment: Qt.AlignCenter
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /2 - 5 * theme.scaleWidth
            Text{
                text: applied1.clicked?Math.round(aog.actualRate0, 0):Math.round(aog.smoothRate0, 0)
                font.pixelSize: 30;
                anchors.centerIn: parent
                color: aog.backgroundColor
            }
            MouseArea{
                id: switcher1
                anchors.fill: applied1
                onClicked: applied1.clicked = !applied1.clicked
            }
        }
        Rectangle{
            id: target1
            color: aog.backgroundColor
            border.color: "black"
            radius: 10
            Layout.alignment: Qt.AlignCenter
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /2 - 5 * theme.scaleWidth
            Text{
                text: Settings.rate_Product0[10]
                font.pixelSize: 30;
                anchors.centerIn: parent

            }
        }

    }
    RowLayout{
        id: buttonsM2
        visible: product2.checked
        anchors.top: buttonsTop.bottom
        anchors.topMargin: 17 * theme.scaleHeight
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 10 * theme.scaleWidth
        Rectangle{
            id: applied2
            property bool clicked: false;
            color: "#4d4d4d"
            border.color: "black"
            radius: 10
            Layout.alignment: Qt.AlignCenter
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /2 - 5 * theme.scaleWidth
            Text{
                text: applied2.clicked?Math.round(aog.actualRate1, 0):Math.round(aog.smoothRate1, 0)
                font.pixelSize: 30;
                anchors.centerIn: parent
                color: aog.backgroundColor
            }
            MouseArea{
                anchors.fill: applied2
                onClicked: applied2.clicked = !applied2.clicked
            }
        }
        Rectangle{
            id: target2
            color: aog.backgroundColor
            border.color: "black"
            radius: 10
            Layout.alignment: Qt.AlignCenter
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /2 - 5 * theme.scaleWidth
            Text{
                text: Settings.rate_Product1[10]
                font.pixelSize: 30;
                anchors.centerIn: parent

            }
        }

    }
    RowLayout{
        id: buttonsM3
        visible: product3.checked
        anchors.top: buttonsTop.bottom
        anchors.topMargin: 17 * theme.scaleHeight
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 10 * theme.scaleWidth
        Rectangle{
            id: applied3
            property bool clicked: false;
            color: "#4d4d4d"
            border.color: "black"
            radius: 10
            Layout.alignment: Qt.AlignCenter
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /2 - 5 * theme.scaleWidth
            Text{
                text: applied3.clicked?Math.round(aog.actualRate2, 0):Math.round(aog.smoothRate2, 0)
                font.pixelSize: 30;
                anchors.centerIn: parent
                color: aog.backgroundColor
            }
            MouseArea{
                anchors.fill: applied3
                onClicked: applied3.clicked = !applied3.clicked
            }
        }
        Rectangle{
            id: target3
            color: aog.backgroundColor
            border.color: "black"
            radius: 10
            Layout.alignment: Qt.AlignCenter
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /2 - 5 * theme.scaleWidth
            Text{
                text: Settings.rate_Product2[10]
                font.pixelSize: 30;
                anchors.centerIn: parent

            }
        }

    }
    RowLayout{
        id: buttonsM4
        visible: product4.checked
        anchors.top: buttonsTop.bottom
        anchors.topMargin: 17 * theme .scaleHeight
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 10 * theme.scaleWidth
        Rectangle{
            id: applied4
            property bool clicked: false;
            color: "#4d4d4d"
            border.color: "black"
            radius: 10
            Layout.alignment: Qt.AlignCenter
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /2 - 5 * theme.scaleWidth
            Text{
                text: applied4.clicked?Math.round(aog.actualRate3, 0):Math.round(aog.smoothRate3, 0)
                font.pixelSize: 30;
                anchors.centerIn: parent
                color: aog.backgroundColor
            }
            MouseArea{
                anchors.fill: applied4
                onClicked: applied4.clicked = !applied4.clicked
            }
        }
        Rectangle{
            id: target4
            color: aog.backgroundColor
            border.color: "black"
            radius: 10
            Layout.alignment: Qt.AlignCenter
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /2 - 5 * theme.scaleWidth
            Text{
                text: Settings.rate_Product3[10]
                font.pixelSize: 30;
                anchors.centerIn: parent

            }
        }

    }
    Rectangle{
        id: productName
        property bool clicked: false;
        color: "#4d4d4d"
        border.color: "black"
        radius: 10
        Layout.alignment: Qt.AlignCenter
        implicitHeight: 50 * theme.scaleHeight
        implicitWidth: parent.width /2 - 5 * theme.scaleWidth
        anchors.left: parent.left
        anchors.leftMargin: 5 * theme.scaleHeight
        anchors.bottom: parent.bottom
        Text{
            visible: product1.checked
            text: Settings.rate_ProductName[0]
            font.pixelSize: 30* theme.scaleHeight;
            anchors.centerIn: parent
            color: aog.backgroundColor
        }
        Text{
            visible: product2.checked
            text: Settings.rate_ProductName[1]
            font.pixelSize: 30* theme.scaleHeight;
            anchors.centerIn: parent
            color: aog.backgroundColor
        }
        Text{
            visible: product3.checked
            text: Settings.rate_ProductName[2]
            font.pixelSize: 30* theme.scaleHeight;
            anchors.centerIn: parent
            color: aog.backgroundColor
        }
        Text{
            visible: product4.checked
            text: Settings.rate_ProductName[3]
            font.pixelSize: 30* theme.scaleHeight;
            anchors.centerIn: parent
            color: aog.backgroundColor
        }
    }

    Comp.IconButtonColor{
        id: rateUp
        checkable: true
        colorChecked: "lightgray"
        icon.source: prefix + "/images/ratec-up.png"
        implicitHeight: 50 * theme.scaleHeight
        implicitWidth: 50 * theme.scaleWidth
        anchors.right: parent.right
        anchors.rightMargin: 5 * theme.scaleHeight
        anchors.bottom: parent.bottom
        onClicked: { if (product1.checked == true) {Settings.rate_Product0[10]>500?Settings.rate_Product0[10]=500:Settings.rate_Product0[10]+=10}
            else if  (product2.checked == true) {Settings.rate_Product1[10]>500?Settings.rate_Product1[10]=500:Settings.rate_Product1[10]+=10}
            else if  (product3.checked == true) {Settings.rate_Product2[10]>500?Settings.rate_Product2[10]=500:Settings.rate_Product2[10]+=10}
            else if  (product4.checked == true) {Settings.rate_Product3[10]>500?Settings.rate_Product3[10]=500:Settings.rate_Product3[10]+=10}
        }
    }

    Comp.IconButtonColor{
        id: rateDown
        checkable: true
        colorChecked: "lightgray"
        icon.source: prefix + "/images/ratec-down.png"
        implicitHeight: 50 * theme.scaleHeight
        implicitWidth: 50 * theme.scaleWidth
        anchors.right: rateUp.left
        anchors.rightMargin: 5 * theme.scaleHeight
        anchors.left: productName.right
        anchors.leftMargin: 5 * theme.scaleHeight
        anchors.bottom: parent.bottom
        onClicked: { if (product1.checked == true) {Settings.rate_Product0[10]<10?Settings.rate_Product0[10]=0:Settings.rate_Product0[10]-=10}
            else   if (product2.checked == true) {Settings.rate_Product1[10]<10?Settings.rate_Product1[10]=0:Settings.rate_Product1[10]-=10}
            else   if (product3.checked == true) {Settings.rate_Product2[10]<10?Settings.rate_Product2[10]=0:Settings.rate_Product2[10]-=10}
            else   if (product4.checked == true) {Settings.rate_Product3[10]<10?Settings.rate_Product3[10]=0:Settings.rate_Product3[10]-=10}
        }
    }

    function errormessage() { if (aog.actualRate0 < Settings.rate_Product0[10]*0.9 & aog.product0stat & Settings.rate_Product0[2]){
            timedMessage.addMessage(2000, qsTr("Low rate. Increase speed!"));}
        else if (aog.actualRate0 > Settings.rate_Product0[10] * 1.1 & aog.product0stat & Settings.rate_Product0[2]){
            timedMessage.addMessage(2000, qsTr("High rate. Reduce speed!"));}
    }

}
}
