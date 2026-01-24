// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import "components" as Comp

Comp.MoveablePopup {
    id: rcDataPopup
    height: 220 * theme.scaleHeight
    width: 220 * theme.scaleWidth
    visible: false
    modal: false

    property int currentProductIndex: 0
    property var rcModel: RateControl.rcModel

    // Свойства для данных текущего продукта (будем обновлять их через таймер)
    property double currentSmoothRate: 0
    property double currentActualRate: 0
    property double currentAppliedlRate: 0
    property double currentSetRate: 0
    property string currentProductName: ""
    property bool currentProductActive: false

    // Свойства для активности кнопок
    property bool product1Active: false
    property bool product2Active: false
    property bool product3Active: false
    property bool product4Active: false

    // Функция для обновления всех данных
    function updateAllData() {
        if (!rcModel) return;

        // Обновляем данные текущего продукта
        if (currentProductIndex >= 0 && currentProductIndex < rcModel.count) {
            var data = rcModel.get(currentProductIndex);
            if (data) {
                currentSmoothRate = data.productSmoothRate || 0;
                currentActualRate = data.productActualRate || 0;
                currentAppliedlRate = data.productAppliedRate || 0;
                currentSetRate = data.productSetRate || 0;
                currentProductName = data.productName || ("Product " + (currentProductIndex + 1));
                currentProductActive = data.productIsActive || false;
            }
        }

        // Обновляем активности кнопок
        for (var i = 0; i < 4 && i < rcModel.count; i++) {
            var productData = rcModel.get(i);
            var isActive = productData ? productData.productIsActive || false : false;

            switch(i) {
            case 0: product1Active = isActive; break;
            case 1: product2Active = isActive; break;
            case 2: product3Active = isActive; break;
            case 3: product4Active = isActive; break;
            }
        }
    }

    // Подписываемся на изменения модели
    Connections {
        target: rcModel
        enabled: rcDataPopup.visible

        function onDataChanged(topLeft, bottomRight, roles) {
            // Если изменился текущий продукт, обновляем данные
            if (topLeft.row <= currentProductIndex && bottomRight.row >= currentProductIndex) {
                updateAllData();
            }
            // Всегда обновляем активности
            updateAllData();
        }

        function onCountChanged() {
            updateAllData()
        }
    }

    // Обновляем данные при изменении индекса
    onCurrentProductIndexChanged: updateAllData()

    // Обновляем при показе
    onVisibleChanged: {
        if (visible) {
            currentProductIndex = 0;
            updateAllData();
        }
    }

    Rectangle {
        id: rcData
        width: 220 * theme.scaleWidth
        height: 220 * theme.scaleHeight
        color: "#4d4d4d"

        Comp.TopLine {
            id: rcDataTopLine
            onBtnCloseClicked: rcDataPopup.visible = false
            titleText: qsTr("RC")
        }

        ButtonGroup {
            buttons: buttonsTop.children
        }

        RowLayout {
            id: buttonsTop
            anchors.top: rcDataTopLine.bottom
            anchors.topMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 10 * theme.scaleWidth

            Comp.IconButtonColor {
                id: product1
                checkable: true
                colorChecked: product1Active?"green":"grey"
                icon.source: prefix + "/images/ratec1.png"
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: parent.width / 4 - 5 * theme.scaleWidth
                checked: currentProductIndex === 0
                //enabled: product1Active
                enabled: SettingsManager.rate_confProduct0[2]
                onClicked: currentProductIndex = 0

            }

            Comp.IconButtonColor {
                id: product2
                checkable: true
                colorChecked: product2Active?"green":"grey"
                icon.source: prefix + "/images/ratec2.png"
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: parent.width / 4 - 5 * theme.scaleWidth
                checked: currentProductIndex === 1
                //enabled: product2Active
                enabled: SettingsManager.rate_confProduct1[2]
                onClicked: currentProductIndex = 1
            }

            Comp.IconButtonColor {
                id: product3
                checkable: true
                colorChecked: product3Active?"green":"grey"
                icon.source: prefix + "/images/ratec3.png"
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: parent.width / 4 - 5 * theme.scaleWidth
                checked: currentProductIndex === 2
                enabled: SettingsManager.rate_confProduct2[2]
                onClicked: currentProductIndex = 2
            }

            Comp.IconButtonColor {
                id: product4
                checkable: true
                colorChecked: product4Active?"green":"grey"
                icon.source: prefix + "/images/ratec4.png"
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: parent.width / 4 - 5 * theme.scaleWidth
                checked: currentProductIndex === 3
                enabled: SettingsManager.rate_confProduct3[2]
                onClicked: currentProductIndex = 3
            }
        }

        RowLayout {
            id: buttonsMl
            anchors.top: buttonsTop.bottom
            anchors.topMargin: 17 * theme.scaleHeight
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 10 * theme.scaleWidth

            Rectangle {
                id: applied
                property bool clicked: false
                color: "#4d4d4d"
                border.color: "black"
                radius: 10
                Layout.alignment: Qt.AlignCenter
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: parent.width / 2 - 5 * theme.scaleWidth

                Text {
                    text: applied.clicked ? Math.round(currentActualRate) : Math.round(currentSmoothRate)
                    font.pixelSize: 30
                    anchors.centerIn: parent
                    color: aogInterface.backgroundColor
                }

                MouseArea {
                    id: switcher1
                    anchors.fill: applied
                    onClicked: applied.clicked = !applied.clicked
                }
            }

            Rectangle {
                id: target
                property bool clicked: false
                color: aogInterface.backgroundColor
                border.color: "black"
                radius: 10
                Layout.alignment: Qt.AlignCenter
                implicitHeight: 50 * theme.scaleHeight
                implicitWidth: parent.width / 2 - 5 * theme.scaleWidth

                Text {
                    text: target.clicked ? Math.round(currentAppliedlRate) : Math.round(currentSetRate)
                    font.pixelSize: 30
                    anchors.centerIn: parent
                }

                MouseArea {
                    id: switcher2
                    anchors.fill: target
                    onClicked: target.clicked = !target.clicked
                }
            }
        }

        Rectangle {
            id: productNameRect
            color: "#4d4d4d"
            border.color: "black"
            radius: 10
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: parent.width / 2 - 5 * theme.scaleWidth
            anchors.left: parent.left
            anchors.leftMargin: 5 * theme.scaleHeight
            anchors.bottom: parent.bottom

            Text {
                text: currentProductName
                font.pixelSize: 16 * theme.scaleHeight
                anchors.centerIn: parent
                color: aogInterface.backgroundColor
            }
        }

        Comp.IconButtonColor {
            id: rateUp
            icon.source: prefix + "/images/ratec-up.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: 50 * theme.scaleWidth
            anchors.right: parent.right
            anchors.rightMargin: 5 * theme.scaleHeight
            anchors.bottom: parent.bottom
            enabled: currentProductIndex >= 0 && currentProductIndex < 4 && currentProductActive
            onClicked: RateControl.increaseSetRate(currentProductIndex, 10);
        }

        Comp.IconButtonColor {
            id: rateDown
            icon.source: prefix + "/images/ratec-down.png"
            implicitHeight: 50 * theme.scaleHeight
            implicitWidth: 50 * theme.scaleWidth
            anchors.right: rateUp.left
            anchors.rightMargin: 5 * theme.scaleHeight
            anchors.left: productNameRect.right
            anchors.leftMargin: 5 * theme.scaleHeight
            anchors.bottom: parent.bottom
            enabled: currentProductIndex >= 0 && currentProductIndex < 4 && currentProductActive
            onClicked: RateControl.decreaseSetRate(currentProductIndex, 10);
        }
    }
}
