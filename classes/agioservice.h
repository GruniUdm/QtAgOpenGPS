#ifndef AGIOSERVICE_H
#define AGIOSERVICE_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QPointF>
#include <QQmlEngine>
#include <QtQml>

#include "gpsworker.h"
#include "ntripworker.h" 
#include "udpworker.h"
#include "serialworker.h"

/**
 * @brief Real-time AgIO service for GPS, NTRIP, and hardware communication
 * 
 * This class runs in the main/UI thread to provide ZERO-latency access
 * to position data for OpenGL rendering and AutoSteer functionality.
 * I/O operations are handled by worker threads.
 */
class AgIOService : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    
    // Real-time position data (direct OpenGL access)
    Q_PROPERTY(double latitude MEMBER m_latitude NOTIFY gpsDataChanged)
    Q_PROPERTY(double longitude MEMBER m_longitude NOTIFY gpsDataChanged)
    Q_PROPERTY(double heading MEMBER m_heading NOTIFY gpsDataChanged)
    Q_PROPERTY(double speed MEMBER m_speed NOTIFY gpsDataChanged)
    Q_PROPERTY(double altitude MEMBER m_altitude NOTIFY gpsDataChanged)
    Q_PROPERTY(double hdop MEMBER m_hdop NOTIFY gpsDataChanged)
    Q_PROPERTY(QPointF vehicle_xy MEMBER m_vehicleXY NOTIFY vehiclePositionChanged)
    
    // IMU data (real-time for AutoSteer)
    Q_PROPERTY(double imuRoll MEMBER m_imuRoll NOTIFY imuDataChanged)
    Q_PROPERTY(double imuPitch MEMBER m_imuPitch NOTIFY imuDataChanged)
    Q_PROPERTY(double imuYaw MEMBER m_imuYaw NOTIFY imuDataChanged)
    
    // GPS/Dual antenna roll (separate from IMU roll)
    Q_PROPERTY(double rollGPS MEMBER m_rollGPS NOTIFY gpsDataChanged)
    
    // GPS status
    Q_PROPERTY(bool gpsConnected MEMBER m_gpsConnected NOTIFY gpsStatusChanged)
    Q_PROPERTY(int gpsQuality MEMBER m_gpsQuality NOTIFY gpsStatusChanged)
    Q_PROPERTY(int satellites MEMBER m_satellites NOTIFY gpsStatusChanged)
    Q_PROPERTY(double age MEMBER m_age NOTIFY gpsStatusChanged)
    
    // Connection status
    Q_PROPERTY(bool bluetoothConnected MEMBER m_bluetoothConnected NOTIFY bluetoothStatusChanged)
    Q_PROPERTY(QString bluetoothDevice MEMBER m_bluetoothDevice NOTIFY bluetoothStatusChanged)
    Q_PROPERTY(bool ethernetConnected MEMBER m_ethernetConnected NOTIFY ethernetStatusChanged)
    Q_PROPERTY(bool ntripConnected READ ntripConnected NOTIFY ntripStatusChanged)
    
    // NTRIP status  
    Q_PROPERTY(int ntripStatus MEMBER m_ntripStatus NOTIFY ntripStatusChanged)
    Q_PROPERTY(QString ntripStatusText READ ntripStatusText NOTIFY statusChanged)
    Q_PROPERTY(int rawTripCount MEMBER m_rawTripCount NOTIFY ntripStatusChanged)
    
    // Additional connection status for QML compatibility
    Q_PROPERTY(bool imuConnected MEMBER m_imuConnected NOTIFY imuStatusChanged)
    Q_PROPERTY(bool steerConnected MEMBER m_steerConnected NOTIFY steerStatusChanged)
    Q_PROPERTY(bool machineConnected MEMBER m_machineConnected NOTIFY machineStatusChanged)
    Q_PROPERTY(bool blockageConnected MEMBER m_blockageConnected NOTIFY blockageStatusChanged)
    
    // Settings properties (matching original QMLSettings interface)
    Q_PROPERTY(QString setNTRIP_url MEMBER m_ntripUrl NOTIFY settingsChanged)
    Q_PROPERTY(QString setNTRIP_userName MEMBER m_ntripUser NOTIFY settingsChanged)
    Q_PROPERTY(QString setNTRIP_userPassword MEMBER m_ntripPassword NOTIFY settingsChanged)
    Q_PROPERTY(QString setNTRIP_mount MEMBER m_ntripMount NOTIFY settingsChanged)
    Q_PROPERTY(int setNTRIP_casterPort MEMBER m_ntripPort NOTIFY settingsChanged)
    Q_PROPERTY(bool setNTRIP_isOn MEMBER m_ntripEnabled NOTIFY settingsChanged)
    
    // Serial communication settings
    Q_PROPERTY(QString setGnss_SerialPort MEMBER m_gnssSerialPort NOTIFY settingsChanged)
    Q_PROPERTY(int setGnss_BaudRate MEMBER m_gnssBaudRate NOTIFY settingsChanged)
    Q_PROPERTY(QString setImu_SerialPort MEMBER m_imuSerialPort NOTIFY settingsChanged)
    Q_PROPERTY(int setImu_BaudRate MEMBER m_imuSerialBaud NOTIFY settingsChanged)
    Q_PROPERTY(QString setSteer_SerialPort MEMBER m_steerSerialPort NOTIFY settingsChanged)
    Q_PROPERTY(int setSteer_BaudRate MEMBER m_steerSerialBaud NOTIFY settingsChanged)
    
    // Bluetooth settings
    Q_PROPERTY(bool setBluetooth_isOn MEMBER m_bluetoothEnabled NOTIFY settingsChanged)
    Q_PROPERTY(QVariantList setBluetooth_deviceList MEMBER m_bluetoothDevices NOTIFY settingsChanged)
    
    // Network settings
    Q_PROPERTY(int setUDP_IP1 MEMBER m_udpIP1 NOTIFY settingsChanged)
    Q_PROPERTY(int setUDP_IP2 MEMBER m_udpIP2 NOTIFY settingsChanged)
    Q_PROPERTY(int setUDP_IP3 MEMBER m_udpIP3 NOTIFY settingsChanged)
    Q_PROPERTY(QString setNTRIP_ipAddress READ ntripIPAddress NOTIFY settingsChanged)
    
    // Display settings
    Q_PROPERTY(bool setMenu_isMetric MEMBER m_isMetric NOTIFY settingsChanged)
    Q_PROPERTY(bool setDisplay_isDayMode MEMBER m_isDayMode NOTIFY settingsChanged)
    
    // Tool settings
    Q_PROPERTY(bool setTool_isSectionsNotZones MEMBER m_isSectionsNotZones NOTIFY settingsChanged)
    Q_PROPERTY(QVariantList setTool_zones MEMBER m_toolZones NOTIFY settingsChanged)
    Q_PROPERTY(QVariantList setKey_hotkeys MEMBER m_hotkeys NOTIFY settingsChanged)
    
    // Additional GPS properties for QML compatibility
    Q_PROPERTY(int sats MEMBER m_satellites NOTIFY gpsStatusChanged)  // Alias for satellites
    Q_PROPERTY(int quality MEMBER m_gpsQuality NOTIFY gpsStatusChanged)  // Alias for gpsQuality
    Q_PROPERTY(double gpsHeading MEMBER m_heading NOTIFY gpsDataChanged)  // Alias for heading
    Q_PROPERTY(double dualHeading MEMBER m_dualHeading NOTIFY gpsDataChanged)  // Dual antenna heading
    Q_PROPERTY(double gpsHz MEMBER m_gpsHz NOTIFY statusChanged)  // GPS frequency
    Q_PROPERTY(double nowHz MEMBER m_nowHz NOTIFY statusChanged)  // Current Hz
    Q_PROPERTY(double yawrate MEMBER m_yawRate NOTIFY imuDataChanged)  // IMU yaw rate
    
    // NMEA sentence strings for debugging
    Q_PROPERTY(QString gga MEMBER m_ggaSentence NOTIFY statusChanged)
    Q_PROPERTY(QString vtg MEMBER m_vtgSentence NOTIFY statusChanged)
    Q_PROPERTY(QString panda MEMBER m_pandaSentence NOTIFY statusChanged)
    Q_PROPERTY(QString paogi MEMBER m_paogiSentence NOTIFY statusChanged)
    Q_PROPERTY(QString hdt MEMBER m_hdtSentence NOTIFY statusChanged)
    Q_PROPERTY(QString avr MEMBER m_avrSentence NOTIFY statusChanged)
    Q_PROPERTY(QString hpd MEMBER m_hpdSentence NOTIFY statusChanged)
    Q_PROPERTY(QString sxt MEMBER m_sxtSentence NOTIFY statusChanged)
    Q_PROPERTY(QString unknownSentence MEMBER m_unknownSentence NOTIFY statusChanged)
    
    // Enhanced UI Integration (Phase 4.5.4)
    Q_PROPERTY(QString gpsStatusText READ gpsStatusText NOTIFY statusChanged)
    Q_PROPERTY(QString moduleStatusText READ moduleStatusText NOTIFY statusChanged)
    Q_PROPERTY(QString serialStatusText READ serialStatusText NOTIFY statusChanged)
    Q_PROPERTY(bool showErrorDialog READ showErrorDialog NOTIFY errorOccurred)
    Q_PROPERTY(QString lastErrorMessage READ lastErrorMessage NOTIFY errorOccurred)
    Q_PROPERTY(QVariantList discoveredModules READ discoveredModules NOTIFY moduleDiscoveryChanged)

