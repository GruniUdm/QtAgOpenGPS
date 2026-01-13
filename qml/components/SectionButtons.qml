// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
import QtQuick
import QtQuick.Controls.Fusion
//import Settings
import AOG

//This is a the row of on-screen section-control buttonw

Rectangle {
    id: sectionButtons
    objectName: "sectionButtons"

    /*
    MockSettings { //for testing with qmlscene only
        id: settings
    }

    AOGInterface { //for testing with qmlscene only
        id: aog
    }*/

    width: 600
    height: childrenRect.height * theme.scaleHeight

    color: "red"

    property color offColor: "Crimson"
    property color offTextColor: "White"
    property color onColor: "DarkGoldenrod"
    property color onTextColor: "White"
    property color autoColor: "ForestGreen"
    property color autoTextColor: "White"

    //methods
    function setColors() {
        //same colors for sections and zones
        if (theme.displayIsDayMode) {
            sectionButtons.offColor = "Red"
            sectionButtons.offTextColor = "Black"

            sectionButtons.autoColor = "Lime"
            sectionButtons.autoTextColor = "Black"

            sectionButtons.onColor = "Yellow"
            sectionButtons.onTextColor = "Black"
        } else {
            sectionButtons.offColor = "Crimson"
            sectionButtons.offTextColor = "White"

            sectionButtons.autoColor = "ForestGreen"
            sectionButtons.autoTextColor = "White"

            sectionButtons.onColor = "DarkGoldenRod"
            sectionButtons.onTextColor = "White"
        }
    }

    onVisibleChanged: {
        console.debug(qmlLog, "visibility changed to ", visible);
        console.debug(qmlLog, "Tools model information:");
        console.debug(qmlLog, Tools.toolsList[0].sectionButtonsModel.rowCount());
    }

    Component {
        id: sectionViewDelegate
        SectionButton {

            Component.onCompleted: {
                console.log(qmlLog, model);

            }
            property int numSections: Tools.toolsList[0].sectionButtonsModel.rowCount()
            width: (sectionButtons.width / numSections) > 40 ? (sectionButtons.width / numSections) : 40
            buttonText: (model.buttonNumber + 1).toFixed(0)
            visible: (model.buttonNumber < numSections) ? true : false
            color: (model.state === 0 ? offColor : (model.state === 1 ? autoColor : onColor))
            textColor: (model.state ===0 ? offTextColor : (model.state === 1 ? autoTextColor : onTextColor))

            onButtonClicked: {
                //toggle tri state
                Tools.setSectionButtonState(0,model.buttonNumber, (model.state + 1 ) % 3)
                console.debug(qmlLog, "button clicked: ",model.buttonNumber, " new state is ", model.state);
            }
        }
    }

    ListView {
        id: sectionButtonList
        orientation: Qt.Horizontal
        width: parent.width
        height: 40
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        model: Tools.toolsList[0].sectionButtonsModel

        Component.onCompleted:  {
            console.debug(qmlLog, "Tools model information:");
            console.debug(qmlLog, Tools.toolsList[0].sectionButtonsModel.rowCount());

        }

        boundsMovement: Flickable.StopAtBounds

        ScrollBar.horizontal: ScrollBar {
            parent: sectionButtonList.parent
            anchors.top: sectionButtonList.bottom
            anchors.left: sectionButtonList.left
            anchors.right: sectionButtonList.right
        }

        clip: true

        delegate: sectionViewDelegate
        flickableDirection: Flickable.HorizontalFlick
    }
}
