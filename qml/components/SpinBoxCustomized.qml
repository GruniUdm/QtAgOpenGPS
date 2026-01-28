// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Spinbox, just customized. Allows the use of decimals
import QtQuick
import QtQuick.Controls.Fusion

Item {
    id: spinBox_Customized
    property double from
    property double to
    property string text: ""
    property int stepSize: 1
    property bool editable: true
    property double boundValue
    property int fontPixelSize: 16
    property int decimals: 0
    property int decimalFactor: 1
    property bool enabled: true  // Добавлено отдельное свойство для включения/выключения

    onDecimalsChanged: {
        if (decimals > 0)
            decimalFactor = Math.pow(10, decimals)
        else
            decimalFactor = 1
    }

    // Основное свойство значения
    property double value: 0

    signal valueModified()
    signal editingFinished()

    width: spinner.width
    height: spinner.height + (spin_text.visible ? spin_text.height : 0) + (spin_message.visible ? spin_message.height : 0)

    // Метод для программной установки значения с учетом decimalFactor
    function setSpinValue(newValue) {
        if (newValue !== value) {
            value = newValue
            boundValue = newValue
            spinner.internalValue = newValue * decimalFactor
            spinner.updateTextFromValue()
            valueModified()
        }
    }

    // Метод для сброса значения
    function resetValue() {
        setSpinValue(0)
    }

    // Синхронизация boundValue -> value -> spinner
    onBoundValueChanged: {
        if (boundValue !== value) {
            value = boundValue
            spinner.internalValue = boundValue * decimalFactor
            spinner.updateTextFromValue()
        }
    }

    // Синхронизация value -> boundValue -> spinner
    onValueChanged: {
        if (value !== boundValue) {
            boundValue = value
        }
        // Всегда обновляем спиннер
        spinner.internalValue = value * decimalFactor
        spinner.updateTextFromValue()
    }

    // Кастомный спинбокс без использования стандартного SpinBox
    Rectangle {
        id: spinner
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        height: 40 * theme.scaleHeight
        width: Math.max(150 * theme.scaleWidth, 150 * theme.scaleWidth) // Фиксированная ширина
        border.color: text_input.activeFocus ? "#4a90e2" : "#cccccc"
        border.width: text_input.activeFocus ? 2 : 1
        radius: 3
        color: spinBox_Customized.enabled ? "white" : "#f0f0f0"

        // Объединяем editable и enabled: компонент активен только если оба true
        property bool isActive: spinBox_Customized.editable && spinBox_Customized.enabled

        property int internalValue: spinBox_Customized.value * decimalFactor
        property int from: spinBox_Customized.from * decimalFactor
        property int to: spinBox_Customized.to * decimalFactor
        property int stepSize: spinBox_Customized.stepSize * decimalFactor
        property int decimals: spinBox_Customized.decimals

        // Функция для форматирования значения
        function formatValue(value) {
            return Number(value / decimalFactor).toLocaleString(Qt.locale(), 'f', decimals)
        }

        // Обновление текста из значения
        function updateTextFromValue() {
            if (!text_input.activeFocus) {
                text_input.text = formatValue(internalValue)
            }
        }

        // Кнопка увеличения
        Rectangle {
            id: upButton
            anchors {
                right: parent.right
                top: parent.top
                bottom: parent.bottom
            }
            width: 40 * theme.scaleWidth
            color: upMouseArea.pressed ? "#e6e6e6" : "#f6f6f6"
            // Темная рамка когда enabled (#333333), светлая когда disabled (#e0e0e0)
            border.color: spinner.isActive ? "#333333" : "#e0e0e0"
            border.width: 1

            MouseArea {
                id: upMouseArea
                anchors.fill: parent
                hoverEnabled: true
                enabled: spinner.isActive // Кнопка активна только когда спинбокс активен
                onClicked: {
                    if (spinner.isActive) {
                        var newValue = spinner.internalValue + spinner.stepSize
                        if (newValue <= spinner.to) {
                            spinner.internalValue = newValue
                            spinner.updateTextFromValue()
                            spinBox_Customized.updateValueFromSpinner()
                        }
                    }
                }
                onEntered: {
                    if (spinner.isActive) {
                        parent.color = "#e6e6e6"
                    }
                }
                onExited: {
                    if (spinner.isActive) {
                        parent.color = upMouseArea.pressed ? "#e6e6e6" : "#f6f6f6"
                    }
                }
            }

            Text {
                text: "+"
                font.pixelSize: 14 * theme.scaleHeight
                // Черный текст когда enabled, серый когда disabled
                color: spinner.isActive ? "#333333" : "#888888"
                anchors.centerIn: parent
            }
        }

        // Кнопка уменьшения
        Rectangle {
            id: downButton
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
            }
            width: 40 * theme.scaleWidth
            color: downMouseArea.pressed ? "#e6e6e6" : "#f6f6f6"
            // Темная рамка когда enabled (#333333), светлая когда disabled (#e0e0e0)
            border.color: spinner.isActive ? "#333333" : "#e0e0e0"
            border.width: 1

            MouseArea {
                id: downMouseArea
                anchors.fill: parent
                hoverEnabled: true
                enabled: spinner.isActive // Кнопка активна только когда спинбокс активен
                onClicked: {
                    if (spinner.isActive) {
                        var newValue = spinner.internalValue - spinner.stepSize
                        if (newValue >= spinner.from) {
                            spinner.internalValue = newValue
                            spinner.updateTextFromValue()
                            spinBox_Customized.updateValueFromSpinner()
                        }
                    }
                }
                onEntered: {
                    if (spinner.isActive) {
                        parent.color = "#e6e6e6"
                    }
                }
                onExited: {
                    if (spinner.isActive) {
                        parent.color = downMouseArea.pressed ? "#e6e6e6" : "#f6f6f6"
                    }
                }
            }

            Text {
                text: "–"
                font.pixelSize: 14 * theme.scaleHeight
                // Черный текст когда enabled, серый когда disabled
                color: spinner.isActive ? "#333333" : "#888888"
                anchors.centerIn: parent
            }
        }

        // Поле ввода текста
        TextInput {
            id: text_input
            anchors {
                left: downButton.right
                right: upButton.left
                top: parent.top
                bottom: parent.bottom
                margins: 2
            }
            text: spinner.formatValue(spinner.internalValue)
            font.pixelSize: spinBox_Customized.fontPixelSize
            // Темный текст когда enabled, серый когда disabled
            color: spinner.isActive ? "#333333" : "#888888"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            readOnly: !spinner.isActive
            enabled: spinner.isActive // Поле ввода активно только когда спинбокс активен
            validator: DoubleValidator {
                bottom: spinBox_Customized.from
                top: spinBox_Customized.to
                decimals: spinBox_Customized.decimals
                notation: DoubleValidator.StandardNotation
            }
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            selectByMouse: true

            // При завершении редактирования
            onEditingFinished: {
                spinBox_Customized.editingFinished()
                spinBox_Customized.updateValueFromText()
            }

            // При нажатии Enter
            Keys.onReturnPressed: {
                focus = false
                spinBox_Customized.updateValueFromText()
                spinBox_Customized.editingFinished()
            }

            Keys.onEscapePressed: {
                focus = false
                text = spinner.formatValue(spinner.internalValue) // Восстанавливаем предыдущее значение
            }

            // При изменении фокуса
            onActiveFocusChanged: {
                if (!activeFocus) {
                    spinBox_Customized.updateValueFromText()
                }
            }
        }

        // Обновление значения при изменении internalValue
        onInternalValueChanged: {
            spinner.updateTextFromValue()
        }
    }

    // Функция для обновления значения из текстового поля
    function updateValueFromText() {
        // Проверяем, активен ли спинбокс
        if (!spinner.isActive) {
            return
        }

        var textValue = text_input.text.replace(',', '.') // Заменяем запятую на точку
        var numValue = Number.fromLocaleString(Qt.locale(), textValue)

        if (!isNaN(numValue)) {
            // Ограничиваем значение
            var limitedValue = Math.min(Math.max(numValue, from), to)
            var internalValue = limitedValue * decimalFactor

            // Округляем до нужного количества знаков после запятой
            if (decimals > 0) {
                internalValue = Math.round(internalValue)
            }

            spinner.internalValue = internalValue
            updateValueFromSpinner()
        } else {
            // Если введено недопустимое значение, восстанавливаем предыдущее
            text_input.text = spinner.formatValue(spinner.internalValue)
        }
    }

    // Функция для обновления значения из спиннера
    function updateValueFromSpinner() {
        // Проверяем, активен ли спинбокс
        if (!spinner.isActive) {
            return
        }

        var currentValue = spinner.internalValue / decimalFactor

        // Обновляем сообщение о границах
        if (Math.abs(currentValue - from) < 0.0001) {
            spin_message.visible = true
            spin_message.text = qsTr("Min: %1").arg(from)
        } else if (Math.abs(currentValue - to) < 0.0001) {
            spin_message.visible = true
            spin_message.text = qsTr("Max: %1").arg(to)
        } else {
            spin_message.visible = false
        }

        // Обновляем значение и отправляем сигнал
        if (currentValue !== value) {
            value = currentValue
            boundValue = currentValue
            valueModified()
        }
    }

    // Текст метки (если предоставлен)
    Text {
        id: spin_text
        visible: text.length > 0
        text: spinBox_Customized.text
        anchors.bottom: spinner.top
        anchors.bottomMargin: 5 * theme.scaleHeight
        anchors.horizontalCenter: spinner.horizontalCenter
        font.pixelSize: spinBox_Customized.fontPixelSize
        color: "#333333"
    }

    // Сообщение о границах
    Text {
        id: spin_message
        visible: false
        text: ""
        color: "#e74c3c"
        anchors.top: spinner.bottom
        anchors.topMargin: 5 * theme.scaleHeight
        anchors.horizontalCenter: spinner.horizontalCenter
        font.pixelSize: spinBox_Customized.fontPixelSize * 0.9
    }
}
