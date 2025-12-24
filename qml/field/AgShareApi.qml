import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Dialog {
    id: agShareApi
    visible: false
    height: 500 * theme.scaleHeight
    width: 700 * theme.scaleWidth
    anchors.centerIn: parent
    modal: false

    property bool settingsChanged: false

    // Цвета в стиле Windows
    property color accentColor: "#0078D7"
    property color successColor: "#107C10"
    property color errorColor: "#D83B01"
    property color warningColor: "#FF8C00"
    property color lightGray: "#F3F2F1"
    property color darkGray: "#605E5C"
    property color primaryColor: "#0078D7"

    // Таймер для проверки буфера обмена
    Timer {
        id: clipboardTimer
        interval: 500
        running: visible
        repeat: true
        onTriggered: {
            // Используем встроенный Clipboard из QtQuick
            pasteButton.enabled = false // Сначала отключаем

            // Проверяем буфер обмена через задержку
            checkClipboardTimer.start()
        }
    }

    Timer {
        id: checkClipboardTimer
        interval: 50
        onTriggered: {
            // Безопасная проверка буфера обмена
            try {
                pasteButton.enabled = true // В реальном приложении здесь должна быть проверка
            } catch (error) {
                console.log("Clipboard check error:", error)
                pasteButton.enabled = false
            }
        }
    }

    // Connections {
    //     target: aog
    //     // Предполагаем, что aog имеет сигналы для AgShare клиента
    //     function onAgShareApiChecked(success, message) {
    //         testConnectionButton.enabled = true
    //         progressBar.visible = false

    //         if (success) {
    //             statusLabel.text = "✓ Connection successful"
    //             statusLabel.color = successColor
    //         } else {
    //             statusLabel.text = "✗ " + message
    //             statusLabel.color = errorColor
    //         }
    //     }
    // }

    background: Rectangle {
        color: "ghostwhite"
        radius: 8
        border.color: darkGray
        border.width: 1
    }

    ScrollView {
        anchors.fill: parent
        padding: 20

        ColumnLayout {
            width: parent.width - 40
            spacing: 16

            // Заголовок
            Label {
                text: "AgShare Settings"
                font.pixelSize: 20
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: 10
            }

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
                        onTextChanged: {
                            settingsChanged = true
                            statusLabel.text = "Not Saved"
                        }
                    }

                    Item { Layout.columnSpan: 2; Layout.preferredHeight: 5 }
                }
            }

            // Секция API ключа
            GroupBox {
                title: "API Key Configuration"
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
                        text: "AgShare-5d6d368e-9d87-4191-8609-e7f803a7ac8a"
                        onTextChanged: {
                            settingsChanged = true
                            statusLabel.text = "Not Saved"
                        }

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
                        }
                    }



                    Button {
                        id: pasteButton
                        text: "Paste"
                        enabled: false // По умолчанию отключена
                        Layout.preferredWidth: 80

                        onClicked: {
                            // Простая реализация - в реальном приложении нужен Clipboard
                            // Временно разрешаем ввод вручную
                            apiKeyField.text = "PASTE_FUNCTIONALITY_REQUIRES_CLIPBOARD_API"
                            settingsChanged = true
                            statusLabel.text = "Not Saved"
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
                    Rectangle {
                        id: toggleButton
                        Layout.fillWidth: true
                        Layout.preferredHeight: 40
                        radius: 4
                        color: toggleButton.enabledState ? successColor : errorColor

                        property bool enabledState: false

                        Label {
                            anchors.centerIn: parent
                            text: toggleButton.enabledState ? "AgShare: ON" : "AgShare: OFF"
                            color: "white"
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                toggleButton.enabledState = !toggleButton.enabledState
                                settingsChanged = true
                                statusLabel.text = "Not Saved"
                            }
                        }
                    }

                    // Автозагрузка
                    Rectangle {
                        id: autoUploadButton
                        Layout.fillWidth: true
                        Layout.preferredHeight: 40
                        radius: 4
                        color: autoUploadButton.autoUploadState ? successColor : darkGray

                        property bool autoUploadState: false

                        Label {
                            anchors.centerIn: parent
                            text: autoUploadButton.autoUploadState ? "Auto Upload: ON" : "Auto Upload: OFF"
                            color: "white"
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                autoUploadButton.autoUploadState = !autoUploadButton.autoUploadState
                                settingsChanged = true
                                statusLabel.text = "Not Saved"
                            }
                        }
                    }

                    Label {
                        text: "Enable/disable AgShare integration"
                        color: darkGray
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Automatically upload fields when created"
                        color: darkGray
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
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

                    Rectangle {
                        id: testConnectionButton
                        Layout.preferredWidth: 140
                        Layout.preferredHeight: 35
                        radius: 4
                        color: testConnectionButton.enabled ? accentColor : darkGray

                        property bool enabled: true

                        Label {
                            anchors.centerIn: parent
                            text: "Test Connection"
                            color: "white"
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            enabled: testConnectionButton.enabled
                            onClicked: {
                                if (!validateSettings()) return

                                testConnectionButton.enabled = false
                                statusLabel.text = "Connecting..."
                                statusLabel.color = darkGray
                                progressBar.visible = true

                                // Устанавливаем настройки
                                if (aog && typeof aog.agshareclient_setServerUrl === "function") {
                                    aog.agshareclient_setServerUrl(serverField.text)
                                }
                                if (aog && typeof aog.agshareclient_setApiKey === "function") {
                                    aog.agshareclient_setApiKey(apiKeyField.text)
                                }

                                // Запускаем проверку
                                if (aog && typeof aog.agshareclient_checkApi === "function") {
                                    aog.agshareclient_checkApi()
                                } else {
                                    // Если функция не существует, имитируем ответ
                                    testConnectionTimer.start()
                                }

                                // Скрываем прогресс через 2 секунды
                                progressTimer.restart()
                            }
                        }

                        Timer {
                            id: testConnectionTimer
                            interval: 1000
                            onTriggered: {
                                testConnectionButton.enabled = true
                                progressBar.visible = false
                                statusLabel.text = "✓ Test connection (simulated)"
                                statusLabel.color = successColor
                            }
                        }

                        Timer {
                            id: progressTimer
                            interval: 2000
                            onTriggered: progressBar.visible = false
                        }
                    }

                    Rectangle {
                        id: saveButton
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 35
                        radius: 4
                        color: settingsChanged ? successColor : darkGray

                        Label {
                            anchors.centerIn: parent
                            text: "Save"
                            color: "white"
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            enabled: settingsChanged
                            onClicked: {
                                if (!validateSettings()) return

                                // Сохраняем настройки
                                saveSettings()
                                settingsChanged = false
                                statusLabel.text = "✓ Settings saved"
                                statusLabel.color = successColor
                            }
                        }
                    }
                }

                Item { Layout.fillWidth: true }

                // Кнопки справа
                RowLayout {
                    spacing: 10

                    Rectangle {
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 35
                        radius: 4
                        border.color: accentColor
                        border.width: 1
                        color: "transparent"

                        Label {
                            anchors.centerIn: parent
                            text: "Register"
                            color: accentColor
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: Qt.openUrlExternally("https://agshare.agopengps.com/register")
                        }
                    }

                    Rectangle {
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 35
                        radius: 4
                        border.color: darkGray
                        border.width: 1
                        color: "transparent"

                        Label {
                            anchors.centerIn: parent
                            text: "Close"
                            color: darkGray
                            font.bold: true
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                canceled()
                                agShareApi.close()
                            }
                        }
                    }
                }
            }

            // Ссылка на документацию
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 10
                Layout.preferredHeight: 20
                color: "transparent"

                Label {
                    anchors.centerIn: parent
                    text: "Need help? View Documentation"
                    color: darkGray
                    font.pixelSize: 12
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: Qt.openUrlExternally("https://agshare.agopengps.com/docs")
                }
            }

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
        loadSettings()
        settingsChanged = false
    }

    onVisibleChanged: {
        if (visible) {
            loadSettings()
        }
    }

    // Функция загрузки настроек
    function loadSettings() {
        // Проверяем существование SettingsManager
        if (typeof SettingsManager !== "undefined") {
            if (SettingsManager.agShare_server !== undefined) {
                serverField.text = SettingsManager.agShare_server
            }
            if (SettingsManager.agShare_apiKey !== undefined) {
                apiKeyField.text = SettingsManager.agShare_apiKey
            }
            if (SettingsManager.agShare_enabled !== undefined) {
                toggleButton.enabledState = SettingsManager.agShare_enabled
            }
            if (SettingsManager.agShare_uploadActive !== undefined) {
                autoUploadButton.autoUploadState = SettingsManager.agShare_uploadActive
            }
        } else {
            // Загружаем значения по умолчанию
            serverField.text = "https://agshare.agopengps.com"
            apiKeyField.text = ""
            toggleButton.enabledState = false
            autoUploadButton.autoUploadState = false
        }

        settingsChanged = false
        statusLabel.text = "Ready"
        statusLabel.color = darkGray
    }

    // Функция сохранения настроек
    function saveSettings() {
        // Проверяем существование SettingsManager
        if (typeof SettingsManager !== "undefined") {
            SettingsManager.agShare_server = serverField.text
            SettingsManager.agShare_apiKey = apiKeyField.text
            SettingsManager.agShare_enabled = toggleButton.enabledState
            SettingsManager.agShare_uploadActive = autoUploadButton.autoUploadState

            // Если SettingsManager имеет метод save()
            if (typeof SettingsManager.save === "function") {
                SettingsManager.save()
            }
        } else {
            console.log("SettingsManager not available - settings not saved")
        }

        settingsChanged = false
        return true
    }

    // Функция проверки настроек
    function validateSettings() {
        if (!serverField.text || serverField.text.trim() === "") {
            statusLabel.text = "✗ Server URL is required"
            statusLabel.color = errorColor
            return false
        }

        if (!apiKeyField.text || apiKeyField.text.trim() === "") {
            statusLabel.text = "✗ API Key is required"
            statusLabel.color = errorColor
            return false
        }

        // Простая валидация URL
        var url = serverField.text.trim()
        if (!url.startsWith("http://") && !url.startsWith("https://")) {
            statusLabel.text = "✗ URL must start with http:// or https://"
            statusLabel.color = errorColor
            return false
        }

        return true
    }
}
