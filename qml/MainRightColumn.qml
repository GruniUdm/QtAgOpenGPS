import QtQuick
import QtQuick.Layouts
//import Settings
// Interface import removed - now QML_SINGLETON
import AOG
import "components" as Comp

ColumnLayout {
    id: rightColumn //buttons

    visible: aog.isJobStarted


    onHeightChanged: {
        theme.btnSizes[0] = height / (children.length)
        theme.buttonSizesChanged()
    }
    onVisibleChanged: if(visible === false)
                          width = 0
                      else
                          width = children.width

    Comp.MainWindowBtns {
        property bool isContourLockedByUser //store if user locked
        id: btnContourLock
        isChecked: aog.btnIsContourLocked
        visible: btnContour.checked
        checkable: true
        icon.source: prefix + "/images/ColorUnlocked.png"
        iconChecked: prefix + "/images/ColorLocked.png"
        buttonText: qsTr("Lock")
        onClicked: {
            aog.contourLock() // Qt 6.8 MODERN: Direct Q_INVOKABLE call
            if (aog.btnIsContourLocked)
                isContourLockedByUser = true
        }
        // Connections{
        //     target: aogInterface
        //     function onBtnIsContourLockedChanged() {
        //         btnContourLock.checked = aog.btnIsContourLocked
        //         if(btnContourLock.isContourLockedByUser)
        //             btnContourLock.isContourLockedByUser = false
        //     }
        //     // function onIsBtnAutoSteerOnChanged() {
        //     //     if (!btnContourLock.isContourLockedByUser && btnContour.checked === true){
        //     //         if(btnContourLock.checked !== aog.isBtnAutoSteerOn){
        //     //             aog.contourLock() // Qt 6.8 MODERN: Direct Q_INVOKABLE call
        //     //         }
        //     //     }
        //     // }
        // }
    }
    Comp.MainWindowBtns {
        id: btnContour
        isChecked: aog.isContourBtnOn // Qt 6.8 MODERN: Q_PROPERTY access
        checkable: true
        icon.source: prefix + "/images/ContourOff.png"
        iconChecked: prefix + "/images/ContourOn.png"
        buttonText: qsTr("Contour")
        onClicked: {
            aog.contour() // Qt 6.8 MODERN: Direct Q_INVOKABLE call
        }
        onCheckedChanged: { //gui logic
            btnTrackCycle.visible = !checked
            btnTrackCycleBk.visible = !checked
            btnContourPriority.visible = checked
        }
    }

    Comp.IconButton{
        id: btnTrackCycle
        icon.source: prefix + "/images/ABLineCycle.png"
        Layout.alignment: Qt.AlignCenter
        implicitWidth: theme.buttonSize
        implicitHeight: theme.buttonSize

        onClicked: {
            if (TracksInterface.idx > -1) {
                TracksInterface.next()
            }
        }
    }
    Comp.IconButton{
        id: btnTrackCycleBk
        icon.source: prefix + "/images/ABLineCycleBk.png"
        Layout.alignment: Qt.AlignCenter
        implicitWidth: theme.buttonSize
        implicitHeight: theme.buttonSize

        onClicked: {
            if (TracksInterface.idx > -1) {
                TracksInterface.prev()
            }
        }

    }
    Comp.IconButton{
        id: btnAutoTrack
        checkable: true
        isChecked: aog.autoTrackBtnState
        icon.source: prefix + "/images/AutoTrackOff.png"
        iconChecked: prefix + "/images/AutoTrack.png"
        Layout.alignment: Qt.AlignCenter
        implicitWidth: theme.buttonSize
        implicitHeight: theme.buttonSize
        onCheckedChanged: aog.autoTrackBtnState = checked ? 1 : 0 // Qt 6.8 MODERN: Q_PROPERTY assignment
        onClicked: aog.autoTrack() // Qt 6.8 MODERN: Direct Q_INVOKABLE call
    }

    Comp.MainWindowBtns {
        id: btnSectionManual
        isChecked: aog.manualBtnState == 2 // Qt 6.8 MODERN: Q_PROPERTY access
        checkable: true
        icon.source: prefix + "/images/ManualOff.png"
        iconChecked: prefix + "/images/ManualOn.png"
        buttonText: qsTr("Manual")
        onCheckedChanged: {
            if (checked) {
                btnSectionAuto.checked = false;
                sectionButtons.setAllSectionsToState(2 /*auto*/);
                aog.manualBtnState = 2 // Qt 6.8 MODERN: btnStates::on

            } else {
                sectionButtons.setAllSectionsToState(0 /*off*/);
                aog.manualBtnState = 0 // Qt 6.8 MODERN: btnStates::off
            }
        }
    }

    Comp.MainWindowBtns {
        id: btnSectionAuto
        isChecked: aog.autoBtnState == 1 // Qt 6.8 MODERN: Q_PROPERTY access
        checkable: true
        icon.source: prefix + "/images/SectionMasterOff.png"
        iconChecked: prefix + "/images/SectionMasterOn.png"
        buttonText: qsTr("Auto")
        onCheckedChanged: {
            if (checked) {
                btnSectionManual.checked = false;
                sectionButtons.setAllSectionsToState(1 /*auto*/);
                aog.autoBtnState = 1 // Qt 6.8 MODERN: btnStates::auto
            } else {
                sectionButtons.setAllSectionsToState(0 /*off*/);
                aog.autoBtnState = 0 // Qt 6.8 MODERN: btnStates::off
            }
        }
    }
    Comp.MainWindowBtns {
        id: btnAutoYouTurn
        isChecked: aog.isYouTurnBtnOn // Qt 6.8 MODERN: Q_PROPERTY access
        checkable: true
        icon.source: prefix + "/images/YouTurnNo.png"
        iconChecked: prefix + "/images/YouTurn80.png"
        buttonText: qsTr("AutoUturn")
        visible: TracksInterface.idx > -1
        //enabled: aog.isBtnAutoSteerOn
        onClicked: aog.autoYouTurn() // Qt 6.8 MODERN: Direct Q_INVOKABLE call
    }
    Comp.MainWindowBtns {
        id: btnAutoSteer
        icon.source: prefix + "/images/AutoSteerOff.png"
        iconChecked: prefix + "/images/AutoSteerOn.png"
        checkable: true
        isChecked: Backend.mainWindow.isBtnAutoSteerOn  // ⚡ PHASE 6.0.20 FIX: Use isChecked for bidirectional binding (sync with C++ protection)
        // ⚡ PHASE 6.0.20 FIX: Require ACTIVE line (not just in memory) - currentABLine/Curve check mode === AB/Curve
        enabled: ((aogInterface.currentABLine > -1 || aogInterface.currentABCurve > -1) || aog.isContourBtnOn) && aog.isJobStarted
        //Is remote activation of autosteer enabled? //todo. Eliminated in 6.3.3
        // Threading Phase 1: Auto steer mode display
        buttonText: (SettingsManager.as_isAutoSteerAutoOn ? "R" : "M")

        onClicked: {
            // ⚡ PHASE 6.0.20 FIX: Check ACTIVE line (not just in memory)
            if ((aogInterface.currentABLine > -1 || aogInterface.currentABCurve > -1) || btnContour.isChecked) {
                Backend.mainWindow.isBtnAutoSteerOn = !Backend.mainWindow.isBtnAutoSteerOn; // Qt 6.8 MODERN: Q_PROPERTY assignment
            } else {
                // No active line or contour: don't allow AutoSteer
                Backend.mainWindow.isBtnAutoSteerOn = false; // Qt 6.8 MODERN: Q_PROPERTY assignment
            }
        }

        //property bool isTrackOn: (currentABLine > -1 && aog.isJobStarted === true)  //

        // ⚡ PHASE 6.0.20: Speed-based AutoSteer deactivation MOVED to C++
        // Logic now in formgps.cpp:175 setSpeedKph() for better architecture
        // Automatic protection in both simulation and real GPS modes
        /*
        Connections {
            target: aogInterface
            // function onIsBtnAutoSteerOnChanged() {
            //     //TODO: use track interface in trk
            //     if (aog.isBtnAutoSteerOn) {
            //         btnAutoSteer.checked = true
            //     } else {
            //         //default to turning everything off
            //         btnAutoSteer.checked = false
            //     }
            // }
            function onSpeedKphChanged() {
                if (btnAutoSteer.checked) {
                    if (aog.speedKph < SettingsManager.as_minSteerSpeed) { // Qt 6.8 MODERN: Q_PROPERTY access
                        aog.isBtnAutoSteerOn = false
                    } else if (aog.speedKph > SettingsManager.as_maxSteerSpeed) { // Qt 6.8 MODERN: Q_PROPERTY access
                        //timedMessage
                        aog.isBtnAutoSteerOn = false
                    }
                }
            }
        }
        */
    }
}
