import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.platform as Platform

import ".."
import "../components"

Dialog {
    id:agShareApi
    height: 500  * theme.scaleHeight
    width:700  * theme.scaleWidth
    anchors.centerIn: parent
    modal: false
    visible: false
    function show(){
        agShareApi.visible = true
        agShareEn.isChecked = SettingsManager.agShare_enabled
        autoUploadButton.isChecked = SettingsManager.agShare_uploadActive
    }

        property bool settingsChanged: false
        property color accentColor: "#0078D7"
        property color successColor: "#107C10"
        property color errorColor: "#D83B01"
        property color warningColor: "#FF8C00"
        property color lightGray: "#F3F2F1"
        property color darkGray: "#605E5C"


        ScrollView {
            anchors.fill: parent
            padding: 20

            ColumnLayout {
                width: parent.width - 40
                spacing: 16

                // Секция сервера
                GroupBox {
                    title: "Server Configuration"
                    Layout.fillWidth: true

                    GridLayout {
                        columns: 2
                        columnSpacing: 10
                        rowSpacing: 10
                        anchors.fill: parent

                        Label {
                            text: "Server URL:"
                            font.bold: true
                            Layout.alignment: Qt.AlignRight
                        }

                        TextField {
                            id: serverField
                            Layout.fillWidth: true
                            placeholderText: "https://agshare.agopengps.com"
                            text: "https://agshare.agopengps.com"
                            onTextChanged: settingsChanged = true

                            background: Rectangle {
                                border.color: serverField.activeFocus ? accentColor : darkGray
                                border.width: 1
                                radius: 2
                            }
                        }

                        Item { Layout.columnSpan: 2; Layout.preferredHeight: 5 }
                    }
                }

                // Секция API ключа
                GroupBox {
                    Layout.fillWidth: true

                    GridLayout {
                        columns: 3
                        columnSpacing: 10
                        rowSpacing: 10
                        anchors.fill: parent

                        Label {
                            text: "API Key:"
                            font.bold: true
                            Layout.alignment: Qt.AlignRight
                        }

                        TextField {
                            id: apiKeyField
                            Layout.fillWidth: true
                            placeholderText: "Enter your AgShare API key"
                            echoMode: TextInput.Password
                            text: ""
                            onTextChanged: settingsChanged = true

                            background: Rectangle {
                                border.color: apiKeyField.activeFocus ? primaryColor : "#CCCCCC"
                                border.width: 1
                                radius: 4
                            }

                            Button {
                                id: showKeyButton
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.margins: 5
                                width: 30
                                height: 25
                                text: apiKeyField.echoMode === TextInput.Password ? "👁" : "🙈"
                                font.pixelSize: 12
                                onClicked: apiKeyField.echoMode = apiKeyField.echoMode === TextInput.Password
                                           ? TextInput.Normal
                                           : TextInput.Password

                                background: Rectangle {
                                    color: parent.down ? "#E0E0E0" : (parent.hovered ? "#F0F0F0" : "transparent")
                                    radius: 3
                                }
                            }
                        }

                        Button {
                            id: pasteButton
                            text: "Paste"
                            enabled: true
                            Layout.preferredWidth: 80

                            onClicked: {
                                if (Platform.Clipboard.text !== "") {
                                    apiKeyField.text = Platform.Clipboard.text
                                    // Не очищаем буфер, чтобы пользователь мог использовать данные еще раз
                                    // Platform.Clipboard.clear()
                                    settingsChanged = true
                                    statusLabel.text = "Not Saved"
                                }
                            }
                        }

                        Item { Layout.columnSpan: 3; Layout.preferredHeight: 5 }
                    }
                }

                // Секция статуса
                GroupBox {
                    title: "Connection Status"
                    Layout.fillWidth: true

                    ColumnLayout {
                        spacing: 10
                        anchors.fill: parent

                        // Индикатор статуса
                        Rectangle {
                            Layout.fillWidth: true
                            height: 40
                            radius: 4
                            border.width: 1
                            border.color: darkGray

                            Label {
                                id: statusLabel
                                anchors.centerIn: parent
                                text: "Ready"
                                color: darkGray
                                font.pixelSize: 14
                            }
                        }

                        // Прогресс-бар (скрыт по умолчанию)
                        ProgressBar {
                            id: progressBar
                            Layout.fillWidth: true
                            visible: false
                            indeterminate: true
                        }
                    }
                }

                // Секция управления
                GroupBox {
                    title: "Upload Settings"
                    Layout.fillWidth: true

                    GridLayout {
                        columns: 2
                        columnSpacing: 20
                        rowSpacing: 15
                        anchors.fill: parent

                        // Включение/отключение AgShare
                        MainWindowBtns {
                            id: agShareEn
                            text: "AgShare: OFF"
                            checkable: true
                            //checked: SettingsManager.agShare_enabled
                            icon.source: prefix + "/images/UploadOff.png"
                            iconChecked: prefix + "/images/UploadOn.png"

                            onClicked: {
                                agShareEn.text = agShareEn.checked ? "AgShare: ON" : "AgShare: OFF"
                                settingsChanged = true
                                statusLabel.text = "Not Saved"
                            }
                        }

                        // Автозагрузка
                        MainWindowBtns {
                            id: autoUploadButton
                            text: "Auto Upload: OFF"
                            checkable: true
                            //checked: SettingsManager.agShare_uploadActive
                            icon.source: prefix + "/images/AutoUploadOff.png"
                            iconChecked: prefix + "/images/AutoUploadOn.png"

                            onClicked: {
                                autoUploadButton.text = autoUploadButton.checked ? "Auto Upload: ON" : "Auto Upload: OFF"
                                settingsChanged = true
                                statusLabel.text = "Not Saved"
                            }
                        }
                    }
                }

                // Кнопки действий
                RowLayout {
                    Layout.fillWidth: true
                    Layout.topMargin: 10
                    spacing: 10

                    // Основные кнопки слева
                    RowLayout {
                        spacing: 10

                        Button {
                            id: testConnectionButton
                            text: "Test Connection"
                            Layout.preferredWidth: 140
                            Layout.preferredHeight: 35

                            background: Rectangle {
                                radius: 4
                                color: testConnectionButton.hovered ? "#106EBE" : accentColor
                            }

                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.bold: true
                            }

                            onClicked: {
                                testConnectionButton.enabled = false
                                statusLabel.text = "Connecting..."
                                statusLabel.color = darkGray
                                progressBar.visible = true

                                // Устанавливаем настройки
                                aog.agshareclient_setServerUrl(serverField.text)
                                aog.agshareclient_setApiKey(apiKeyField.text)

                                // Запускаем проверку
                                aog.agshareclient_checkApi()

                                // Скрываем прогресс через 2 секунды (на всякий случай)
                                progressTimer.restart()
                            }

                            Timer {
                                id: progressTimer
                                interval: 2000
                                onTriggered: progressBar.visible = false
                            }
                        }

                        Button {
                            id: saveButton
                            text: "Save"
                            Layout.preferredWidth: 100
                            Layout.preferredHeight: 35
                            enabled: settingsChanged

                            background: Rectangle {
                                radius: 4
                                color: saveButton.enabled ? successColor : darkGray
                            }

                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.bold: true
                            }

                            onClicked: {
                                // Сохраняем настройки
                                // Здесь должен быть вызов сохранения настроек
                                if (!validateSettings()) return
                                saveSettings()
                                settingsChanged = false
                                statusLabel.text = "✓ Settings saved"
                                statusLabel.color = successColor
                          }
                        }
                    }

                    Item { Layout.fillWidth: true }

                    // Кнопки справа
                    RowLayout {
                        spacing: 10

                        Button {
                            text: "Register"
                            Layout.preferredWidth: 100
                            Layout.preferredHeight: 35

                            background: Rectangle {
                                radius: 4
                                border.color: accentColor
                                border.width: 1
                                color: "transparent"
                            }

                            contentItem: Text {
                                text: parent.text
                                color: accentColor
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.bold: true
                            }

                            onClicked: Qt.openUrlExternally("https://agshare.agopengps.com/register")
                        }

                        Button {
                            text: "Cancel"
                            Layout.preferredWidth: 100
                            Layout.preferredHeight: 35

                            background: Rectangle {
                                radius: 4
                                border.color: darkGray
                                border.width: 1
                                color: "transparent"
                            }

                            contentItem: Text {
                                text: parent.text
                                color: darkGray
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.bold: true
                            }

                            onClicked: agShareApi.visible = false
                        }
                    }
                }

                // // Ссылка на документацию
                // Text {
                //     Layout.alignment: Qt.AlignHCenter
                //     Layout.topMargin: 10

                //     text: "Need help? <a href='https://agshare.agopengps.com/docs'>View Documentation</a>"
                //     color: darkGray
                //     font.pixelSize: 12

                //     MouseArea {
                //         anchors.fill: parent
                //         cursorShape: Qt.PointingHandCursor
                //         onClicked: Qt.openUrlExternally("https://agshare.agopengps.com/docs")
                //     }
                // }

                // Разделитель
                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#E1DFDD"
                    Layout.topMargin: 20
                    Layout.bottomMargin: 10
                }

                // Статус версии
                Label {
                    Layout.alignment: Qt.AlignHCenter
                    text: "AgShare Client v1.0 • API v2"
                    color: darkGray
                    font.pixelSize: 11
                }
            }
        }

        // Инициализация при загрузке
        Component.onCompleted: {
            // Загружаем сохраненные настройки
            serverField.text = SettingsManager.agShare_server
            apiKeyField.text = SettingsManager.agShare_apiKey
            agShareEn.checked = SettingsManager.agShare_enabled
            autoUploadButton.checked = SettingsManager.agShare_uploadActive
            settingsChanged = false
        }

        // Функция сохранения настроек
        function saveSettings() {
            // Сохраняем в настройки приложения
            SettingsManager.agShare_server = serverField.text
            SettingsManager.agShare_apiKey = apiKeyField.text
            SettingsManager.agShare_enabled = agShareEn.checked
            SettingsManager.agShare_uploadActive = autoUploadButton.checked
            settingsChanged = false
            return true
        }

        // Функция проверки настроек
            function validateSettings() {
                if (serverField.text.trim() === "") {
                    statusLabel.text = "✗ Server URL is required"
                    statusLabel.color = errorColor
                    return false
                }

                if (apiKeyField.text.trim() === "") {
                    statusLabel.text = "✗ API Key is required"
                    statusLabel.color = errorColor
                    return false
                }

                // Валидация URL
                var urlRegex = /^(https?:\/\/)?([\da-z\.-]+)\.([a-z\.]{2,6})([\/\w \.-]*)*\/?$/
                if (!urlRegex.test(serverField.text)) {
                    statusLabel.text = "✗ Invalid server URL"
                    statusLabel.color = errorColor
                    return false
                }

                return true
            }
    }
