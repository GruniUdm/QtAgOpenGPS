// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import "components" as Comp

Comp.MoveablePopup {
    id: ploughDataPopup
    height: 320 * theme.scaleHeight
    width: 240 * theme.scaleWidth
    visible: false
    modal: false

    // Используем PloughControl напрямую
        property var ploughControl: PloughControl

        // Связываем свойства с PloughControl
        property int currentWidth: ploughControl.currentWidth
        property int targetWidth: ploughControl.targetWidth
        property int deadBand: ploughControl.deadBand
        property bool ploughEnabled: ploughControl.ploughEnabled
        property int ploughMode: ploughControl.ploughMode
        property string ploughModeName: ploughControl.ploughModeName

        // Функция для изменения целевой ширины с шагом
        function changeTargetWidth(delta) {
            var newWidth = targetWidth + delta
            // Ограничиваем значения
            newWidth = Math.max(ploughControl.minWidth, Math.min(newWidth, ploughControl.maxWidth))
            ploughControl.setTargetWidth(newWidth)
        }

        // Подписываемся на изменения свойств PloughControl
        Connections {
            target: ploughControl
            enabled: ploughDataPopup.visible

            function onCurrentWidthChanged() {
                // Обновляем локальные свойства
                currentWidth = ploughControl.currentWidth
            }

            function onTargetWidthChanged() {
                targetWidth = ploughControl.targetWidth
            }

            function onDeadBandChanged() {
                deadBand = ploughControl.deadBand
            }

            function onPloughEnabledChanged() {
                ploughEnabled = ploughControl.ploughEnabled
            }

            function onPloughModeChanged() {
                ploughMode = ploughControl.ploughMode
                ploughModeName = ploughControl.ploughModeName
            }
        }

    Rectangle {
        id: ploughData
        width: parent.width
        height: parent.height
        color: "#4d4d4d"

        Comp.TopLine {
            id: ploughDataTopLine
            onBtnCloseClicked: ploughDataPopup.visible = false
            titleText: qsTr("Plough")
        }

        // Основная табличная структура
        GridLayout {
            id: mainGrid
            anchors.top: ploughDataTopLine.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 5 * theme.scaleWidth

            columns: 4
            rows: 4
            rowSpacing: 5 * theme.scaleHeight
            columnSpacing: 5 * theme.scaleWidth

            // Строка 1: Уставка
            // Текст "Уставка" - занимает 2 колонки, текст по центру
            Rectangle {
                Layout.row: 0
                Layout.column: 0
                Layout.columnSpan: 2
                Layout.preferredWidth: parent.width/2
                Layout.fillHeight: true
                Layout.preferredHeight: 40 * theme.scaleHeight
                color: "transparent"

                Text {
                    anchors.fill: parent
                    text: qsTr("Текущая ширина")
                    font.pixelSize: 18 * theme.scaleHeight
                    color: aogInterface.backgroundColor
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
            }

            // Поле уставки - занимает 2 колонки, текст по центру
            Rectangle {
                id: target
                Layout.row: 0
                Layout.column: 2
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 40 * theme.scaleHeight
                property bool clicked: false
                color: aogInterface.backgroundColor
                border.color: "black"
                radius: 10

                Text {
                    anchors.fill: parent
                    text: Math.round(currentWidth)
                    font.pixelSize: 18 * theme.scaleHeight
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: target.clicked = !target.clicked
                }
            }

            // Строка 2: Расход
            // Текст "Расход" - занимает 2 колонки, текст по центру
            Rectangle {
                Layout.row: 1
                Layout.column: 0
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 40 * theme.scaleHeight
                color: "transparent"

                Text {
                    anchors.fill: parent
                    text: qsTr("Целевая ширина")
                    font.pixelSize: 18 * theme.scaleHeight
                    color: aogInterface.backgroundColor
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
            }

            // Поле расхода - занимает 2 колонки, текст по центру
            Rectangle {
                id: applied
                Layout.row: 1
                Layout.column: 2
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 40 * theme.scaleHeight
                property bool clicked: false
                color: aogInterface.backgroundColor
                border.color: "black"
                radius: 10

                Text {
                    anchors.fill: parent
                    text: Math.round(targetWidth)
                    font.pixelSize: 18 * theme.scaleHeight
                    color: "black"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: applied.clicked = !applied.clicked
                }
            }

            // Строка 3: Применено
            // Текст "Применено" - занимает 2 колонки, текст по центру
            Rectangle {
                Layout.row: 2
                Layout.column: 0
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 40 * theme.scaleHeight
                color: "transparent"

                Text {
                    anchors.fill: parent
                    text: qsTr("Мертвая зона")
                    font.pixelSize: 18 * theme.scaleHeight
                    color: aogInterface.backgroundColor
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
            }

            // Поле примененного количества - занимает 2 колонки, текст по центру
            Rectangle {
                id: qtty
                Layout.row: 2
                Layout.column: 2
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 40 * theme.scaleHeight
                property bool clicked: false
                color: aogInterface.backgroundColor
                border.color: "black"
                radius: 10

                Text {
                    anchors.fill: parent
                    text: Math.round(deadBand)
                    font.pixelSize: 18 * theme.scaleHeight
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: qtty.clicked = !qtty.clicked
                }
            }

            Rectangle {
                            Layout.row: 3
                            Layout.column: 0
                            Layout.columnSpan: 4
                            Layout.fillWidth: true
                            Layout.preferredHeight: 30 * theme.scaleHeight
                            color: "transparent"

                            Text {
                                anchors.fill: parent
                                text: ploughModeName
                                font.pixelSize: 16 * theme.scaleHeight
                                color: getModeColor(ploughMode)
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                wrapMode: Text.WordWrap
                            }

                            function getModeColor(mode) {
                                switch(mode) {
                                case 0: return "red"      // Выкл
                                case 3: return "green"    // Фиксированный
                                case 4: return "blue"     // Расширение
                                case 6: return "orange"   // Сужение
                                case 5: case 7: return "yellow"  // Пределы
                                default: return "white"
                                }
                            }
                        }

            // Строка 4: Нижняя панель
            // Кнопка авто - 1 колонка
            Rectangle {
                id: autoBtn
                Layout.row: 4
                Layout.column: 0
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 40 * theme.scaleHeight
                property bool clicked: false
                color: aogInterface.backgroundColor
                border.color: "black"
                radius: 10

                Text {
                    anchors.fill: parent
                    text: autoBtn.clicked ? qsTr("Man") : qsTr("Auto")
                    font.pixelSize: 18 * theme.scaleHeight
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {autoBtn.clicked = !autoBtn.clicked
                    PloughControl.setPloughEnabled(autoBtn.clicked)}
                }
            }

            // Кнопка уменьшения - 1 колонка
            Comp.IconButtonColor {
                id: rateDown
                Layout.row: 4
                Layout.column: 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 40 * theme.scaleHeight
                icon.source: prefix + "/images/ratec-down.png"
                enabled: true
                onClicked: changeTargetWidth(-10)
            }

            // Кнопка увеличения - 1 колонка
            Comp.IconButtonColor {
                id: rateUp
                Layout.row: 4
                Layout.column: 3
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: 40 * theme.scaleHeight
                icon.source: prefix + "/images/ratec-up.png"
                enabled: true
                onClicked: changeTargetWidth(10)
            }
        }
    }
}