public:
    ~AgIOService();

    // C++ singleton access (strict singleton pattern - same as CTrack/CVehicle)
    static AgIOService* instance() {
        static AgIOService* s_instance = new AgIOService(nullptr);
        return s_instance;
    }
    
    // Direct access methods for OpenGL (ZERO latency)
    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }
    double heading() const { return m_heading; }
    
    // GPS quality interpretation (like FormLoop original)
    Q_INVOKABLE QString fixQuality() const;
    double speed() const { return m_speed; }       // Direct access for GPS speed
    double altitude() const { return m_altitude; }  // GPS altitude
    double hdop() const { return m_hdop; }          // GPS accuracy
    int satellites() const { return m_satellites; }  // Number of satellites
    double gpsHz() const { return m_gpsHz; }        // GPS frequency
    double nowHz() const { return m_nowHz; }        // Current Hz
    QPointF vehicleXY() const { return m_vehicleXY; }
    double easting() const { return m_easting; }    // Direct access for OpenGL
    double northing() const { return m_northing; }  // Direct access for OpenGL
    
    // IMU data getters (Qt 6 best practice - explicit getters)
    double imuRoll() const { return m_imuRoll; }
    double imuPitch() const { return m_imuPitch; }
    double imuYaw() const { return m_imuYaw; }
    
    // Status getters
    bool ntripConnected() const { return m_ntripStatus == 4; }
    bool imuConnected() const { return m_imuConnected; }
    QString ntripStatusText() const;
    QString ntripIPAddress() const;
    
    // Enhanced UI Integration getters (Phase 4.5.4)
    QString gpsStatusText() const;
    QString moduleStatusText() const;
    QString serialStatusText() const;
    bool showErrorDialog() const { return m_showErrorDialog; }
    QString lastErrorMessage() const { return m_lastErrorMessage; }
    QVariantList discoveredModules() const;
    
    // QML invokable methods
    Q_INVOKABLE void configureNTRIP();
    Q_INVOKABLE void startBluetoothDiscovery();
    Q_INVOKABLE void connectBluetooth(const QString& deviceName);
    Q_INVOKABLE void disconnectBluetooth();
    Q_INVOKABLE void startCommunication();
    Q_INVOKABLE void stopCommunication();
    Q_INVOKABLE void saveSettings();
    Q_INVOKABLE void loadSettings();
    
    // Test methods
    Q_INVOKABLE void testThreadCommunication();
    Q_INVOKABLE void wakeUpModules(const QString& broadcastIP = "192.168.1.255");
    
    // Additional QML methods for compatibility
    Q_INVOKABLE void btnUDPListenOnly_clicked(int checkState);
    Q_INVOKABLE void ntripDebug(int checkState);
    Q_INVOKABLE void bluetoothDebug(int checkState);
    Q_INVOKABLE void bt_search(const QString& deviceName);
    Q_INVOKABLE void bt_remove_device(const QString& deviceName);
    Q_INVOKABLE void setIPFromUrl(const QString& url);
    Q_INVOKABLE void btnSendSubnet_clicked();
    
    // Phase 4.6: PGN transmission (replaces FormGPS SendPgnToLoop)
    Q_INVOKABLE void sendPgn(const QByteArray& pgnData);
    
    // Phase 4.5: Direct worker communication (FormLoop eliminated)

