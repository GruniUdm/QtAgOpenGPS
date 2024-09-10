import QtQuick
import QtQuick.Controls.Fusion

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
    id: agioInterfaceType

	//These are the status booleans for the various connections.
	property bool ethernetConnected: false
	property bool ntripConnected: false
	property bool aogConnected: false
	property bool steerConnected: false
	property bool gpsConnected: false
	property bool imuConnected: false
	property bool machineConnected: false

    property int ntripStatus: 0

	//NMEA variables
	property double latitude: 0
	property double longitude: 0
	property double altitude: 0
	property double speed: 0
	property double heading: 0
	property double imuheading: 0
	property double imuroll: 0
	property double imupitch: 0
	property double age: 0
	property int quality: 0
	property int sats: 0
	property double yawrate: 0
	property double hdop: 0

	property string gga: ""
	property string vtg: ""
	property string panda: ""
	property string paogi: ""
	property string hdt: ""
	property string avr: ""
	property string hpd: ""
	property string sxt: ""

    property int nmeaError: 0 // triggers if altitude changes drastically--a sign of 2 separate nmea strings


    //these are signals that get sent to the backend
    signal btnSendSubnet_clicked();
    signal ntripDebug(bool doWeDebug);
    signal setIPFromUrl(string url);
    signal configureNTRIP();



}
