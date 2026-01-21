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

    property int currentProductId: 0
    property var currentProduct: null

    // Обновляем данные при изменении ID
    onCurrentProductIdChanged: {
        if (RateControl.rcModel) {
            // Обновляем currentProduct
            for (var i = 0; i < RateControl.rcModel.count; i++) {
                var product = RateControl.rcModel.get(i);
                if (product.productId === currentProductId) {
                    currentProduct = product;
                    return;
                }
            }
            currentProduct = null;
        }
    }

    // Также обновляем при показе
    onVisibleChanged: {
        if (visible) {
            currentProductId = 0; // Сбрасываем на первый продукт
        }
    }

    // Функция для получения данных продукта
    function getProductData(id) {
        if (!RateControl.rcModel) return null;
        // Используем встроенный метод модели для получения данных по индексу
        // Нужно найти продукт по ID в модели
        for (var i = 0; i < RateControl.rcModel.count; i++) {
            var product = RateControl.rcModel.get(i);
            if (product.productId === id) {
                return product;
            }
        }
        return null;
    }

    // Текущий продукт
    //property var currentProduct: getProductData(currentProductId)

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
        currentProductId = 0;
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
            checked: currentProductId === 0
            onClicked: currentProductId = 0
        }

        Comp.IconButtonColor{
            id: product2
            checkable: true
            colorChecked: "green"
            icon.source: prefix + "/images/ratec2.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /4 - 5 * theme.scaleWidth
            checked: currentProductId === 1
            onClicked: currentProductId = 1
        }

        Comp.IconButtonColor{
            id: product3
            checkable: true
            colorChecked: "green"
            icon.source: prefix + "/images/ratec3.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /4 - 5 * theme.scaleWidth
            checked: currentProductId === 2
            onClicked: currentProductId = 2
        }

        Comp.IconButtonColor{
            id: product4
            checkable: true
            colorChecked: "green"
            icon.source: prefix + "/images/ratec4.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width /4 - 5 * theme.scaleWidth
            checked: currentProductId === 3
            onClicked: currentProductId = 3
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
                  (currentProduct.productName || "Product " + (currentProductId + 1)) :
                  "N/A"
            font.pixelSize: 16 * theme.scaleHeight; // Еще меньше для надежности
            anchors.centerIn: parent
            color: aog.backgroundColor
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
        onClicked: {
            RateControl.increaseSetRate(currentProductId, 10);
            // Обновляем текущий продукт после изменения
            currentProduct = getProductData(currentProductId);
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
        onClicked: {
            RateControl.decreaseSetRate(currentProductId, 10);
            // Обновляем текущий продукт после изменения
            currentProduct = getProductData(currentProductId);
        }
    }
}
}
