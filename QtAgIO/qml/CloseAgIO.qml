import QtQuick
import QtQuick.Dialogs
import AgIO 1.0

MessageDialog {
    buttons: MessageDialog.Ok
    text: qsTr("AgIO is already running")
    onButtonClicked: Qt.quit()
}
