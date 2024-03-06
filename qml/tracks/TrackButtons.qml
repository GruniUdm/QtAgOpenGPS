import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

import ".."

TimedRectangle {
    id: trackButtons
    color: "white"
    width: 110*7
    height: 110
    RowLayout{
        anchors.fill: parent
        anchors.margins: 5
        IconButtonTransparent{
            id: marker
            icon.source: "/images/ABSnapNudgeMenuRef.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: refNudge.show()
        }
        IconButtonColor{
            icon.source: "/images/AutoSteerSnapToPivot.png"
            implicitWidth: marker.width
            implicitHeight: marker.width
            Layout.alignment: Qt.AlignCenter
        }
        IconButtonTransparent{
            icon.source: "/images/SwitchOff.png"
            Layout.alignment: Qt.AlignCenter
        }
        IconButtonTransparent{
            icon.source: "/images/ABTracks.png"
            Layout.alignment: Qt.AlignCenter
        }
        IconButtonTransparent{
            icon.source: "/images/AddNew.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: trackNew.show()
        }
        IconButtonTransparent{
            icon.source: "/images/ABDraw.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: lineDrawer.show()
        }
        IconButtonTransparent{
            icon.source: "/images/ABSnapNudgeMenu.png"
            Layout.alignment: Qt.AlignCenter
            onClicked: lineNudge.show()
        }
    }
}