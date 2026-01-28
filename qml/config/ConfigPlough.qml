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

        ModuleComm.modulesSend238()
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
                        icon.source: prefix + "/images/Config/PloughOff.png"
                        iconChecked: prefix + "/images/Config/PloughOn.png"
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


