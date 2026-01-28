// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Config Hyd lift timing etc
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion
//import Settings
import AOG

import ".."
import "../components"

/*todo:
 can't find setting for "invert relays"
 not sure about the "send button"
 */
Rectangle{
    id: configPlough
    anchors.fill: parent
    color: aogInterface.backgroundColor

    onVisibleChanged: {
        load_settings()
    }

    function load_settings()
    {
        if (visible) {
            var sett = SettingsManager.ardMac_setting0

            if ((sett & 1) === 0 ) invertRelays.checked = false
            else invertRelays.checked = true

            if ((sett & 2) === 0 ) plowControlOn.checked = false
            else plowControlOn.checked = true

            desiredWidth.value = SettingsManager.ardMac_hydRaiseTime
            deadzonePlow.value = SettingsManager.ardMac_hydLowerTime

            nudUser1.value = SettingsManager.ardMac_user1
            nudUser2.value = SettingsManager.ardMac_user2
            nudUser3.value = SettingsManager.ardMac_user3
            nudUser4.value = SettingsManager.ardMac_user4

            // Загружаем сохраненное значение измеренной разницы
            measuredDiff.value = SettingsManager.vehicle_hydraulicLiftLookAhead

            unsaved.visible = false
        }
    }

    function save_settings() {
        var set = 1
        var reset = 2046
        var sett = 0

        if (invertRelays.checked) sett |= set
        else sett &= reset

        set <<=1
        reset <<= 1
        reset += 1

        if(plowControlOn.checked) sett |= set
        else sett &= reset

        SettingsManager.ardMac_setting0 = sett
        SettingsManager.ardMac_hydRaiseTime = desiredWidth.value
        SettingsManager.ardMac_hydLowerTime = deadzonePlow.value

        SettingsManager.ardMac_user1 = nudUser1.value
        SettingsManager.ardMac_user2 = nudUser2.value
        SettingsManager.ardMac_user3 = nudUser3.value
        SettingsManager.ardMac_user4 = nudUser4.value

        SettingsManager.vehicle_hydraulicLiftLookAhead = measuredDiff.value
        SettingsManager.ardMac_isHydEnabled = plowControlOn.checked

        ModuleComm.modulesSend238()
        //pboxSendMachine.Visible = false
    }

    // Функция для калибровки минимального значения
    function calibrateMin() {
        // Здесь должна быть логика калибровки минимального значения
        console.log("Starting minimum calibration...")

        // Сбрасываем измеренную разницу на 0 для новой калибровки
        onClicked: {
            unsaved.visible = true
            measuredDiff.setSpinValue(0)
        }

        // TODO: Добавить логику отправки команды на калибровку
        // ModuleComm.calibrateMin()
    }

    // Функция для калибровки максимального значения
    function calibrateMax() {
        // Здесь должна быть логика калибровки максимального значения
        console.log("Starting maximum calibration...")

        // Сбрасываем измеренную разницу на 0 для новой калибровки
        onClicked: {
            unsaved.visible = true
            measuredDiff.setSpinValue(0)
        }

        // TODO: Добавить логику отправки команды на калибровку
        // ModuleComm.calibrateMax()
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10 * theme.scaleWidth

        // Заголовок
        Text {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 10 * theme.scaleHeight
            Layout.bottomMargin: 20 * theme.scaleHeight
            text: qsTr("Plough Control")
            font.bold: true
            font.pixelSize: 18 * theme.scaleHeight
            color: aogInterface.textColor
        }

        // Основная таблица в GridLayout
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            border.color: aogInterface.textColor
            border.width: 2 * theme.scaleHeight

            GridLayout {
                anchors.fill: parent
                anchors.margins: 2 * theme.scaleHeight
                columns: 3
                rows: 4
                rowSpacing: 0
                columnSpacing: 0

                // === Ряд 1: Кнопка включения ===
                // Ячейка 1.1 - Кнопка включения (занимает все 3 колонки)
                Rectangle {
                    Layout.row: 0
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.preferredHeight: 160 * theme.scaleHeight
                    color: "transparent"


                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10 * theme.scaleWidth

                        IconButtonColor {
                            id: plowControlOn
                            Layout.preferredWidth: 150 * theme.scaleWidth
                            Layout.preferredHeight: 120 * theme.scaleHeight
                            Layout.alignment: Qt.AlignCenter
                            text: qsTr("Enable")
                            icon.source: prefix + "/images/Config/PloughOff.png"
                            iconChecked: prefix + "/images/Config/PloughOn.png"
                            checkable: true
                            onClicked: unsaved.visible = true
                        }
                    }
                }
                Item {
                    Layout.row: 0
                    Layout.column: 1
                    Layout.fillWidth: true
                    //Layout.fillHeight: true

                    IconButtonColor {
                        id: invertRelays
                        anchors.centerIn: parent
                        height: 60 * theme.scaleHeight
                        text: qsTr("Invert Relay")
                        icon.source: prefix + "/images/Config/ConSt_InvertRelay.png"
                        checkable: true
                        enabled: plowControlOn.checked
                        onClicked: unsaved.visible = true
                    }
                }
                // Ячейка 2.3 - Изображение
                Rectangle {
                    Layout.row: 0
                    Layout.column: 2
                    Layout.rowSpan: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"

                    Image {
                        anchors.fill: parent
                        anchors.margins: 10 * theme.scaleWidth
                        source: prefix + "/images/Config/PloughWidth.png"
                        fillMode: Image.PreserveAspectFit
                    }
                }

                // === Ряд 2 ===
                // Ячейка 2.1 - measuredDiff
                Rectangle {
                    Layout.row: 1
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"


                    SpinBoxCustomized {
                        id: measuredDiff
                        anchors.fill: parent
                        anchors.margins: 5 * theme.scaleWidth
                        from: 0
                        to: 255
                        editable: true
                        enabled: plowControlOn.checked
                        text: qsTr("Enter Measured Difference")
                        onValueChanged: {
                            unsaved.visible = true
                        }
                        decimals: 0

                    }
                }

                // Ячейка 2.2 - deadzonePlow
                Rectangle {
                    Layout.row: 1
                    Layout.column: 1
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"


                    SpinBoxCustomized {
                        id: deadzonePlow
                        anchors.fill: parent
                        anchors.margins: 5 * theme.scaleWidth
                        from: 1
                        to: 255
                        editable: true
                        enabled: plowControlOn.checked
                        text: qsTr("Deadzone in mm")
                        onValueChanged: unsaved.visible = true
                    }
                }



                // === Ряд 3 ===
                // Ячейка 3.1 - btn_calMin
                Rectangle {
                    Layout.row: 2
                    Layout.column: 0
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"


                    IconButtonColor {
                        id: btn_calMin
                        anchors.centerIn: parent
                        height: 60 * theme.scaleHeight
                        text: qsTr("Min Cal")
                        icon.source: prefix + "/images/Config/calMin.png"
                        enabled: plowControlOn.checked
                        onClicked: {
                            // Вызываем функцию калибровки
                            calibrateMin()
                        }
                    }
                }

                // Ячейка 3.2 - btn_calMax
                Rectangle {
                    Layout.row: 2
                    Layout.column: 1
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"

                    IconButtonColor {
                        id: btn_calMax
                        anchors.centerIn: parent
                        height: 60 * theme.scaleHeight
                        text: qsTr("Max Cal")
                        icon.source: prefix + "/images/Config/calMax.png"
                        enabled: plowControlOn.checked
                        onClicked: {
                            // Вызываем функцию калибровки
                            calibrateMax()
                        }
                    }
                }

                // Ячейка 3.3 - desiredWidth
                Rectangle {
                    Layout.row: 2
                    Layout.column: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"


                    SpinBoxCustomized {
                        id: desiredWidth
                        anchors.fill: parent
                        anchors.margins: 5 * theme.scaleWidth
                        from: 1
                        to: 255
                        editable: true
                        enabled: plowControlOn.checked
                        text: qsTr("Desired Plough Width")
                        onValueChanged: unsaved.visible = true
                    }
                }

                // === Ряд 4: Кнопка сохранения ===
                // Ячейка 4.1 - Пустая
                Item{
                    Layout.row: 3
                    Layout.column: 0
                }

                // Ячейка 4.2 - Пустая
                Item{
                    Layout.row: 3
                    Layout.column: 1
                }

                // Ячейка 4.3 - Кнопка сохранения
                Rectangle {
                    Layout.row: 3
                    Layout.column: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "transparent"


                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 10 * theme.scaleWidth

                        // Текст кнопки
                        Text {
                            id: saveButtonText
                            text: qsTr("Send + Save")
                            color: aogInterface.textColor
                            font.pixelSize: 14 * theme.scaleHeight
                        }

                        // Изображение несохраненных изменений
                        Image {
                            id: unsaved
                            Layout.preferredWidth: 30 * theme.scaleWidth
                            Layout.preferredHeight: 30 * theme.scaleHeight
                            visible: false
                            source: prefix + "/images/Config/ConSt_Mandatory.png"
                            fillMode: Image.PreserveAspectFit
                        }

                        // Кнопка сохранения
                        IconButtonTransparent {
                            id: modulesSave
                            objectName: "btnModulesSave"
                            icon.source: prefix + "/images/ToolAcceptChange.png"
                            onClicked: {
                                save_settings();
                                unsaved.visible = false
                            }
                        }
                    }
                }
            }
        }
    }
}
