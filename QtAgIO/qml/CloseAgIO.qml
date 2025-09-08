import QtQuick
import QtQuick.Dialogs
import AgIO

MessageDialog {
    buttons: MessageDialog.Ok
    text: qsTr("AgIO is already running")
    onButtonClicked: Qt.quit()
}
