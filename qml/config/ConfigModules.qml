// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Config Hyd lift timing etc
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion
//import Settings

import ".."
import "../components"

/*todo:
 can't find setting for "invert relays"
 not sure about the "send button"
 */
Rectangle{
    id: configModules
    anchors.fill: parent
    color: aogInterface.backgroundColor

    onVisibleChanged: {
        load_settings()
    }

    function load_settings()
    {
        if (visible) {
            var sett = SettingsManager.ardMac_setting0

            if ((sett & 1) === 0 ) cboxMachInvertRelays.checked = false
            else cboxMachInvertRelays.checked = true

            if ((sett & 2) === 0 ) cboxIsHydOn.checked = false
            else cboxIsHydOn.checked = true

            nudRaiseTime.value = SettingsManager.ardMac_hydRaiseTime
            nudLowerTime.value = SettingsManager.ardMac_hydLowerTime

            nudUser1.value = SettingsManager.ardMac_user1
            nudUser2.value = SettingsManager.ardMac_user2
            nudUser3.value = SettingsManager.ardMac_user3
            nudUser4.value = SettingsManager.ardMac_user4

            unsaved.visible = false
        }
    }

    function save_settings() {
        var set = 1
        var reset = 2046
        var sett = 0

        if (cboxMachInvertRelays.checked) sett |= set
        else sett &= reset

        set <<=1
        reset <<= 1
        reset += 1

        if(cboxIsHydOn.checked) sett |= set
        else sett &= reset

        SettingsManager.ardMac_setting0 = sett
        SettingsManager.ardMac_hydRaiseTime = nudRaiseTime.value
        SettingsManager.ardMac_hydLowerTime = nudLowerTime.value

        SettingsManager.ardMac_user1 = nudUser1.value
        SettingsManager.ardMac_user2 = nudUser2.value
        SettingsManager.ardMac_user3 = nudUser3.value
        SettingsManager.ardMac_user4 = nudUser4.value

        SettingsManager.vehicle_hydraulicLiftLookAhead = nudHydLiftLookAhead.value
        SettingsManager.ardMac_isHydEnabled = cboxIsHydOn.checked

        //set pgns
        //signal handler will do the folling:
        /*
            mf.p_238.pgn[mf.p_238.set0] = property_setArdMac_setting0
            mf.p_238.pgn[mf.p_238.raiseTime] = property_setArdMac_hydRaiseTime
            mf.p_238.pgn[mf.p_238.lowerTime] = property_setArdMac_hydLowerTime

            mf.p_238.pgn[mf.p_238.user1] = property_setArd_user1
            mf.p_238.pgn[mf.p_238.user2] = property_setArd_user2
            mf.p_238.pgn[mf.p_238.user3] = property_setArd_user3
            mf.p_238.pgn[mf.p_238.user4] = property_setArd_user3
            */

        aog.modulesSend238() // Qt 6.8 MODERN: Direct Q_INVOKABLE call
        //pboxSendMachine.Visible = false
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10 * theme.scaleWidth

        // Заголовок
        Text {
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 10 * theme.scaleHeight
            Layout.bottomMargin: 20 * theme.scaleHeight
            text: qsTr("Machine Module")
            font.bold: true
        }

        // Основное содержимое
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20 * theme.scaleWidth

            // Левая часть - гидравлика
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width * 0.7

                // Заголовок гидравлики
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 15 * theme.scaleHeight
                    text: qsTr("Hydraulic Lift Config")
                    font.bold: true
                    color: aogInterface.textColor
                }

                // Grid для гидравлики (3 строки × 3 столбца)
                GridLayout {
                    id: hydGrid
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    columns: 3
                    rows: 2
                    columnSpacing: 15 * theme.scaleWidth
                    rowSpacing: 15 * theme.scaleHeight

                    // Первая строка
                    IconButtonColor {
                        id: cboxIsHydOn
                        Layout.preferredWidth: 150 * theme.scaleWidth
                        Layout.preferredHeight: 120 * theme.scaleHeight
                        Layout.alignment: Qt.AlignCenter
                        icon.source: prefix + "/images/SwitchOff.png"
                        iconChecked: prefix + "/images/SwitchOn.png"
                        checkable: true
                        onClicked: unsaved.visible = true
                    }

                    SpinBoxCustomized {
                        id: nudRaiseTime
                        Layout.fillWidth: true
                        from: 1
                        to: 255
                        editable: true
                        enabled: cboxIsHydOn.checked
                        text: qsTr("Raise Time (secs)")
                        onValueChanged: unsaved.visible = true
                    }

                    Image {
                        Layout.preferredWidth: 180 * theme.scaleWidth
                        Layout.preferredHeight: 180 * theme.scaleHeight
                        Layout.alignment: Qt.AlignCenter
                        source: prefix + "/images/Config/ConMa_LiftRaiseTime.png"
                        fillMode: Image.PreserveAspectFit
                    }

                    // Вторая строка
                    SpinBoxCustomized {
                        id: nudHydLiftLookAhead
                        Layout.fillWidth: true
                        from: 1
                        to: 20
                        editable: true
                        enabled: cboxIsHydOn.checked
                        text: qsTr("Look Ahead (secs)")
                        onValueChanged: unsaved.visible = true
                        decimals: 1
                    }

                    SpinBoxCustomized {
                        id: nudLowerTime
                        Layout.fillWidth: true
                        from: 1
                        to: 255
                        editable: true
                        enabled: cboxIsHydOn.checked
                        text: qsTr("Lower Time (secs)")
                        onValueChanged: unsaved.visible = true
                    }

                    Image {
                        Layout.preferredWidth: 180 * theme.scaleWidth
                        Layout.preferredHeight: 180 * theme.scaleHeight
                        Layout.alignment: Qt.AlignCenter
                        source: prefix + "/images/Config/ConMa_LiftLowerTime.png"
                        fillMode: Image.PreserveAspectFit
                    }

                    // Кнопка инвертирования реле
                    IconButtonColor {
                        id: cboxMachInvertRelays
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: 10 * theme.scaleHeight
                        text: qsTr("Invert Relays")
                        icon.source: prefix + "/images/Config/ConSt_InvertRelay.png"
                        checkable: true
                        enabled: cboxIsHydOn.checked
                        onClicked: unsaved.visible = true
                    }
                }


            }

            // Разделительная линия
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: 1
                color: aogInterface.borderColor
            }

            // Правая часть - пользовательские настройки и кнопка сохранения
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width * 0.3
                spacing: 0

                // Заголовок пользовательских настроек
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 15 * theme.scaleHeight
                    text: qsTr("User Settings")
                    font.bold: true
                    color: aogInterface.textColor
                }

                // Контейнер для равномерного распределения всех элементов
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 0

                    // Пользовательские настройки - каждая в своем слоте
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        SpinBoxCustomized {
                            id: nudUser1
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width
                            from: 0
                            to: 255
                            editable: true
                            text: qsTr("User 1")
                            onValueChanged: unsaved.visible = true
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        SpinBoxCustomized {
                            id: nudUser2
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width
                            from: 0
                            to: 255
                            editable: true
                            text: qsTr("User 2")
                            onValueChanged: unsaved.visible = true
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        SpinBoxCustomized {
                            id: nudUser3
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width
                            from: 0
                            to: 255
                            editable: true
                            text: qsTr("User 3")
                            onValueChanged: unsaved.visible = true
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        SpinBoxCustomized {
                            id: nudUser4
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width
                            from: 0
                            to: 255
                            editable: true
                            text: qsTr("User 4")
                            onValueChanged: unsaved.visible = true
                        }
                    }

                    // Кнопка сохранения - также в своем слоте
                    Item {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        // Контейнер для кнопки и изображения
                        RowLayout {
                            anchors.centerIn: parent
                            spacing: 10 * theme.scaleWidth

                            // Изображение несохраненных изменений (справа от кнопки)
                            Image {
                                id: unsaved
                                Layout.preferredWidth: 30 * theme.scaleWidth
                                Layout.preferredHeight: 30 * theme.scaleHeight
                                visible: false
                                source: prefix + "/images/Config/ConSt_Mandatory.png"
                                fillMode: Image.PreserveAspectFit
                            }

                            // Текст кнопки
                            Text {
                                id: saveButtonText
                                text: qsTr("Send + Save")
                                color: aogInterface.textColor
                            }

                            // Кнопка сохранения
                            IconButtonTransparent {
                                id: modulesSave
                                objectName: "btnModulesSave"
                                icon.source: prefix + "/images/ToolAcceptChange.png"
                                onClicked: { save_settings(); unsaved.visible = false }
                            }
                        }
                    }
                }
            }
        }
    }
}
