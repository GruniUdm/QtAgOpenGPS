// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Link between the backend and QML. 
import QtQuick
import QtQuick.Controls.Fusion
import AOG

/* This type contains properties, signals, and functions to interface
   the C++ backend with the QML gui, while abstracting and limiting
   how much the C++ needs to know about the QML structure of the GUI,
   and how much the QML needs to know about the backends.

   This type also exposes the QSettings object for QML to use. However
   there are not change signals in the settings object, so we'll provide
   a signal here to let the backend know QML touched a setting.  Also
   a javascript function here that C++ can call to let QML objects know
   something in settings changed.

*/




Item {
    //id: aogInterfaceRoot  // Renamed to avoid conflict with global context property "aog"

    // ⚡ PHASE 6.3.0: Q_PROPERTY BINDINGS - Connect to FormGPS C++ Q_PROPERTY
    // This ensures InterfaceProperty and QML use the same source of truth

    // AOGTheme is already created in MainWindow.qml as 'theme'
    // We can access it directly since it's in the parent scope
    
    // Expose theme colors as aog properties for compatibility
    property color backgroundColor: theme.backgroundColor
    property color textColor: theme.textColor
    property color borderColor: theme.borderColor
    property color blackDayWhiteNight: theme.blackDayWhiteNight
    property color whiteDayBlackNight: theme.whiteDayBlackNight
    /*
    Connections {
        target: Settings
        function onMenu_isMetricChanged() {
            console.debug("isMetric is now", SettingsManager.menu_isMetric)
        }
    }
    */
    //Primarily these properties are updated by the backend c++ code
    //signals and data structures for specific UI functions are now
    //in the interfaces qml directory

    //property double frameTime: 0

    // ⚡ PHASE 6.3.0: Bind to FormGPS C++ Q_PROPERTY for unified source of truth
    // Use context property "aog" (FormGPS C++) instead of local id
    //property bool isJobStarted: aog.isJobStarted  // Context property, not local id
    //property bool blockageConnected: false
    // Qt 6.8 QProperty + BINDABLE: Simple properties to allow setProperty() updates from C++
    //property int manualBtnState: aog.manualBtnState
    //property int autoBtnState: 0
    //property bool autoYouturnBtnState: true
    //property bool isYouTurnRight: true
    //property bool autoTrackBtnState: false
    //property double distancePivotToTurnLine: -2222

    //property bool isPatchesChangingColor: false


    //sections 0-15 are used for on-screen buttons if
    //not using zones.  If using zones the rest are used
    //for mapping zones to sections

    //property variant sectionButtonState: [ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ]
    //property variant blockageRowCount: [ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 ]
    // Qt 6.8 QProperty + BINDABLE: Simple properties to allow setProperty() updates from C++
    //property bool isContourBtnOn: false
    //property bool btnIsContourLocked: false
    // Qt 6.8 QProperty + BINDABLE: Simple property to allow setProperty() updates from C++
    //property bool isBtnAutoSteerOn: false

    // ⚡ PHASE 6.0.20: BI-DIRECTIONAL SETTERS - Use context property "aog"
    // Now that id is renamed to aogInterfaceRoot, we can use "aog" context property
    // function setIsBtnAutoSteerOn(value) {
    //     aog.isBtnAutoSteerOn = value  // Context property (FormGPS C++)
    // }

    // 🚨 CRITICAL - Navigation & Guidance
    // function setCurrentABLine(value) {
    //     currentABLine = value
    //     aog.currentABLine = value  // Context property (FormGPS C++)
    // }
    // function setCurrentABCurve(value) {
    //     currentABCurve = value
    //     aog.currentABCurve = value  // Context property (FormGPS C++)
    // }
    // setSectionButtonState() REMOVED - Direct property binding used instead

    // ⚠️ MAJOR - Control Buttons
    // function setManualBtnState(value) {
    //     aog.manualBtnState = value  // Context property (FormGPS C++)
    // }
    // function setAutoBtnState(value) {
    //     aog.autoBtnState = value  // Context property (FormGPS C++)
    // }
    // function setAutoTrackBtnState(value) {
    //     aog.autoTrackBtnState = (value ? true : false)  // Context property (FormGPS C++)
    // }

    // 📊 INTERFACE
    function setPanMode(value) {
        panMode = value
        // panMode is local property, no aog equivalent needed
    }

    // 📈 USER STATISTICS with type conversion
    // function setDistanceUser(value) {
    //     aog.distanceUser = parseFloat(value)  // Context property (FormGPS C++)
    // }
    // function setWorkedAreaTotalUser(value) {
    //     aog.workedAreaTotalUser = parseFloat(value)  // Context property (FormGPS C++)
    // }

    // 🧪 SIMULATION/TEST
    // function setAvgPivDistance(value) {
    //     avgPivDistance = value
    //     // avgPivDistance is local property, no aog equivalent needed
    // }
    // Qt 6.8 QProperty + BINDABLE: Simple properties to allow setProperty() updates from C++
    //property bool isYouTurnBtnOn: false
    //property bool isYouTurnTriggered: false


    //property bool loopbackConnected: true

    //loopbackConnected is true, unless the backend changes it to false
    //onLoopbackConnectedChanged: closeAOG.open()
//    onIsBtnAutoSteerOnChanged: {
//        console.debug("isBtnAutoSteerOn is now in aog inface " + isBtnAutoSteerOn)
//    }

    //General FormGPS information updated at GPS rate.
    //property double latStart: aog.latStart
    //property double lonStart: aog.lonStart
    //property double easting: aog.easting
    //property double northing: aog.northing
    //property double latitude: aog.latitude
    //property double longitude: aog.longitude
    //property double heading: aog.heading//this is fix2fix heading
    //property double toolEasting: aog.toolEasting
    //property double toolNorthing: aog.toolNorthing
    //property double toolLatitude: aog.toolLatitude
    //property double toolLongitude: aog.toolLongitude
    //property double toolHeading: aog.toolHeading
    //property double imuRollDegrees: aog.imuRollDegrees
    //signal changeImuRoll(double new_roll) //usually just set to 88888;

    //property bool isReverse: false
    //property bool isReverseWithIMU: aog.isReverseWithIMU

    // Qt 6.8 QProperty + BINDABLE: Simple property to allow setProperty() updates from C++
    //property double speedKph: aog.speedKph
    //property double offlineDistance: 32000
    //property double avgPivDistance: 32000

    // Phase 6.0.20 Task 24 Step 3.2: Migrated to FormGPS Q_PROPERTY BINDABLE
    //property int steerModuleConnectedCounter: 0 → aog.steerModuleConnectedCounter
    //property bool steerSwitchHigh: false → aog.steerSwitchHigh (already migrated)

    property bool panMode: false
    onPanModeChanged: if(panMode == false)
                          centerOgl()

    //david added these
    //formgps_position.cpp line 1144
    //property double workedAreaTotal: 0
    // Qt 6.8 QProperty + BINDABLE: Simple properties to allow setProperty() updates from C++
    //property double workedAreaTotalUser: 0.0
    //property double actualAreaCovered: 0 //actual acres covered. Doesn't count overlap.
    //property double distanceUser: 0.0
    property double areaOuterBoundary: 0
    //property double areaBoundaryOuterLessInner: 0//outer minus the inner
    //property double altitude: 0
    //property double hdop: 0
    //property double age: 0
    //property int fixQuality: 0
    //property int satellitesTracked: 0

    //property double imuHeading: aog.imuHeading
    //signal changeImuHeading(double newImuHeading)

    //property int angVel: aog.imuAngVel//angular velocity I assume
    property string timeTilFinished: ""
    property string workRate: "value"
    property string percentOverlap: "value"
    property string percentLeft: "value"
    //property double steerAngleActual: aog.steerAngleActual
    //property double steerAngleSet: aog.steerAngleSet
    // Removed - now calculated with direct binding below
    //property double lblCalcSteerAngleInner: 0
    // DEAD CODE from C# original - lblCalcSteerAngleOuter never displayed in UI (FormSteerWiz.Designer.cs has no widget)
    // C# FormSteer.cs lines 335, 848, 854: all assignments commented out
    // TODO Phase 7: Remove all dead code comments
    //property double lblCalcSteerAngleOuter: 0
    //property double lblDiameter: 0
    //property bool startSA: false
    //property double rawHz: 0
    //property double hz: 0
    //property double droppedSentences: 0
    property double gpsHeading: 0 // to be implemented ???
    //property double fusedHeading: 0
    //property int sentenceCounter: 0 //for No GPS screen
    //property bool hydLiftDown: false
    property bool hydLiftIsOn: false
    //property bool isHeadlandOn: false
    //property int blockage_avg: 0
    //property int blockage_min1: 0
    //property int blockage_min2: 0
    //property int blockage_max: 0
    //property int blockage_min1_i: 0
    //property int blockage_min2_i: 0
    //property int blockage_max_i: 0
    //property bool blockage_blocked: false;
    //property bool imuCorrected: aog.imuCorrected
    // REMOVED lblimuCorrected - bug: converted bool to double instead of showing real IMU heading



    // MIGRATED to local properties - steerAngle*Rounded moved to SteerConfigWindow.qml
    //property double steerAngleActualRounded: Math.round(aog.steerAngleActual*100)/100
    //property double steerAngleSetRounded: Math.round(aog.steerAngleSet*100)/100
    //property int lblPWMDisplay: 0
    //property var vehicle_xy: Qt.point(0,0)
    //property var vehicle_bounding_box: Qt.rect(0,0,0,0)

    //property int lblmodeActualXTE: 0
    property double lblmodeActualHeadingError: 0 // dead code or to be implemed ???

    //onVehicle_xyChanged: console.log("vehicle xy is", vehicle_xy);
    //onVehicle_bounding_boxChanged: console.log("vehicle box is", vehicle_bounding_box);

    // Removed - now calculated with direct binding below

    // MIGRATED to local property - dispImuHeading moved to GPSData.qml
    //property string dispImuHeading: aog.imuHeading > 360 ? "#INV" : Number(aog.imuHeading.toLocaleString(Qt.locale(), 'f', 1))
    // Qt 6.8 QProperty + BINDABLE: Use direct binding instead of signal handler
    //property bool isTrackOn: (currentABLine > -1 && aog.isJobStarted === true)  // Use local currentABLine property movd to MainRightColumn.qml

    //can we use these line properties for the Display?
    //AB Lines properties, signals, and methods

    // Qt 6.8 COMPUTED PROPERTIES: Reactive bindings based on TracksInterface.idx and .mode
    // TrackMode enum: None=0, AB=2, Curve=4, bndTrackOuter=8, bndTrackInner=16, bndCurve=32, waterPivot=64
    readonly property int currentABLine: {
        let i = TracksInterface.idx
        if (i >= 0 && TracksInterface.mode === 2)  // TrackMode::AB = 2
            return i
        return -1
    }

    readonly property int currentABCurve: {
        let i = TracksInterface.idx
        if (i >= 0 && TracksInterface.mode === 4)  // TrackMode::Curve = 4
            return i
        return -1
    }

    property double currentABLine_heading: 0 //TODO delete or move to interfaces/LinesInterface.qml.  seems to be unused

    // AB Lines and Curves lists - restored for QML compatibility
    property var abLinesList: []
    property var abCurvesList: []

    property int current_trackNum: 0
    property int current_trackIdx: -1  // 🎯 ACTIVE TRACK - Access to CTrack.idx via FormGPS for YouTurn line switching

    //property int sensorData: -1

    // ⚡ PHASE 6.3.0: Q_PROPERTY CONVERSIONS - Missing AOGInterface properties
    // Added for conversion from InterfaceProperty to Q_PROPERTY
    // These properties replace the InterfaceProperty declarations in headers
    property double userSquareMetersAlarm: 0  // From classes/cfielddata.h:39

    //on-screen buttons

    //snap track buttons - REMOVED: Modernized to Q_INVOKABLE direct calls
    // REMOVED: signal snapSideways(double distance)//positive, right, negative, left
    // REMOVED: signal snapToPivot()

    // ✅ PHASE 6.0.20: ALL USER ACTION SIGNALS REMOVED - MODERNIZED TO Q_INVOKABLE CALLS
    // All display buttons, work modes, navigation, and utility signals have been
    // modernized to direct Q_INVOKABLE calls (aog.*, AOGRenderer.*, etc.)
    // following Qt 6.8 Rectangle Pattern architecture.
    //
    // REMOVED 34 LEGACY SIGNALS (Batches 1-14 completed):
    // - Graphics: zoomIn, zoomOut, tiltDown, tiltUp, btn2D, btn3D, n2D, n3D
    // - Tools: btnResetTool, btnHeadland, btnContour, btnContourLock, btnContourPriority
    // - Navigation: btnAutoTrack, uturn, lateral, autoYouTurn, swapAutoYouTurnDirection, btnResetCreatedYouTurn
    // - Vehicle: isHydLiftOn, isYouSkipOn
    // - Simulation: btnResetSim, sim_bump_speed, sim_zero_speed, sim_reset, sim_rotate, reset_direction
    // - Settings: settings_reload, settings_tempsave, settings_revert, settings_save
    // - Utility: centerOgl, deleteAppliedArea, btnFlag

    // was wizard - BATCH 12 signals removed - modernized to Q_INVOKABLE calls

    property bool isCollectingData: false
    //property int sampleCount: 0
    //property double confidenceLevel: 0
    //property bool hasValidRecommendation: false

    // Phase 6.0.20 Task 24 Step 3.5: Geodetic conversion functions removed
    // Now use aog.convertLocalToWGS84() and aog.convertWGS84ToLocal() Q_INVOKABLE methods
    // These delegate to CNMEA for single source of truth

}
