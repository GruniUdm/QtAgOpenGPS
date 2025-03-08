import QtQuick
import AgIO 1.0

//this file only for test when using AgIO as standalone app for test

Window {
  id: root
  width: 480
  height: 480
  visible: true
  title: qsTr("AgIO test as a standaolone app")

  AgIO {
    //id: agIO

    anchors.fill: parent
  }
}