public slots:
    // Application lifecycle
    void initialize();
    void shutdown();
    
    // Enhanced UI Integration (Phase 4.5.4)
    Q_INVOKABLE void updateGPSStatus();
    Q_INVOKABLE void updateModuleStatus();
    Q_INVOKABLE void updateNTRIPStatus();
    Q_INVOKABLE void updateSerialStatus();
    Q_INVOKABLE void clearErrorDialog();
    Q_INVOKABLE void startModuleDiscovery();
    Q_INVOKABLE void stopModuleDiscovery();
    
    // Network configuration
    Q_INVOKABLE void configureSubnet();

private slots:
    // Worker thread data reception (Qt::DirectConnection for real-time)
    void onGpsDataReceived(double lat, double lon, double heading, double speed);
    void onImuDataReceived(double roll, double pitch, double yaw);
    void onGpsStatusChanged(bool connected, int quality, int satellites, double age);
    void onNmeaSentenceReceived(const QString& sentenceType, const QString& rawSentence);
    
    void onBluetoothStatusChanged(bool connected, const QString& device);
    void onNtripStatusChanged(int status, const QString& statusText);
    void onUdpStatusChanged(bool connected);
    
    void onNtripDataReceived(const QByteArray& rtcmData);
    void onUdpDataReceived(const QByteArray& data);
    void onNmeaFromUdp(const QString& nmea);  // GPS via UDP
    
    // NMEA GPS parsing - CRITICAL for UDP 15555/17777 replacement
    void parseNmeaGpsData(const QString& nmeaString);
    
    // Settings updates from workers
    void onSettingsUpdateRequired(const QString& key, const QVariant& value);
    
    // Serial Worker slots
    void onSerialIMUDataReceived(const QByteArray& imuData);
    void onSerialAutosteerResponse(const QByteArray& response);
    void onSerialGPSConnected(bool connected);
    void onSerialIMUConnected(bool connected);
    void onSerialAutosteerConnected(bool connected);
    void onSerialError(const QString& portName, const QString& error);
    
    // Enhanced UDP Worker slots (Phase 4.5.4)
    void onModuleDiscovered(const QString& moduleIP, const QString& moduleType);
    void onModuleTimeout(const QString& moduleIP);
    void onNetworkScanCompleted(const QStringList& discoveredModules);
    void onPgnDataReceived(const QByteArray& pgnData);
    
    // Multi-subnet discovery slots
    void onModuleSubnetDiscovered(const QString& moduleIP, const QString& currentSubnet);
    void onSubnetScanCompleted(const QString& activeSubnet);

