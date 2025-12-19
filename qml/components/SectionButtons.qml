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

    color: "transparent"

    property bool triState: true

    // Qt 6.8 QProperty + BINDABLE: Interface properties for setProperty() compatibility
    property bool toolIsSectionsNotZones: SettingsManager.tool_isSectionsNotZones
    property int vehicleNumSections: SettingsManager.vehicle_numSections
    property string toolZones: SettingsManager.tool_zones

    // Threading Phase 1: Section vs zones configuration
    property int numSections: (toolIsSectionsNotZones ? vehicleNumSections : parseInt(toolZones.split(',')[0]))
    property var buttonState: [ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ]
    property color offColor: "Crimson"
    property color offTextColor: "White"
    property color onColor: "DarkGoldenrod"
    property color onTextColor: "White"
    property color autoColor: "ForestGreen"
    property color autoTextColor: "White"

    signal sectionClicked(int whichsection)

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

    function setSectionState (sectionNo: int, new_state: int) {
        // Qt 6.8 BINDABLE: Direct property binding - no function calls
        // states: 0 = off, 1 = auto, 2 = on
        console.log("DEBUG setSectionState CALLED: section", sectionNo, "new_state", new_state);

        // Protection against invalid indices
        if (sectionNo < 0 || sectionNo >= vehicleNumSections) {
            console.error("setSectionState: Invalid sectionNo", sectionNo, "max:", vehicleNumSections);
            return;
        }

        // Create copies of current arrays for direct property assignment
        var newBackendState = aog.sectionButtonState.slice() // Copy local AOGInterface array
        var newButtonState = sectionButtons.buttonState.slice()       // Copy UI array

        if (toolIsSectionsNotZones) {
            // 1:1 correlation between buttons and sections
            if (sectionNo < newBackendState.length && sectionNo < newButtonState.length) {
                newBackendState[sectionNo] = new_state // Backend tool array
                newButtonState[sectionNo] = new_state   // UI button array
            }
        } else {
            // Zones mode - one button controls multiple sections
            if (sectionNo < newButtonState.length) {
                newButtonState[sectionNo] = new_state // UI button
            }

            // Parse zone configuration
            var zones = toolZones.split(',');
            if (zones.length === 0) {
                console.error("setSectionState: Empty toolZones");
                return;
            }

            // Calculate zone boundaries
            var zone_left = (sectionNo === 0) ? 0 : parseInt(zones[sectionNo]) || 0;
            var zone_right = (sectionNo + 1 < zones.length) ?
                            parseInt(zones[sectionNo + 1]) || 0 :
                            newBackendState.length;

            // Update all sections in the zone
            if (zone_left >= 0 && zone_right > zone_left && zone_right <= newBackendState.length) {
                for (var j = zone_left; j < zone_right; j++) {
                    newBackendState[j] = new_state
                }
            } else {
                console.error("setSectionState: Invalid zone bounds", zone_left, zone_right);
                return;
            }
        }

        // Pure property binding - direct assignment instead of function call
        console.log("DEBUG: Before assignment - newBackendState[" + sectionNo + "] =", newBackendState[sectionNo]);
        aog.sectionButtonState = newBackendState
        sectionButtons.buttonState = newButtonState
        console.log("DEBUG: Property assignment completed");
    }

    function setAllSectionsToState(new_state: int) {
        // Qt 6.8 BINDABLE: Direct property binding for bulk updates
        // states: 0 = off, 1 = auto, 2 = on

        // Create copies of current arrays for direct property assignment
        var newBackendState = aog.sectionButtonState.slice() // Copy local AOGInterface array
        var newButtonState = sectionButtons.buttonState.slice()       // Copy UI array

        if (toolIsSectionsNotZones) {
            // 1:1 correlation between buttons and sections
            for (var i = 0; i < 65; i++) {
                if (i < numSections) {
                    newBackendState[i] = new_state // Active sections
                } else {
                    newBackendState[i] = 0 // Inactive sections set to Off
                }

                if (i < 16) {
                    newButtonState[i] = (i < numSections) ? new_state : 0 // UI buttons
                }
            }
        } else {
            // Zones mode - buttons control multiple sections
            var zones = toolZones.split(',')

            for (var i = 0; i < numSections; i++) {
                newButtonState[i] = new_state // UI button

                // Calculate zone boundaries
                var zone_left = (i === 0) ? 0 : parseInt(zones[i]) || 0;
                var zone_right = (i + 1 < zones.length) ?
                                parseInt(zones[i + 1]) || 0 :
                                newBackendState.length;

                // Update all sections in the zone
                for (var j = zone_left; j < zone_right; j++) {
                    if (j < newBackendState.length) {
                        newBackendState[j] = new_state
                    }
                }
            }
        }

        // Pure property binding - direct assignment instead of function call
        aog.sectionButtonState = newBackendState
        sectionButtons.buttonState = newButtonState
    }

    onNumSectionsChanged: {
        buttonModel.clear()
        for (var i = 0; i < numSections; i++) {
            buttonModel.append( { sectionNo: i } )
        }
    }

    //callbacks, connections, and signals
    Component.onCompleted:  {
        console.log("COMPONENT COMPLETED: toolIsSectionsNotZones =", toolIsSectionsNotZones, "vehicleNumSections =", vehicleNumSections, "toolZones =", toolZones, "numSections =", numSections);
        setColors()
        buttonModel.clear()
        for (var i = 0; i < numSections; i++) {
            buttonModel.append( { sectionNo: i } )
        }
    }

    // Qt 6.8 QProperty + BINDABLE: Direct property binding replaces signal handler

    ListModel {
        id: buttonModel
        //this will hold the individual buttons
        //{ sectionNo: button_number }
    }

    Component {
        id: sectionViewDelegate
        SectionButton {
            width: (sectionButtons.width / numSections) > 40 ? (sectionButtons.width / numSections) : 40
            buttonText: (model.sectionNo + 1).toFixed(0)
            visible: (model.sectionNo < numSections) ? true : false
            color: (sectionButtons.buttonState[model.sectionNo]===0 ? offColor : (sectionButtons.buttonState[model.sectionNo] === 1 ? autoColor : onColor))
            textColor: (sectionButtons.buttonState[model.sectionNo]===0 ? offTextColor : (sectionButtons.buttonState[model.sectionNo] === 1 ? autoTextColor : onTextColor))

            onButtonClicked: {
                //emit signal
                sectionClicked(model.sectionNo)
                //set the state here
                setSectionState(model.sectionNo, (sectionButtons.buttonState[model.sectionNo] + 1 ) % 3)
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

        model: buttonModel
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
