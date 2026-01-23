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

    // Используем индекс вместо ID
    property int currentProductIndex: 0
    property var currentProduct: null

    // Функция для обновления текущего продукта по индексу
    function updateCurrentProduct() {
        if (RateControl.rcModel && currentProductIndex >= 0 && currentProductIndex < RateControl.rcModel.count) {
            currentProduct = RateControl.rcModel.get(currentProductIndex);
        } else {
            currentProduct = null;
        }
    }

    // Обновляем при изменении индекса
    onCurrentProductIndexChanged: {
        updateCurrentProduct();
    }

    // Обновляем при показе
    onVisibleChanged: {
        if (visible) {
            currentProductIndex = 0;
            updateCurrentProduct();
        }
    }

    // Также обновляем при изменениях в модели
    Connections {
        target: RateControl.rcModel
        onCountChanged: updateCurrentProduct()
        onDataChanged: {
            // Если изменились данные текущего индекса
            if (topLeft.row <= currentProductIndex && bottomRight.row >= currentProductIndex) {
                updateCurrentProduct();
            }
        }
    }

Rectangle{
    id: rcData
    width: 220 * theme.scaleWidth
    height: 220 * theme.scaleHeight
    color: "#4d4d4d"

    onVisibleChanged: {
        if (rcData.visible == true) {
            show()
        }
    }

    function show(){
        currentProductIndex = 0;
        updateCurrentProduct();
    }

    Comp.TopLine{
        id: rcDataTopLine
        onBtnCloseClicked: rcDataPopup.visible = false
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
            checkable: true
            colorChecked: "green"
            icon.source: prefix + "/images/ratec1.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /4 - 5 * theme.scaleWidth
            checked: currentProductIndex === 0
            // Отключаем кнопку, если продукта нет
            enabled: RateControl.rcModel ? RateControl.rcModel.count > 0 : false
            //enabled: RateControl.rcModel.get(0).isActive
            onClicked: currentProductIndex = 0
        }

        Comp.IconButtonColor{
            id: product2
            checkable: true
            colorChecked: "green"
            icon.source: prefix + "/images/ratec2.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /4 - 5 * theme.scaleWidth
            checked: currentProductIndex === 1
            enabled: RateControl.rcModel ? RateControl.rcModel.count > 1 : false
            onClicked: currentProductIndex = 1
        }

        Comp.IconButtonColor{
            id: product3
            checkable: true
            colorChecked: "green"
            icon.source: prefix + "/images/ratec3.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /4 - 5 * theme.scaleWidth
            checked: currentProductIndex === 2
            enabled: RateControl.rcModel ? RateControl.rcModel.count > 2 : false
            onClicked: currentProductIndex = 2
        }

        Comp.IconButtonColor{
            id: product4
            checkable: true
            colorChecked: "green"
            icon.source: prefix + "/images/ratec4.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /4 - 5 * theme.scaleWidth
            checked: currentProductIndex === 3
            enabled: RateControl.rcModel ? RateControl.rcModel.count > 3 : false
            onClicked: currentProductIndex = 3
        }
    }

    ButtonGroup {
        buttons: buttonsMl.children
    }

    RowLayout{
        id: buttonsMl
        visible: true
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
                text: {
                    if (!currentProduct) return "0";
                    var actualRate = currentProduct.productActualRate || 0;
                    var smoothRate = currentProduct.productSmoothRate || 0;
                    return applied1.clicked ?
                           Math.round(actualRate) :
                           Math.round(smoothRate);
                }
                font.pixelSize: 30;
                anchors.centerIn: parent
                color: aogInterface.backgroundColor
            }

            MouseArea{
                id: switcher1
                anchors.fill: applied1
                onClicked: applied1.clicked = !applied1.clicked
            }
        }

        Rectangle{
            id: target1
            color: aogInterface.backgroundColor
            border.color: "black"
            radius: 10
            Layout.alignment: Qt.AlignCenter
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /2 - 5 * theme.scaleWidth

            Text{
                text: currentProduct ?
                      Math.round(currentProduct.productSetRate || 0) :
                      "0"
                font.pixelSize: 30;
                anchors.centerIn: parent
            }
        }
    }

    Rectangle{
        id: productName
        color: "#4d4d4d"
        border.color: "black"
        radius: 10
        implicitHeight: 50 * theme.scaleHeight
        implicitWidth: parent.width /2 - 5 * theme.scaleWidth
        anchors.left: parent.left
        anchors.leftMargin: 5 * theme.scaleHeight
        anchors.bottom: parent.bottom

        Text{
            visible: true
            text: currentProduct ?
                  (currentProduct.productName || "Product " + (currentProductIndex + 1)) :
                  "N/A"
            font.pixelSize: 16 * theme.scaleHeight;
            anchors.centerIn: parent
            color: aogInterface.backgroundColor
        }
    }

    Comp.IconButtonColor{
        id: rateUp
        icon.source: prefix + "/images/ratec-up.png"
        implicitHeight: 50 * theme.scaleHeight
        implicitWidth: 50 * theme.scaleWidth
        anchors.right: parent.right
        anchors.rightMargin: 5 * theme.scaleHeight
        anchors.bottom: parent.bottom
        enabled: currentProductIndex >= 0 && currentProductIndex < 4
        onClicked: {
            // Используем индекс
            RateControl.rcModel.increaseSetRate(currentProductIndex, 10);
            // Обновляем текущий продукт
            updateCurrentProduct();
        }
    }

    Comp.IconButtonColor{
        id: rateDown
        icon.source: prefix + "/images/ratec-down.png"
        implicitHeight: 50 * theme.scaleHeight
        implicitWidth: 50 * theme.scaleWidth
        anchors.right: rateUp.left
        anchors.rightMargin: 5 * theme.scaleHeight
        anchors.left: productName.right
        anchors.leftMargin: 5 * theme.scaleHeight
        anchors.bottom: parent.bottom
        enabled: currentProductIndex >= 0 && currentProductIndex < 4
        onClicked: {
            // Используем индекс
            RateControl.rcModel.decreaseSetRate(currentProductIndex, 10);
            // Обновляем текущий продукт
            updateCurrentProduct();
        }
    }
}
}