signals:
    // Property change notifications for QML
    void gpsDataChanged();
    void vehiclePositionChanged();
    void imuDataChanged();
    void gpsStatusChanged();
    void bluetoothStatusChanged();
    void ethernetStatusChanged();
    void ntripStatusChanged();
    void settingsChanged();
    
    // Additional status change signals for QML compatibility
    void imuStatusChanged();
    void steerStatusChanged();
    void machineStatusChanged();
    void blockageStatusChanged();
    
    // Enhanced UI Integration signals (Phase 4.5.4)
    void statusChanged();
    void errorOccurred(const QString& error);
    void statusMessageChanged(const QString& message);
    void moduleDiscoveryChanged();
    
    // Commands to workers (Qt::QueuedConnection)
    void requestStartGPS(const QString& serialPort, int baudRate);
    void requestStopGPS();
    void requestStartNTRIP(const QString& url, const QString& user, 
                          const QString& password, const QString& mount, int port);
    void requestStopNTRIP();
    void requestStartUDP(const QString& address, int port);
    void requestStopUDP();
    void requestBluetoothScan();
    
    // Serial Worker commands
    void requestSerialWorkerStart();
    void requestSerialWorkerStop();

private:
    // Private constructor for strict singleton pattern
    explicit AgIOService(QObject* parent = nullptr);
    
    // Real-time data members (main thread - direct OpenGL access)
    double m_latitude, m_longitude, m_heading, m_speed;
    double m_altitude, m_hdop;  // GPS altitude and accuracy
    QPointF m_vehicleXY;  // Computed position for OpenGL
    double m_easting, m_northing;  // Local coordinates (meters)
    double m_imuRoll, m_imuPitch, m_imuYaw;
    double m_rollGPS;  // GPS/Dual antenna roll (0 for single antenna)
    
    // Status data
    bool m_gpsConnected, m_bluetoothConnected, m_ethernetConnected;
    int m_gpsQuality, m_satellites;
    double m_age;
    QString m_bluetoothDevice;
    
    // Additional connection status
    bool m_imuConnected, m_steerConnected, m_machineConnected, m_blockageConnected;
    
    // NTRIP status
    int m_ntripStatus;
    bool m_ntripEnabled;
    int m_rawTripCount;
    
    // Settings data  
    QString m_ntripUrl, m_ntripUser, m_ntripPassword, m_ntripMount;
    int m_ntripPort;
    QString m_gpsSerialPort, m_imuSerialPort, m_steerSerialPort;
    int m_gpsSerialBaud, m_imuSerialBaud, m_steerSerialBaud;
    bool m_bluetoothEnabled;
    
    // Additional settings data members  
    QString m_gnssSerialPort;
    int m_gnssBaudRate;
    QVariantList m_bluetoothDevices;
    int m_udpIP1, m_udpIP2, m_udpIP3;
    bool m_isMetric, m_isDayMode;
    bool m_isSectionsNotZones;
    QVariantList m_toolZones, m_hotkeys;
    
    // UDP settings
    QString m_udpBroadcastAddress;  // e.g., "192.168.1.255"
    int m_udpListenPort;  // 9999 for AgIO
    int m_udpSendPort;    // 8888 to modules
    
    // Additional GPS/IMU data members
    double m_dualHeading;  // Dual antenna heading
    double m_gpsHz;        // GPS frequency
    double m_nowHz;        // Current Hz measurement  
    double m_yawRate;      // IMU yaw rate
    
    // NMEA sentence storage for debugging
    QString m_ggaSentence;
    QString m_vtgSentence;
    QString m_pandaSentence;
    QString m_paogiSentence;
    QString m_hdtSentence;
    QString m_avrSentence;
    QString m_hpdSentence;
    QString m_sxtSentence;
    QString m_unknownSentence;
    
    // Enhanced UI state (Phase 4.5.4)
    QString m_gpsStatusText;
    QString m_moduleStatusText;
    QString m_ntripStatusTextInternal;
    QString m_serialStatusText;
    QString m_lastErrorMessage;
    bool m_showErrorDialog;
    QVariantList m_discoveredModules;
    
    // Worker threads
    QThread* m_gpsThread;
    QThread* m_ntripThread;
    QThread* m_udpThread;
    QThread* m_serialThread;
    
    // Workers
    GPSWorker* m_gpsWorker;
    NTRIPWorker* m_ntripWorker;
    UDPWorker* m_udpWorker;
    SerialWorker* m_serialWorker;
    
    // Core components
    QTimer* m_heartbeatTimer;
    
    // Initialization flag
    bool m_initialized;
    
    // Setup methods
    void setupWorkerThreads();
    void connectWorkerSignals();
    void loadDefaultSettings();
    
    // Utility methods
    void updateVehiclePosition();  // Convert lat/lon to vehicle_xy
    void logDebugInfo() const;
};

#endif // AGIOSERVICE_H