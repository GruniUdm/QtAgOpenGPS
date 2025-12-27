// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Pinout for hyd lift/sections
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Layouts
import AOG

import ".."
import "../components"

Rectangle{
    id: configModulesBlockage
    anchors.fill: parent
    color: aogInterface.backgroundColor
    visible: false

    onVisibleChanged: {
        if (visible) load_settings()
    }

    function load_settings() {
        // Threading Phase 1: Seed blockage configuration
        graincountMin.boundValue = SettingsManager.seed_blockCountMin
        graincountMax.boundValue = SettingsManager.seed_blockCountMax
        modulerows1.boundValue = SettingsManager.seed_blockRow1
        modulerows2.boundValue = SettingsManager.seed_blockRow2
        modulerows3.boundValue = SettingsManager.seed_blockRow3
        modulerows4.boundValue = SettingsManager.seed_blockRow4
        cboxIsBlockageOn.checked = SettingsManager.seed_blockageIsOn

        mandatory.visible = false
    }

    function save_settings() {
        // Threading Phase 1: Save seed blockage configuration
        SettingsManager.seed_blockCountMin = graincountMin.value
        SettingsManager.seed_blockCountMax = graincountMax.value
        SettingsManager.seed_blockRow1 = modulerows1.value
        SettingsManager.seed_blockRow2 = modulerows2.value
        SettingsManager.seed_blockRow3 = modulerows3.value
        SettingsManager.seed_blockRow4 = modulerows4.value
        SettingsManager.seed_blockageIsOn = cboxIsBlockageOn.checked
        SettingsManager.seed_numRows = Number(SettingsManager.seed_blockRow1 + SettingsManager.seed_blockRow2 + SettingsManager.seed_blockRow3 + SettingsManager.seed_blockRow4)
        blockageRows.setSizes()
        mandatory.visible = false

        aog.toBlockageMonitoring() // Qt 6.8 MODERN: Direct Q_INVOKABLE call
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10 * theme.scaleWidth

        // Заголовок
        Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 10 * theme.scaleHeight
            Layout.bottomMargin: 20 * theme.scaleHeight
            text: qsTr("Planter Monitor")
            font.bold: true
        }

        // Основная область настроек
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20 * theme.scaleWidth

            // Левая часть - настройки рядов модулей
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width * 0.5
                spacing: 0

                // Заголовок модулей
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 15 * theme.scaleHeight
                    Layout.preferredHeight: 30 * theme.scaleHeight
                    text: qsTr("Module Rows Configuration")
                    font.bold: true
                    color: aogInterface.textColor
                }

                // Контейнер для равномерного распределения 4 групп
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0

                    // Первая группа - модуль 1
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            //spacing: 5 * theme.scaleHeight

                            SpinBoxCustomized {
                                id: modulerows1
                                Layout.fillWidth: true
                                from: 0
                                to: 255
                                editable: true
                                enabled: cboxIsBlockageOn.checked
                                boundValue: SettingsManager.seed_blockRow1
                                onValueModified: {
                                    SettingsManager.seed_blockRow1 = value
                                    mandatory.visible = true
                                }

                                Text {
                                    text: qsTr("Rows on module 1")
                                    font.bold: true
                                    color: aogInterface.textColor
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5 * theme.scaleWidth
                                }
                            }
                        }
                    }

                    // Вторая группа - модуль 2
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            //spacing: 5 * theme.scaleHeight

                            SpinBoxCustomized {
                                id: modulerows2
                                Layout.fillWidth: true
                                from: 0
                                to: 255
                                editable: true
                                enabled: cboxIsBlockageOn.checked
                                boundValue: SettingsManager.seed_blockRow2
                                onValueModified: {
                                    SettingsManager.seed_blockRow2 = value
                                    mandatory.visible = true
                                }
                                Text {
                                    text: qsTr("Rows on module 2")
                                    font.bold: true
                                    color: aogInterface.textColor
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5 * theme.scaleWidth
                                }
                            }
                        }
                    }

                    // Третья группа - модуль 3
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            //spacing: 5 * theme.scaleHeight

                            SpinBoxCustomized {
                                id: modulerows3
                                Layout.fillWidth: true
                                from: 0
                                to: 255
                                editable: true
                                enabled: cboxIsBlockageOn.checked
                                boundValue: SettingsManager.seed_blockRow3
                                onValueModified: {
                                    SettingsManager.seed_blockRow3 = value
                                    mandatory.visible = true
                                }
                                Text {
                                    text: qsTr("Rows on module 3")
                                    font.bold: true
                                    color: aogInterface.textColor
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5 * theme.scaleWidth
                                }
                            }
                        }
                    }

                    // Четвертая группа - модуль 4
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            //spacing: 5 * theme.scaleHeight

                            SpinBoxCustomized {
                                id: modulerows4
                                Layout.fillWidth: true
                                from: 0
                                to: 255
                                editable: true
                                enabled: cboxIsBlockageOn.checked
                                boundValue: SettingsManager.seed_blockRow4
                                onValueModified: {
                                    SettingsManager.seed_blockRow4 = value
                                    mandatory.visible = true
                                }
                                Text {
                                    text: qsTr("Rows on module 4")
                                    font.bold: true
                                    color: aogInterface.textColor
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5 * theme.scaleWidth
                                }
                            }
                        }
                    }
                }
            }

            // Разделительная линия
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 1
                color: aogInterface.borderColor
            }

            // Правая часть - настройки порогов зерен
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width * 0.5
                spacing: 0

                // Заголовок порогов
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 15 * theme.scaleHeight
                    Layout.preferredHeight: 30 * theme.scaleHeight
                    text: qsTr("Grain Count Thresholds")
                    font.bold: true
                    color: aogInterface.textColor
                }

                // Контейнер для равномерного распределения 3 групп
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0

                    // Первая группа - минимум
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            //spacing: 5 * theme.scaleHeight



                            SpinBoxCustomized {
                                id: graincountMin
                                Layout.fillWidth: true
                                from: 0
                                to: 10000
                                editable: true
                                enabled: cboxIsBlockageOn.checked
                                boundValue: SettingsManager.seed_blockCountMin
                                onValueModified: {
                                    SettingsManager.seed_blockCountMin = value
                                    mandatory.visible = true
                                }
                                Text {
                                    text: qsTr("Minimum grain count " + Utils.per_unit())
                                    font.bold: true
                                    color: aogInterface.textColor
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5 * theme.scaleWidth
                                }
                            }
                        }
                    }

                    // Вторая группа - максимум
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ColumnLayout {
                            anchors.fill: parent
                            //spacing: 5 * theme.scaleHeight

                            SpinBoxCustomized {
                                id: graincountMax
                                Layout.fillWidth: true
                                from: 0
                                to: 10000
                                editable: true
                                enabled: cboxIsBlockageOn.checked
                                boundValue: SettingsManager.seed_blockCountMax
                                onValueModified: {
                                    SettingsManager.seed_blockCountMax = value
                                    mandatory.visible = true
                                }

                                Text {
                                    text: qsTr("Maximum grain count " + Utils.per_unit())
                                    font.bold: true
                                    color: aogInterface.textColor
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    anchors.bottom: parent.top
                                    anchors.bottomMargin: 5 * theme.scaleWidth
                                }
                            }
                        }
                    }

                    // Третья группа - включение мониторинга
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        RowLayout {
                            anchors.fill: parent
                            spacing: 10 * theme.scaleWidth
                            // Оставлено пустым, так как переключатель перенесен вниз
                        }
                    }
                }
            }
        }

        // Нижняя панель с кнопками - ИСПРАВЛЕННАЯ ВЕРСИЯ
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 100 * theme.scaleHeight
            Layout.topMargin: 20 * theme.scaleHeight
            Layout.bottomMargin: 10 * theme.scaleHeight

            // Левые кнопки - выровнены по левому краю
            Row {
                spacing: 10 * theme.scaleWidth
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter

                IconButtonTransparent {
                    id: back
                    width: 80 * theme.scaleWidth
                    height: 60 * theme.scaleHeight
                    anchors.verticalCenter: parent.verticalCenter
                    enabled: cboxIsBlockageOn.checked
                    icon.source: prefix + "/images/back-button.png"
                    onClicked: {
                        graincountMin.boundValue = 0
                        graincountMax.boundValue = 0
                        modulerows1.boundValue = 0
                        modulerows2.boundValue = 0
                        modulerows3.boundValue = 0
                        modulerows4.boundValue = 0
                        crops.currentIndex[0] = 0
                        mandatory.visible = true
                    }
                }

                IconButtonTransparent {
                    id: loadSetBlockage
                    width: 80 * theme.scaleWidth
                    height: 60 * theme.scaleHeight
                    anchors.verticalCenter: parent.verticalCenter
                    enabled: cboxIsBlockageOn.checked
                    icon.source: prefix + "/images/UpArrow64.png"
                    onClicked: {
                        load_settings()
                        mandatory.visible = true
                    }
                }

                // Кнопка включения мониторинга с текстом снизу
                Column {
                    //spacing: 5 * theme.scaleHeight
                    anchors.verticalCenter: parent.verticalCenter

                    IconButtonColor {
                        id: cboxIsBlockageOn
                        width: 100 * theme.scaleWidth
                        height: 60 * theme.scaleHeight
                        anchors.horizontalCenter: parent.horizontalCenter
                        icon.source: prefix + "/images/SwitchOff.png"
                        iconChecked: prefix + "/images/SwitchOn.png"
                        checkable: true
                        onClicked: mandatory.visible = true
                        Text {
                            text: qsTr("Enable Blockage Monitoring")
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5 * theme.scaleWidth
                            color: aogInterface.textColor
                        }
                    }
                }
            }

            // Правые кнопки - выровнены по правому краю
            Row {
                spacing: 10 * theme.scaleWidth
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    id: mandatory
                    width: 30 * theme.scaleWidth
                    height: 30 * theme.scaleHeight
                    anchors.verticalCenter: parent.verticalCenter
                    visible: false
                    source: prefix + "/images/Config/ConSt_Mandatory.png"
                    fillMode: Image.PreserveAspectFit
                }

                // Кнопка сохранения с текстом снизу
                Column {
                    //spacing: 5 * theme.scaleHeight
                    anchors.verticalCenter: parent.verticalCenter

                    IconButtonTransparent {
                        id: btnPinsSave
                        width: 150 * theme.scaleWidth
                        height: 60 * theme.scaleHeight
                        anchors.horizontalCenter: parent.horizontalCenter
                        icon.source: prefix + "/images/ToolAcceptChange.png"
                        onClicked: save_settings()
                        Text {
                            text: qsTr("Send + Save")
                            font.bold: true
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.top
                            anchors.bottomMargin: 5 * theme.scaleWidth
                            color: aogInterface.textColor
                        }
                    }
                }
            }
        }
    }
}
