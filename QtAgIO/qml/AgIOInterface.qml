import QtQuick
import QtQuick.Controls.Fusion
//import AgIO
/* This type contains properties, signals, and functions to interface
   the C++ backend with the QML gui, while abstracting and limiting
   how much the C++ needs to know about the QML structure of the GUI,
   and how much the QML needs to know about the backends.

   This type also exposes the QSettings object for QML to use. However
   there are not change signals in the AgIOSettings object, so we'll provide
   a signal here to let the backend know QML touched a setting.  Also
   a javascript function here that C++ can call to let QML objects know
   something in AgIOSettings changed.

   MIGRATION NOTE: Phase 4.1 - Modified to use AgIOService direct properties
   for real-time data instead of static values.
*/




Item {
    id: agioInterfaceType

	// Connection status - direct AgIOService properties for real-time updates
	property bool ethernetConnected: AgIOService.ethernetConnected
	property bool ntripConnected: AgIOService.ntripConnected
	property bool aogConnected: false // TODO: Add to AgIOService if needed
	property bool steerConnected: false // TODO: Map to AgIOService.moduleConnected
	property bool gpsConnected: AgIOService.gpsConnected
	property bool imuConnected: false // TODO: Add IMU connection status to AgIOService
	property bool machineConnected: false // TODO: Add machine connection status to AgIOService
    property bool blockageConnected: false // TODO: Add blockage connection status to AgIOService
    property bool bluetoothConnected: AgIOService.bluetoothConnected

    // NTRIP status - direct AgIOService properties for real-time monitoring
    property int ntripStatus: AgIOService.ntripStatus
    property string ntripStatusText: AgIOService.ntripStatusText
    property int tripBytes: 0 // TODO: Add totalNTRIPBytes to AgIOService
    property int ntripCounter: 0 // TODO: Add NTRIP message counter to AgIOService
    property int rawTripCount: 0 // TODO: Add raw NTRIP count to AgIOService


	// GPS/NMEA data - direct AgIOService properties for real-time updates
	property double latitude: AgIOService.latitude
	property double longitude: AgIOService.longitude
	property double altitude: 0 // TODO: Add altitude to AgIOService if available
	property double speed: AgIOService.speed
    property double gpsHeading: AgIOService.heading
    property double dualHeading: 0 // TODO: Add dual heading to AgIOService if needed
    property double imuHeading: AgIOService.imuYaw
    property double imuRoll: AgIOService.imuRoll
    property double imuPitch: AgIOService.imuPitch
    property double age: AgIOService.age
    property int hdop: 0 // TODO: Add HDOP to AgIOService if available
	property int quality: AgIOService.gpsQuality
	property int sats: AgIOService.satellites
	property double yawrate: 0 // TODO: Add yaw rate to AgIOService if needed
    property double gpsHz: 0 // TODO: Add GPS frequency to AgIOService if needed
    property double nowHz: 0 // TODO: Add current Hz to AgIOService if needed

	property string gga: ""
	property string vtg: ""
	property string panda: ""
	property string paogi: ""
	property string hdt: ""
	property string avr: ""
	property string hpd: ""
	property string sxt: ""
    property string unknownSentence: ""

    property int nmeaError: 0 // triggers if altitude changes drastically--a sign of 2 separate nmea strings

    //these are signals that get sent to the backend
    signal btnSendSubnet_clicked();
    signal ntripDebug(bool doWeDebug);
    signal setIPFromUrl(string url);
    signal configureNTRIP();


    //advanced Menu triggers
    signal btnUDPListenOnly_clicked(bool isIt);

    //bluetooth
    signal bt_search(string device)
    signal bt_kill()
    signal bt_remove_device(string device)
    property string connectedBTDevices: ""
    signal bluetoothDebug(bool doWeDebug);
    signal startBluetoothDiscovery()


}

