import QtQuick
import QtQuick.Controls.Fusion
import AgIO 1.0
//This is a the row of on-screen section-control buttonw

Rectangle {
    id: testing
    width: 900
    height: 600

    MockSettings { //for testing with qmlscene only
        id: agiosettings
    }

    AOGInterface { //for testing with qmlscene only
        id: aog
    }

    SectionButtons {
        id: sb
        triState: false

        anchors.top: parent.top
        anchors.left: parent.left

        width: 800

    }

    Button {
        anchors.top: sb.bottom
        anchors.left: parent.left
        width: 100
        height: 20
        text: "Click me"

        onClicked: {
            agiosettings.setTool_isSectionsNotZones = ! agiosettings.setTool_isSectionsNotZones
            console.debug("numSections is " + sb.numSections)
        }

    }
}
