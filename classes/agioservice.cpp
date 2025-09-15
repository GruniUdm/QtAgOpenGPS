#include "agioservice.h"
#include "classes/settingsmanager.h"
#include <QDebug>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QElapsedTimer>
#include <QHostInfo>
#include <QDateTime>

// SomcoSoftware approach: Qt manages the singleton automatically

AgIOService::AgIOService(QObject *parent)
    : QObject(parent)
    , m_latitude(0.0)
    , m_longitude(0.0)
    , m_heading(0.0)
    , m_speed(0.0)
    , m_altitude(0.0)
    , m_hdop(99.9)  // Initialize to high value indicating no fix
    , m_vehicleXY(0.0, 0.0)
    , m_easting(0.0)
    , m_northing(0.0)
    , m_imuRoll(0.0)
    , m_imuPitch(0.0)
    , m_rollGPS(0.0)  // GPS/Dual roll = 0 for single antenna
    , m_imuYaw(0.0)
    , m_gpsConnected(false)
    , m_bluetoothConnected(false)
    , m_ethernetConnected(false)
    , m_gpsQuality(0)
    , m_satellites(0)
    , m_age(0.0)
    , m_ntripStatus(0)
    , m_ntripEnabled(false)
    , m_rawTripCount(0)
    // Additional connection status initialization
    , m_imuConnected(false)
    , m_steerConnected(false)
    , m_machineConnected(false)
    , m_blockageConnected(false)
    , m_ntripPort(2101)
    , m_gpsSerialBaud(115200)
    , m_imuSerialBaud(115200)
    , m_steerSerialBaud(38400)
    , m_bluetoothEnabled(false)
    , m_gnssBaudRate(115200)
    , m_udpIP1(192)
    , m_udpIP2(168)
    , m_udpIP3(1)
    , m_isMetric(false)
    , m_isDayMode(true)
    , m_isSectionsNotZones(true)
    , m_gpsThread(nullptr)
    , m_ntripThread(nullptr)
    , m_udpThread(nullptr)
    , m_serialThread(nullptr)
    , m_gpsWorker(nullptr)
    , m_ntripWorker(nullptr)
    , m_udpWorker(nullptr)
    , m_serialWorker(nullptr)
    , m_heartbeatTimer(nullptr)
    , m_initialized(false)
    // Additional GPS/IMU initialization
    , m_dualHeading(0.0)
    , m_gpsHz(0.0)
    , m_nowHz(0.0)
    , m_yawRate(0.0)
    // NMEA sentences initialization
    , m_ggaSentence("")
    , m_vtgSentence("")
    , m_pandaSentence("")
    , m_paogiSentence("")
    , m_hdtSentence("")
    , m_avrSentence("")
    , m_hpdSentence("")
    , m_sxtSentence("")
    , m_unknownSentence("")
    // Enhanced UI state (Phase 4.5.4)
    , m_gpsStatusText("GPS Disconnected")
    , m_moduleStatusText("No Modules")
    , m_ntripStatusTextInternal("NTRIP Off")
    , m_serialStatusText("Serial Disconnected")
    , m_lastErrorMessage("")
    , m_showErrorDialog(false)
{
    qDebug() << "🔧 AgIOService constructor - Main Thread:" << QThread::currentThread();
    
    qDebug() << "🏗️ AgIOService singleton constructor called, parent:" << parent;
    
    // Initialize AgIO-specific settings - now via SettingsManager (thread-safe)
    // Uses unified QtAgOpenGPS.ini via SettingsManager
    qDebug() << "📁 AgIOService now uses SettingsManager for thread-safe settings";
    
    // Initialize heartbeat timer
    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setInterval(1000); // 1Hz heartbeat
    connect(m_heartbeatTimer, &QTimer::timeout, this, &AgIOService::logDebugInfo);
    
    // Load default settings
    loadDefaultSettings();
    
    // Initialize immediately comme CTrack/CVehicle singleton pattern
    initialize();
    loadSettings();  
    startCommunication();
    
    qDebug() << "✅ AgIOService created and initialized in main thread";
}

AgIOService::~AgIOService()
{
    qDebug() << "AgIOService destructor - shutdown already handled by FormGPS";
}

// Meyer's singleton pattern - no explicit create() method needed in .cpp

QString AgIOService::ntripStatusText() const
{
    switch (m_ntripStatus) {
        case 0: return "Disconnected";
        case 1: return "Connecting...";
        case 2: return "Authorizing...";
        case 3: return "Connected - Waiting for Data";
        case 4: return "Connected - Receiving Data";
        case 5: return "Connection Error";
        default: return "Unknown Status";
    }
}

QString AgIOService::ntripIPAddress() const
{
    return QString("%1.%2.%3.%4")
        .arg(192).arg(168).arg(m_udpIP3).arg(1);
}

QString AgIOService::fixQuality() const
{
    switch(m_gpsQuality) {
    case 0:
        return "Invalid: ";
    case 1:
        return "GPS 1: ";
    case 2:
        return "DGPS : ";
    case 3:
        return "PPS : ";
    case 4:
        return "RTK fix: ";
    case 5:
        return "Float: ";
    case 6:
        return "Estimate: ";
    case 7:
        return "Man IP: ";
    case 8:
        return "Sim: ";
    default:
        return "Unknown: ";
    }
}

// QML invokable methods
void AgIOService::configureNTRIP()
{
    // Reload settings to get latest from QML
    loadSettings();
    
    qDebug() << "🔧 Configuring NTRIP:" << m_ntripUrl << m_ntripMount << ":" << m_ntripPort;
    
    if (m_ntripEnabled && !m_ntripUrl.isEmpty() && !m_ntripMount.isEmpty()) {
        emit requestStartNTRIP(m_ntripUrl, m_ntripUser, m_ntripPassword, m_ntripMount, m_ntripPort);
        qDebug() << "✅ NTRIP started with" << m_ntripUrl << "/" << m_ntripMount;
    } else {
        emit requestStopNTRIP();
        qDebug() << "🛑 NTRIP stopped";
    }
}

void AgIOService::startBluetoothDiscovery()
{
    qDebug() << "🔍 Starting Bluetooth discovery...";
    emit requestBluetoothScan();
}

void AgIOService::connectBluetooth(const QString& deviceName)
{
    qDebug() << "📱 Connecting to Bluetooth device:" << deviceName;
    m_bluetoothDevice = deviceName;
    // TODO: Implement actual Bluetooth connection via worker
}

void AgIOService::disconnectBluetooth()
{
    qDebug() << "📱 Disconnecting Bluetooth";
    m_bluetoothDevice.clear();
    m_bluetoothConnected = false;
    emit bluetoothStatusChanged();
}

void AgIOService::startCommunication()
{
    if (m_initialized) {
        qDebug() << "⚠️ AgIO communication already started";
        return;
    }
    
    qDebug() << "🚀 Starting AgIO communication - Main Thread:" << QThread::currentThread();
    
    // Setup worker threads
    setupWorkerThreads();
    
    // Start heartbeat
    m_heartbeatTimer->start();
    
    m_initialized = true;
    qDebug() << "✅ AgIO service started successfully";
}

void AgIOService::stopCommunication()
{
    if (!m_initialized) {
        return;
    }
    
    qDebug() << "🛑 Stopping AgIO communication";
    
    // Stop heartbeat
    m_heartbeatTimer->stop();
    
    // Stop all workers
    emit requestStopGPS();
    emit requestStopNTRIP();
    emit requestStopUDP();
    
    m_initialized = false;
}

void AgIOService::saveSettings()
{
    qDebug() << "💾 Saving AgIO settings via SettingsManager (thread-safe)...";

    // ✅ MIGRATION: Using SettingsManager with cohérent keys (comm/ prefix)
    // NTRIP settings
    SettingsManager::instance()->setValue("comm/ntripURL", m_ntripUrl);
    SettingsManager::instance()->setValue("comm/ntripUserName", m_ntripUser);
    SettingsManager::instance()->setValue("comm/ntripUserPassword", m_ntripPassword);
    SettingsManager::instance()->setValue("comm/ntripMount", m_ntripMount);
    SettingsManager::instance()->setValue("comm/ntripCasterPort", m_ntripPort);
    SettingsManager::instance()->setValue("comm/ntripIsOn", m_ntripEnabled);

    // UDP settings (cohérent avec existing keys)
    SettingsManager::instance()->setValue("comm/udpIP1", m_udpIP1);
    SettingsManager::instance()->setValue("comm/udpIP2", m_udpIP2);
    SettingsManager::instance()->setValue("comm/udpIP3", m_udpIP3);

    // GPS/Serial settings
    SettingsManager::instance()->setValue("comm/gpsSerialPort", m_gpsSerialPort);
    SettingsManager::instance()->setValue("comm/gpsSerialBaud", m_gpsSerialBaud);

    // IMU settings
    SettingsManager::instance()->setValue("comm/imuSerialPort", m_imuSerialPort);
    SettingsManager::instance()->setValue("comm/imuSerialBaud", m_imuSerialBaud);

    // Bluetooth settings
    SettingsManager::instance()->setValue("comm/bluetoothEnabled", m_bluetoothEnabled);
    SettingsManager::instance()->setValue("comm/bluetoothDevice", m_bluetoothDevice);

    // Thread-safe sync via SettingsManager
    SettingsManager::instance()->sync();
    qDebug() << "✅ AgIO settings saved via SettingsManager (unified QtAgOpenGPS.ini)";

    emit settingsChanged();
}

void AgIOService::loadSettings()
{
    qDebug() << "📂 Loading AgIO settings via SettingsManager (thread-safe)...";

    // ✅ MIGRATION: Using SettingsManager with cohérent keys
    // NTRIP settings - keys matching saveSettings()
    m_ntripUrl = SettingsManager::instance()->value("comm/ntripURL", "caster.centipede.fr").toString();
    m_ntripUser = SettingsManager::instance()->value("comm/ntripUserName", "").toString();
    m_ntripPassword = SettingsManager::instance()->value("comm/ntripUserPassword", "").toString();
    m_ntripMount = SettingsManager::instance()->value("comm/ntripMount", "LAB1").toString();
    m_ntripPort = SettingsManager::instance()->value("comm/ntripCasterPort", 2101).toInt();
    m_ntripEnabled = SettingsManager::instance()->value("comm/ntripIsOn", false).toBool();

    // UDP settings - thread-safe via SettingsManager
    m_udpIP1 = SettingsManager::instance()->value("comm/udpIP1", 192).toInt();
    m_udpIP2 = SettingsManager::instance()->value("comm/udpIP2", 168).toInt();
    m_udpIP3 = SettingsManager::instance()->value("comm/udpIP3", 1).toInt();
    m_udpBroadcastAddress = QString("%1.%2.%3.255").arg(m_udpIP1).arg(m_udpIP2).arg(m_udpIP3);
    m_udpListenPort = 9999;   // Standard AgOpenGPS UDP port
    m_udpSendPort = SettingsManager::instance()->value("comm/udpSendPort", 8888).toInt();

    // Warn if subnet doesn't match current network
    if (m_udpIP3 == 5) {
        qDebug() << "⚠️ WARNING: Settings show subnet 192.168.5.xxx but your network may be 192.168.1.xxx";
        qDebug() << "  Consider updating comm/udpIP3 in settings to match your network";
    }

    // GPS/Serial settings
    m_gpsSerialPort = SettingsManager::instance()->value("comm/gpsSerialPort", "COM3").toString();
    m_gpsSerialBaud = SettingsManager::instance()->value("comm/gpsSerialBaud", 115200).toInt();

    // IMU settings
    m_imuSerialPort = SettingsManager::instance()->value("comm/imuSerialPort", "COM4").toString();
    m_imuSerialBaud = SettingsManager::instance()->value("comm/imuSerialBaud", 115200).toInt();

    // Bluetooth settings
    m_bluetoothEnabled = SettingsManager::instance()->value("comm/bluetoothEnabled", false).toBool();
    m_bluetoothDevice = SettingsManager::instance()->value("comm/bluetoothDevice", "").toString();

    qDebug() << "✅ AgIO settings loaded via SettingsManager:";
    qDebug() << "  NTRIP:" << m_ntripUrl << "/" << m_ntripMount << ":" << m_ntripPort;
    qDebug() << "  UDP:" << m_udpBroadcastAddress << ":" << m_udpListenPort;
    qDebug() << "  Settings unified in QtAgOpenGPS.ini (thread-safe)";
    emit settingsChanged();
}

void AgIOService::testThreadCommunication()
{
    qDebug() << "🧪 === THREAD COMMUNICATION TEST ===";
    qDebug() << "Main/AgIOService Thread:" << QThread::currentThread();
    qDebug() << "GPS Thread:" << m_gpsThread << "Worker:" << (m_gpsWorker ? "✅" : "❌");
    qDebug() << "NTRIP Thread:" << m_ntripThread << "Worker:" << (m_ntripWorker ? "✅" : "❌");
    qDebug() << "UDP Thread:" << m_udpThread << "Worker:" << (m_udpWorker ? "✅" : "❌");
    qDebug() << "Service Initialized:" << m_initialized;
    qDebug() << "GPS Connected:" << m_gpsConnected;
    qDebug() << "Latitude:" << m_latitude;
    qDebug() << "Longitude:" << m_longitude;
    qDebug() << "NTRIP Status:" << m_ntripStatus;
    qDebug() << "Ethernet Connected:" << m_ethernetConnected;
    qDebug() << "=== END TEST ===";
}

void AgIOService::configureSubnet()
{
    qDebug() << "🌐 Sending subnet configuration to modules:" << m_udpIP1 << "." << m_udpIP2 << "." << m_udpIP3 << ".x";
    
    // Build PGN 201 message to configure module IP (from FormLoop original)
    QByteArray sendIPToModules;
    sendIPToModules.append(char(0x80)); // Header 1
    sendIPToModules.append(char(0x81)); // Header 2
    sendIPToModules.append(char(0x7F)); // Source AgIO
    sendIPToModules.append(char(201)); // PGN 201 (Subnet Configuration) - CORRECTED to decimal!
    sendIPToModules.append(char(5)); // Length = 5 bytes payload  
    sendIPToModules.append(char(201)); // Payload 1 (PGN repeat)
    sendIPToModules.append(char(201)); // Payload 2 (PGN repeat)
    sendIPToModules.append(char(m_udpIP1)); // IP1 (192)
    sendIPToModules.append(char(m_udpIP2)); // IP2 (168)
    sendIPToModules.append(char(m_udpIP3)); // IP3 (1)
    sendIPToModules.append(char(0x47)); // Checksum (from original)
    
    // NEW: Use multi-subnet discovery to find and send PGN 201 to current module subnet
    if (m_udpWorker) {
        qDebug() << "🔍 Using multi-subnet discovery to send PGN 201 to current module subnet";
        
        QMetaObject::invokeMethod(m_udpWorker, "sendPgnToDiscoveredSubnet",
                                  Qt::QueuedConnection,
                                  Q_ARG(QByteArray, sendIPToModules));
    } else {
        qDebug() << "⚠️ UDPWorker not available for subnet configuration";
    }
    
    // Save settings
    saveSettings();
    
    qDebug() << "✅ PGN 201 subnet command sent to modules";
}

void AgIOService::wakeUpModules(const QString& broadcastIP)
{
    qDebug() << "🔊 AgIOService: Waking up dormant modules via UDPWorker";
    
    if (m_udpWorker) {
        // Call UDPWorker's wakeUpModules method
        QMetaObject::invokeMethod(m_udpWorker, "wakeUpModules", 
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, broadcastIP));
    } else {
        qDebug() << "⚠️ UDPWorker not available for module wakeup";
    }
}

// Legacy method removed - Phase 4.5: FormLoop eliminated
// AgIOService now gets GPS data directly from UDPWorker/GPSWorker

// Public slots
void AgIOService::initialize()
{
    qDebug() << "🚀 Initializing AgIOService...";
    
    loadSettings();
    startCommunication();
    
    qDebug() << "✅ AgIOService initialized";
}

void AgIOService::shutdown()
{
    qDebug() << "🛑 Shutting down AgIOService...";

    stopCommunication();
    saveSettings();
    
    // Clean up workers and threads
    if (m_gpsWorker) {
        m_gpsWorker->deleteLater();
        m_gpsWorker = nullptr;
    }
    if (m_gpsThread) {
        m_gpsThread->quit();
        m_gpsThread->wait(3000);
        m_gpsThread->deleteLater();
        m_gpsThread = nullptr;
    }
    
    if (m_ntripWorker) {
        m_ntripWorker->deleteLater();
        m_ntripWorker = nullptr;
    }
    if (m_ntripThread) {
        m_ntripThread->quit();
        m_ntripThread->wait(3000);
        m_ntripThread->deleteLater();
        m_ntripThread = nullptr;
    }
    
    if (m_udpWorker) {
        m_udpWorker->deleteLater();
        m_udpWorker = nullptr;
    }
    if (m_udpThread) {
        m_udpThread->quit();
        m_udpThread->wait(3000);
        m_udpThread->deleteLater();
        m_udpThread = nullptr;
    }
    
    if (m_serialWorker) {
        m_serialWorker->deleteLater();
        m_serialWorker = nullptr;
    }
    if (m_serialThread) {
        m_serialThread->quit();
        m_serialThread->wait(3000);
        m_serialThread->deleteLater();
        m_serialThread = nullptr;
    }
    
    qDebug() << "✅ AgIOService shutdown complete";
}

// Private slots - Worker data reception (Qt::DirectConnection for real-time)
void AgIOService::onGpsDataReceived(double lat, double lon, double heading, double speed)
{
    qDebug() << "✅ AgIOService::onGpsDataReceived - Lat:" << lat << "Lon:" << lon 
             << "Heading:" << heading << "Speed:" << speed;
    
    // PHASE 4.3: Latency profiling for AutoSteer safety
    static QElapsedTimer latencyTimer;
    static bool timerStarted = false;
    if (!timerStarted) {
        latencyTimer.start();
        timerStarted = true;
    }
    
    // Measure reception latency
    qint64 receptionLatency = latencyTimer.nsecsElapsed();
    latencyTimer.restart();
    
    // Direct assignment in main thread (ZERO latency for OpenGL)
    m_latitude = lat;
    m_longitude = lon;
    m_heading = heading;
    m_speed = speed;
    
    // Measure assignment latency
    qint64 assignmentLatency = latencyTimer.nsecsElapsed();
    latencyTimer.restart();
    
    // Update computed vehicle position
    updateVehiclePosition();
    
    // Measure transformation latency
    qint64 transformLatency = latencyTimer.nsecsElapsed();
    latencyTimer.restart();
    
    // Emit notifications for QML
    emit gpsDataChanged();
    emit vehiclePositionChanged();
    
    // Measure signal emission latency
    qint64 signalLatency = latencyTimer.nsecsElapsed();
    
    // Warning if total latency > 1000ns (1μs) for AutoSteer safety
    qint64 totalLatency = assignmentLatency + transformLatency + signalLatency;
    if (totalLatency > 1000) {
        qWarning() << "⚠️ AUTOSTEER LATENCY WARNING:" << totalLatency << "ns"
                   << "(assign:" << assignmentLatency 
                   << "transform:" << transformLatency
                   << "signal:" << signalLatency << ")";
    }
    
    // Debug output (limit frequency)
    static int debugCounter = 0;
    if (++debugCounter % 30 == 0) { // Every 30 updates (1Hz at 30Hz)
        qDebug() << "📍 GPS updated - Latency:" << totalLatency << "ns"
                 << "Lat:" << lat << "Lon:" << lon 
                 << "Thread:" << QThread::currentThread();
    }
}

void AgIOService::onImuDataReceived(double roll, double pitch, double yaw)
{
    // Direct assignment in main thread (ZERO latency for AutoSteer)
    m_imuRoll = roll;
    m_imuPitch = pitch;
    m_imuYaw = yaw;
    
    // Emit notification for QML
    emit imuDataChanged();
}

void AgIOService::onGpsStatusChanged(bool connected, int quality, int satellites, double age)
{
    m_gpsConnected = connected;
    m_gpsQuality = quality;
    m_satellites = satellites;
    m_age = age;
    
    emit gpsStatusChanged();
    
    qDebug() << "📡 GPS status:" << connected << "Q:" << quality << "Sats:" << satellites << "Age:" << age;
}

void AgIOService::onNmeaSentenceReceived(const QString& sentenceType, const QString& rawSentence)
{
    // Store raw NMEA sentences for display
    if (sentenceType == "VTG") {
        m_vtgSentence = rawSentence;
        emit statusChanged(); // Notify QML that VTG sentence changed
        qDebug() << "📡 VTG sentence updated:" << rawSentence;
    }
    // Add other sentence types as needed
}

void AgIOService::onBluetoothStatusChanged(bool connected, const QString& device)
{
    m_bluetoothConnected = connected;
    m_bluetoothDevice = device;
    
    emit bluetoothStatusChanged();
    
    qDebug() << "📱 Bluetooth:" << connected << device;
}

void AgIOService::onNtripStatusChanged(int status, const QString& statusText)
{
    m_ntripStatus = status;
    
    emit ntripStatusChanged();
    
    qDebug() << "📡 NTRIP status:" << status << statusText;
}

void AgIOService::onUdpStatusChanged(bool connected)
{
    m_ethernetConnected = connected;
    
    emit ethernetStatusChanged();
    
    qDebug() << "🌐 UDP status:" << connected;
}

void AgIOService::onNtripDataReceived(const QByteArray& rtcmData)
{
    // Forward RTCM corrections to GPS worker or UDP worker
    // This maintains real-time data flow
    Q_UNUSED(rtcmData)
    
    static int dataCounter = 0;
    if (++dataCounter % 100 == 0) { // Debug every 100 packets
        qDebug() << "📡 NTRIP data received:" << rtcmData.size() << "bytes";
    }
}

void AgIOService::onUdpDataReceived(const QByteArray& data)
{
    // Process GPS and PGN data from hardware modules (port 9999)
    // CRITICAL: This replaces the old UDP 15555/17777 loopback system!
    
    if (data.isEmpty()) return;
    
    
    // Check for NMEA GPS data (starts with '$' = 0x24) - THE GPS DATA SOURCE!
    if (data[0] == 0x24) {
        // NMEA GPS data from module - parse and update GPS properties
        QString nmeaString = QString::fromLatin1(data);
        parseNmeaGpsData(nmeaString);
        return;
    }
    
    // Check for PGN binary data (0x80, 0x81 header)
    if (data.size() >= 4 && static_cast<uint8_t>(data[0]) == 0x80 && static_cast<uint8_t>(data[1]) == 0x81) {
        uint8_t pgn = static_cast<uint8_t>(data[3]);
        
        // Debug PGN reception (reduced frequency)
        static int pgnCounter = 0;
        if (++pgnCounter % 50 == 0) {
            qDebug() << "📡 PGN" << pgn << "received:" << data.size() << "bytes";
        }
        return;
    }
    
    // Debug output for other data types
    static int udpCounter = 0;
    if (++udpCounter % 100 == 0) {
        qDebug() << "🌐 UDP other data received:" << data.size() << "bytes";
    }
}

void AgIOService::onNmeaFromUdp(const QString& nmea)
{
    // NOTE: This method is now OBSOLETE - replaced by parseNmeaGpsData()
    // Keep for legacy compatibility with other NMEA sources
    if (nmea.isEmpty()) return;
    
    static int nmeaLogCounter = 0;
    if (++nmeaLogCounter % 30 == 0) {
        qDebug() << "📡 NMEA via UDP (legacy):" << nmea.left(30);
    }
}

void AgIOService::parseNmeaGpsData(const QString& nmeaString)
{
    // Parse NMEA GPS data from hardware modules ($PANDA format)
    // This is the CRITICAL method that replaces UDP 15555/17777 loopback!
    
    if (nmeaString.isEmpty()) return;
    
    // Check for AgOpenGPS NMEA format ($PANDA or $PAOGI)
    if (nmeaString.startsWith("$PANDA") || nmeaString.startsWith("$PAOGI")) {
        QStringList fields = nmeaString.split(',');
        
        // Debug NMEA format (temporary for parsing verification)
        static int debugCounter = 0;
        if (++debugCounter % 100 == 0) {
            qDebug() << "🔍 NMEA Debug:" << nmeaString.left(120);
            qDebug() << "🔍 Fields count:" << fields.length();
            for (int i = 0; i < qMin(fields.length(), 15); i++) {
                qDebug() << "   Field" << i << ":" << fields[i];
            }
        }
        
        
        // $PANDA format: time,lat,lat_ns,lon,lon_ew,quality,satellites,hdop,altitude,age_dgps,heading,roll_imu,pitch_imu*checksum
        if (fields.length() >= 12) {
            
            // Parse latitude (fields 2,3) - DDMM.MMMMMM format
            if (fields.length() >= 4 && !fields[2].isEmpty() && !fields[3].isEmpty()) {
                double rawLat = fields[2].toDouble();
                double degrees = int(rawLat / 100);
                double minutes = rawLat - (degrees * 100);
                double latitude = degrees + (minutes / 60.0);
                if (fields[3] == "S") latitude = -latitude;
                
                // Parse longitude (fields 4,5) - DDDMM.MMMMMM format  
                if (fields.length() >= 6 && !fields[4].isEmpty() && !fields[5].isEmpty()) {
                    double rawLon = fields[4].toDouble();
                    degrees = int(rawLon / 100);
                    minutes = rawLon - (degrees * 100);
                    double longitude = degrees + (minutes / 60.0);
                    if (fields[5] == "W") longitude = -longitude;
                    
                    // Parse other GPS data - $PANDA format EXACT from original FormLoop:
                    // Fields: 0=$PANDA,1=time,2=lat,3=lat_ns,4=lon,5=lon_ew,6=quality,7=satellites,8=hdop,9=altitude,10=age_dgps,11=speed,12=heading,13=roll,14=pitch,15=yaw
                    int quality = fields.length() >= 7 ? fields[6].toInt() : 0;        // Field 6 (quality)
                    int satellites = fields.length() >= 8 ? fields[7].toInt() : 0;     // Field 7 (satellites)  
                    double hdop = fields.length() >= 9 ? fields[8].toDouble() : 99.9;  // Field 8 (hdop)
                    double altitude = fields.length() >= 10 ? fields[9].toDouble() : 0.0; // Field 9 (altitude)
                    double age = fields.length() >= 11 ? fields[10].toDouble() : 0.0;  // Field 10 (age_dgps)
                    double speed = fields.length() >= 12 ? fields[11].toDouble() : 0.0; // Field 11 (speed knots)
                    // Field 12: GPS Heading (fix2fix heading in degrees)
                    double gpsHeading = fields.length() >= 13 ? fields[12].toDouble() : 0.0;
                    
                    // Validate GPS heading (fix2fix can be invalid when not moving)
                    if (gpsHeading >= 0 && gpsHeading <= 360) {
                        m_heading = gpsHeading; // Valid GPS fix2fix heading
                    } else {
                        // Keep previous heading if current is invalid
                        qDebug() << "⚠️ Invalid GPS heading:" << gpsHeading << "- keeping previous:" << m_heading;
                    }
                    
                    // Update AgIOService GPS properties
                    m_latitude = latitude;
                    m_longitude = longitude;
                    // m_heading is already set from GPS heading above
                    m_speed = speed * 1.852; // Convert knots to km/h for display
                    m_altitude = altitude;
                    m_hdop = hdop;
                    m_satellites = satellites;
                    m_gpsQuality = quality;
                    m_age = age;
                    
                    // Store NMEA sentence for debugging (show in QML)
                    m_pandaSentence = nmeaString; // Complete sentence, no truncation
                    
                    // Update GPS frequency calculation
                    static qint64 lastUpdateTime = 0;
                    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
                    if (lastUpdateTime > 0) {
                        double deltaTime = (currentTime - lastUpdateTime) / 1000.0; // seconds
                        if (deltaTime > 0) {
                            m_nowHz = 1.0 / deltaTime;  // Current Hz
                            m_gpsHz = 0.9 * m_gpsHz + 0.1 * m_nowHz; // Filtered Hz
                        }
                    }
                    lastUpdateTime = currentTime;
                    
                    // Throttle UI updates for debug info (2 Hz for reasonable responsiveness)
                    static int uiUpdateCounter = 0;
                    if (++uiUpdateCounter % 5 == 0) {
                        emit statusChanged();  // Update Hz and PANDA display in QML (2 Hz)
                    }
                    
                    // Set GPS connected status based on quality
                    bool wasConnected = m_gpsConnected;
                    m_gpsConnected = (quality > 0 && satellites >= 4);
                    
                    // BALANCED: Throttle GPS signals to prevent OpenGL flickering but keep responsive
                    // Reduce signal emission to 5 Hz (every 4 updates at 20Hz GPS) for stable but responsive OpenGL
                    static int signalThrottleCounter = 0;
                    if (++signalThrottleCounter % 4 == 0) {
                        emit gpsDataChanged();
                        emit vehiclePositionChanged();
                        emit gpsStatusChanged();  // For sats, quality, age properties (also throttled)
                    }
                    
                    if (!wasConnected && m_gpsConnected) {
                        qDebug() << "✅ GPS CONNECTED via NMEA - Quality:" << quality 
                                << "Satellites:" << satellites << "Position:" << latitude << longitude;
                    }
                    
                    // Parse IMU data if available - $PANDA fields 12,13,14,15 (imu_heading, roll, pitch, yaw_rate)
                    // Note: FormLoop uses SHORT conversion, not DOUBLE - values might be integers
                    if (fields.length() >= 14) {
                        // CRITICAL: AIO firmware sends INTEGER values, not floats!
                        // Field 12: GPS heading (NOT IMU) - fix2fix heading in degrees
                        int gpsHeadingRaw = fields.length() >= 13 ? fields[12].toInt() : 0;
                        double imuYawForDisplay = gpsHeadingRaw; // Store as yaw for GPS Info display
                        
                        // Field 13: Roll angle (AIO sends integer x10: -190 = -19.0°)
                        int rollRaw = fields.length() >= 14 ? fields[13].toInt() : 0;
                        double roll = rollRaw; // Integer x10 value, conversion to degrees done later in display
                        
                        // Field 14: Pitch angle (AIO sends integer x10: -60 = -6.0°)
                        int pitchRaw = fields.length() >= 15 ? fields[14].toInt() : 0;
                        double pitch = pitchRaw; // Integer x10 value, conversion to degrees done later in display
                        
                        // Field 15: Yaw rate (AIO sends x10 integer: yawRate*10)
                        int yawRateRaw = fields.length() >= 16 ? fields[15].toInt() : 0;
                        double yawRate = yawRateRaw / 10.0; // Convert x10 integer to degrees/sec
                        
                        // Update ALL IMU properties - raw values for GPS Info display
                        m_imuRoll = roll;     // Raw x10 value (-9)
                        m_imuPitch = pitch;   // Raw x10 value (-7)  
                        m_imuYaw = imuYawForDisplay;   // GPS heading (65) displayed as "yaw" in GPS Info
                        m_yawRate = yawRate;  // Converted to degrees/sec
                        
                        // IMPORTANT: GPS heading values are ALREADY set above in GPS section
                        // Don't override m_heading and m_dualHeading here
                        
                        // Emit IMU data changed signal
                        emit imuDataChanged();
                        
                        static int imuLogCounter = 0;
                        if (++imuLogCounter % 50 == 0) {
                            qDebug() << "🧭 IMU data parsed - Roll:" << roll << "° Pitch:" << pitch << "° YawRate:" << yawRate << "°/s";
                        }
                    }
                    
                    // Debug GPS parsing (reduced frequency)
                    static int gpsLogCounter = 0;
                    if (++gpsLogCounter % 50 == 0) {
                        qDebug() << "🛰️ GPS NMEA parsed - Lat:" << latitude << "Lon:" << longitude 
                                << "Alt:" << altitude << "m HDOP:" << hdop << "Q:" << quality << "Sats:" << satellites;
                        qDebug() << "🔍 Debug - fixQuality():" << fixQuality() << "pandaSentence:" << m_pandaSentence;
                    }
                }
            }
        }
    }
}

void AgIOService::onSettingsUpdateRequired(const QString& key, const QVariant& value)
{
    // Update settings from workers via SettingsManager (thread-safe)
    SettingsManager::instance()->setValue(key, value);

    emit settingsChanged();
}

// Private methods
void AgIOService::setupWorkerThreads()
{
    qDebug() << "🔧 Setting up worker threads...";
    
    // Create GPS worker thread
    m_gpsThread = new QThread(this);
    m_gpsThread->setObjectName("GPSWorkerThread");
    m_gpsWorker = new GPSWorker();
    m_gpsWorker->moveToThread(m_gpsThread);
    
    // Create NTRIP worker thread
    m_ntripThread = new QThread(this);
    m_ntripThread->setObjectName("NTRIPWorkerThread");
    m_ntripWorker = new NTRIPWorker();
    m_ntripWorker->moveToThread(m_ntripThread);
    
    // Create UDP worker thread
    m_udpThread = new QThread(this);
    m_udpThread->setObjectName("UDPWorkerThread");
    m_udpWorker = new UDPWorker();
    m_udpWorker->moveToThread(m_udpThread);
    
    // Create Serial worker thread
    m_serialThread = new QThread(this);
    m_serialThread->setObjectName("SerialWorkerThread");
    m_serialWorker = new SerialWorker();
    m_serialWorker->moveToThread(m_serialThread);
    
    // Start threads
    m_gpsThread->start();
    m_ntripThread->start();
    m_udpThread->start();
    m_serialThread->start();
    
    qDebug() << "✅ Worker threads created and started";
    
    // Setup thread-safe connections
    connectWorkerSignals();
}

void AgIOService::connectWorkerSignals()
{
    qDebug() << "🔗 Connecting worker signals...";
    
    // === GPS WORKER CONNECTIONS ===
    if (m_gpsWorker) {
        // Data signals (Qt::DirectConnection for real-time performance)
        connect(m_gpsWorker, &GPSWorker::gpsDataReceived,
                this, &AgIOService::onGpsDataReceived, Qt::DirectConnection);
        connect(m_gpsWorker, &GPSWorker::gpsStatusChanged,
                this, &AgIOService::onGpsStatusChanged, Qt::DirectConnection);
        connect(m_gpsWorker, &GPSWorker::nmeaSentenceReceived,
                this, &AgIOService::onNmeaSentenceReceived, Qt::DirectConnection);
        
        // Command signals (Qt::QueuedConnection for thread-safe commands)
        connect(this, &AgIOService::requestStartGPS,
                m_gpsWorker, &GPSWorker::startGPS, Qt::QueuedConnection);
        connect(this, &AgIOService::requestStopGPS,
                m_gpsWorker, &GPSWorker::stopGPS, Qt::QueuedConnection);
        
        qDebug() << "  ✅ GPS worker signals connected";
    }
    
    // === NTRIP WORKER CONNECTIONS ===
    if (m_ntripWorker) {
        // Data signals (Qt::DirectConnection for real-time corrections)
        connect(m_ntripWorker, &NTRIPWorker::ntripDataReceived,
                this, &AgIOService::onNtripDataReceived, Qt::DirectConnection);
        connect(m_ntripWorker, &NTRIPWorker::ntripStatusChanged,
                this, &AgIOService::onNtripStatusChanged, Qt::DirectConnection);
        
        // Command signals (Qt::QueuedConnection for thread-safe commands)
        connect(this, &AgIOService::requestStartNTRIP,
                m_ntripWorker, &NTRIPWorker::startNTRIP, Qt::QueuedConnection);
        connect(this, &AgIOService::requestStopNTRIP,
                m_ntripWorker, &NTRIPWorker::stopNTRIP, Qt::QueuedConnection);
        
        qDebug() << "  ✅ NTRIP worker signals connected";
    }
    
    // === UDP WORKER CONNECTIONS ===
    if (m_udpWorker) {
        // Configure UDP worker for diagnostics/auxiliary communication
        // AgIOService owns GPS on port 9999 exclusively - Phase 4.5
        // AgIOService uses port 9999 for GPS data reception
        m_udpWorker->setListenPort(9999);   // Standard AgOpenGPS UDP port
        
        // Data signals (Qt::DirectConnection for real-time communication)
        connect(m_udpWorker, &UDPWorker::udpDataReceived,
                this, &AgIOService::onUdpDataReceived, Qt::DirectConnection);
        connect(m_udpWorker, &UDPWorker::nmeaReceived,
                this, &AgIOService::onNmeaFromUdp, Qt::DirectConnection);
        connect(m_udpWorker, &UDPWorker::udpStatusChanged,
                this, &AgIOService::onUdpStatusChanged, Qt::DirectConnection);
        
        // Command signals (Qt::QueuedConnection for thread-safe commands)
        connect(this, &AgIOService::requestStartUDP,
                m_udpWorker, &UDPWorker::startUDP, Qt::QueuedConnection);
        connect(this, &AgIOService::requestStopUDP,
                m_udpWorker, &UDPWorker::stopUDP, Qt::QueuedConnection);
        
        qDebug() << "  ✅ UDP worker configured on port 9999 for GPS reception";
    }
    
    // === CROSS-WORKER DATA FLOW ===
    // NTRIP corrections → GPS worker (if available)
    if (m_ntripWorker && m_udpWorker) {
        connect(m_ntripWorker, &NTRIPWorker::ntripDataReceived,
                m_udpWorker, &UDPWorker::sendToTractor, Qt::QueuedConnection);
        qDebug() << "  ✅ NTRIP → UDP data flow connected";
    }
    
    // === SERIAL WORKER CONNECTIONS ===
    if (m_serialWorker) {
        // Data signals (Qt::DirectConnection for real-time serial data)
        connect(m_serialWorker, &SerialWorker::gpsDataReceived,
                m_gpsWorker, &GPSWorker::processNMEAString, Qt::QueuedConnection);
        connect(m_serialWorker, &SerialWorker::imuDataReceived,
                this, &AgIOService::onSerialIMUDataReceived, Qt::DirectConnection);
        connect(m_serialWorker, &SerialWorker::autosteerResponseReceived,
                this, &AgIOService::onSerialAutosteerResponse, Qt::DirectConnection);
        
        // Connection status signals
        connect(m_serialWorker, &SerialWorker::gpsConnected,
                this, &AgIOService::onSerialGPSConnected, Qt::DirectConnection);
        connect(m_serialWorker, &SerialWorker::imuConnected,
                this, &AgIOService::onSerialIMUConnected, Qt::DirectConnection);
        connect(m_serialWorker, &SerialWorker::autosteerConnected,
                this, &AgIOService::onSerialAutosteerConnected, Qt::DirectConnection);
        
        // Error signals
        connect(m_serialWorker, &SerialWorker::serialError,
                this, &AgIOService::onSerialError, Qt::DirectConnection);
        
        // Worker control signals
        connect(this, &AgIOService::requestSerialWorkerStart,
                m_serialWorker, &SerialWorker::startWorker, Qt::QueuedConnection);
        connect(this, &AgIOService::requestSerialWorkerStop,
                m_serialWorker, &SerialWorker::stopWorker, Qt::QueuedConnection);
        
        qDebug() << "  ✅ Serial worker signals connected";
    }
    
    // === ENHANCED UDP WORKER CONNECTIONS (Phase 4.5.4) ===
    if (m_udpWorker) {
        // Advanced module discovery signals
        connect(m_udpWorker, &UDPWorker::moduleDiscovered,
                this, &AgIOService::onModuleDiscovered, Qt::DirectConnection);
        connect(m_udpWorker, &UDPWorker::moduleTimeout,
                this, &AgIOService::onModuleTimeout, Qt::DirectConnection);
        connect(m_udpWorker, &UDPWorker::networkScanCompleted,
                this, &AgIOService::onNetworkScanCompleted, Qt::DirectConnection);
        connect(m_udpWorker, &UDPWorker::pgnDataReceived,
                this, &AgIOService::onPgnDataReceived, Qt::DirectConnection);
        
        // Multi-subnet discovery signals
        connect(m_udpWorker, &UDPWorker::moduleSubnetDiscovered,
                this, &AgIOService::onModuleSubnetDiscovered, Qt::DirectConnection);
        connect(m_udpWorker, &UDPWorker::subnetScanCompleted,
                this, &AgIOService::onSubnetScanCompleted, Qt::DirectConnection);
        
        qDebug() << "  ✅ Enhanced UDP worker signals connected (Phase 4.5.4 + Multi-subnet discovery)";
    }
    
    // Architecture notes:
    // - AgIOService handles GPS UDP reception on port 9999 (exclusive)
    // - AgIOService sends module commands on port 8888
    // - AgIOService complements with:
    //   * Serial GPS via GPSWorker
    //   * NTRIP corrections via NTRIPWorker  
    //   * Module communication on port 9999 (GPS data, diagnostics)
    
    // Start UDP for module communication (port 9999)
    QTimer::singleShot(1000, this, [this]() {
        if (m_initialized) {
            // AgIOService owns UDP communication completely
            emit requestStartUDP(m_udpBroadcastAddress, 8888);  // Send port
            qDebug() << "🚀 AgIOService UDP Architecture:";
            qDebug() << "  AgIOService: GPS reception on port 9999 (exclusive)";
            qDebug() << "  AgIOService: Module commands on port 8888";
            qDebug() << "  FormLoop: ❌ DISABLED - Phase 4.5 complete";
        }
    });
    
    qDebug() << "✅ All worker signals connected";
}

void AgIOService::loadDefaultSettings()
{
    qDebug() << "📂 Loading default settings...";
    
    // Set reasonable defaults
    m_ntripUrl = "rtk2go.com";
    m_ntripUser = "user@example.com";
    m_ntripPassword = "password";
    m_ntripMount = "MOUNT1";
    m_ntripPort = 2101;
    m_ntripEnabled = false;
    
    m_gpsSerialPort = "COM3";
    m_gpsSerialBaud = 115200;
    
    m_imuSerialPort = "COM4";
    m_imuSerialBaud = 115200;
    
    m_bluetoothEnabled = false;
}

void AgIOService::updateVehiclePosition()
{
    // Phase 4.1: WGS84 to Local transformation (meters)
    // Using the same algorithm as CNMEA::ConvertWGS84ToLocal
    
    // Get field origin from settings (or current position on first fix)
    static bool originSet = false;
    static double latStart = 0.0;
    static double lonStart = 0.0;
    
    if (!originSet && m_latitude != 0.0 && m_longitude != 0.0) {
        latStart = m_latitude;
        lonStart = m_longitude;
        originSet = true;
        qDebug() << "🌍 Field origin set:" << latStart << "," << lonStart;
    }
    
    if (!originSet) {
        m_vehicleXY = QPointF(0, 0);
        return;
    }
    
    // Calculate meters per degree at current latitude
    double mPerDegreeLat = 111132.92 - 559.82 * cos(2.0 * latStart * 0.01745329251994329576923690766743) 
                         + 1.175 * cos(4.0 * latStart * 0.01745329251994329576923690766743) 
                         - 0.0023 * cos(6.0 * latStart * 0.01745329251994329576923690766743);
    
    double mPerDegreeLon = 111412.84 * cos(m_latitude * 0.01745329251994329576923690766743) 
                         - 93.5 * cos(3.0 * m_latitude * 0.01745329251994329576923690766743) 
                         + 0.118 * cos(5.0 * m_latitude * 0.01745329251994329576923690766743);
    
    // Convert to local coordinates (meters from field origin)
    double northing = (m_latitude - latStart) * mPerDegreeLat;
    double easting = (m_longitude - lonStart) * mPerDegreeLon;
    
    // Update vehicle position for OpenGL rendering
    m_vehicleXY = QPointF(easting, northing);
    
    // Update additional real-time data
    m_easting = easting;
    m_northing = northing;
}

void AgIOService::logDebugInfo() const
{
    static int heartbeatCounter = 0;
    if (++heartbeatCounter % 25 == 0) { // Every 25 seconds - reduced spam
        qDebug() << "💓 AgIOService heartbeat - GPS:" << m_gpsConnected 
                << "BT:" << m_bluetoothConnected 
                << "NTRIP:" << m_ntripStatus
                << "Thread:" << QThread::currentThread();
    }
}

// Additional QML method implementations for compatibility (from FormLoop)
void AgIOService::btnUDPListenOnly_clicked(int checkState)
{
    bool isListenOnly = (checkState != 0);
    qDebug() << "🔧 UDP Listen Only Mode:" << (isListenOnly ? "ACTIVATED" : "DEACTIVATED");
    
    if (isListenOnly) {
        qDebug() << "UDP Listen Only Mode Activate!! Will Not Send To Modules!!";
        // Emit warning message to QML
        emit errorOccurred("UDP Listen Only Mode Activated!! Warning: Will Not Send To Modules!!");
    } else {
        qDebug() << "UDP Listen Only Mode Deactivated.";
        emit statusMessageChanged("UDP Listen Only Mode Deactivated");
    }
    
    // TODO: Set actual UDP listen-only flag in UDPWorker
    // m_udpWorker->setListenOnlyMode(isListenOnly);
}

void AgIOService::ntripDebug(int checkState)
{
    bool debugEnabled = (checkState != 0);
    qDebug() << "🔍 NTRIP Debug mode is now" << (debugEnabled ? "enabled" : "disabled");
    
    // TODO: Set debug flag in NTRIPWorker
    // if (m_ntripWorker) {
    //     m_ntripWorker->setDebugMode(debugEnabled);
    // }
}

void AgIOService::bluetoothDebug(int checkState)
{
    bool debugEnabled = (checkState != 0);
    qDebug() << "🔍 Bluetooth Debug mode is now" << (debugEnabled ? "enabled" : "disabled");
    
    // TODO: Set debug flag in BluetoothManager equivalent
    // Note: Our architecture doesn't have BluetoothManager yet, this connects to existing method
}

void AgIOService::bt_search(const QString& deviceName)
{
    qDebug() << "🔍 Bluetooth search and connect to device:" << deviceName;
    // Use existing connectBluetooth method which handles the connection
    connectBluetooth(deviceName);
}

void AgIOService::bt_remove_device(const QString& deviceName)
{
    qDebug() << "🗑️ Remove Bluetooth device from paired list:" << deviceName;
    // TODO: Implement device removal from Bluetooth settings/pairing
    // This would typically involve:
    // 1. Remove from system Bluetooth paired devices
    // 2. Update m_bluetoothDevices QVariantList
    // 3. Emit settingsChanged()
}

void AgIOService::setIPFromUrl(const QString& url)
{
    qDebug() << "🌐 Lookup NTRIP IP from URL:" << url;
    
    // Extract hostname from URL (FormLoop's LookupNTripIP functionality)
    QString hostname = url;
    
    // Remove protocol prefix if present
    if (hostname.startsWith("http://")) {
        hostname = hostname.mid(7);
    } else if (hostname.startsWith("https://")) {
        hostname = hostname.mid(8);
    }
    
    // Remove port and path if present  
    int colonPos = hostname.indexOf(':');
    int slashPos = hostname.indexOf('/');
    if (colonPos >= 0) {
        hostname = hostname.left(colonPos);
    } else if (slashPos >= 0) {
        hostname = hostname.left(slashPos);
    }
    
    qDebug() << "🔍 Extracted hostname for DNS lookup:" << hostname;
    
    // Perform DNS lookup (equivalent to QHostInfo::lookupHost in FormLoop)
    QHostInfo::lookupHost(hostname, this, [this](const QHostInfo& info) {
        if (info.error() == QHostInfo::NoError && !info.addresses().isEmpty()) {
            QString ipAddress = info.addresses().first().toString();
            qDebug() << "✅ DNS resolved to IP:" << ipAddress;
            
            // TODO: Update NTRIP settings with resolved IP
            // m_ntripUrl = ipAddress; 
            // emit settingsChanged();
            emit statusMessageChanged("NTRIP IP resolved: " + ipAddress);
        } else {
            qDebug() << "❌ DNS lookup failed:" << info.errorString();
            emit errorOccurred("Failed to resolve NTRIP URL: " + info.errorString());
        }
    });
}

void AgIOService::btnSendSubnet_clicked()
{
    qDebug() << "📡 Send subnet configuration to modules";
    // Use existing configureSubnet method (already implemented)  
    configureSubnet();
}

void AgIOService::sendPgn(const QByteArray& pgnData)
{
    // Phase 4.6: PGN transmission via UDPWorker (replaces FormGPS SendPgnToLoop)
    qDebug() << "📡 Sending PGN data:" << pgnData.size() << "bytes via UDPWorker";
    
    if (m_udpWorker && !pgnData.isEmpty()) {
        // Send via UDPWorker to hardware modules (port 8888)
        QMetaObject::invokeMethod(m_udpWorker, "sendToTractor", 
                                  Qt::QueuedConnection,
                                  Q_ARG(QByteArray, pgnData));
    } else {
        qWarning() << "❌ Cannot send PGN: UDPWorker not available or empty data";
    }
}

// Serial Worker slot implementations
void AgIOService::onSerialIMUDataReceived(const QByteArray& imuData)
{
    // Process IMU binary data and extract roll, pitch, yaw
    // This is a simplified implementation - actual parsing depends on IMU protocol
    
    if (imuData.size() >= 32) { // Assuming 32-byte IMU packets
        // Extract IMU data (implementation depends on specific IMU protocol)
        // For now, just emit the signal
        qDebug() << "📐 Serial IMU data received:" << imuData.size() << "bytes";
        
        // TODO: Parse actual IMU data format and update m_imuRoll, m_imuPitch, etc.
        emit imuDataChanged();
    }
}

void AgIOService::onSerialAutosteerResponse(const QByteArray& response)
{
    qDebug() << "🎛️ Serial AutoSteer response:" << response;
    
    // Process AutoSteer response - could be status, acknowledgment, or error
    // Implementation depends on AutoSteer protocol
    
    // For now, just log the response
    QString responseStr = QString::fromLatin1(response);
    qDebug() << "AutoSteer response:" << responseStr;
}

void AgIOService::onSerialGPSConnected(bool connected)
{
    qDebug() << "📡 Serial GPS connection status:" << connected;
    
    // This doesn't directly change m_gpsConnected as that might come from UDP
    // But we could add a separate serial GPS status property if needed
    
    // For now, update the main GPS status if no other GPS source is active
    if (!m_gpsConnected) {
        m_gpsConnected = connected;
        emit gpsStatusChanged();
    }
}

void AgIOService::onSerialIMUConnected(bool connected)
{
    qDebug() << "🧭 Serial IMU connection status:" << connected;
    
    // Update IMU connection status - could add a specific property for this
    if (connected) {
        qDebug() << "✅ Serial IMU connected and ready";
    } else {
        qDebug() << "❌ Serial IMU disconnected";
    }
    
    // TODO: Add specific serial IMU status property if needed
    emit imuDataChanged();
}

void AgIOService::onSerialAutosteerConnected(bool connected)
{
    qDebug() << "🎛️ Serial AutoSteer connection status:" << connected;
    
    if (connected) {
        qDebug() << "✅ Serial AutoSteer connected and ready";
    } else {
        qDebug() << "❌ Serial AutoSteer disconnected";
    }
    
    // TODO: Add specific serial AutoSteer status property if needed
}

void AgIOService::onSerialError(const QString& portName, const QString& error)
{
    qWarning() << "⚠️ Serial error on" << portName << ":" << error;
    
    // Handle serial errors - could trigger reconnection attempts or UI notifications
    // For critical errors, might want to emit specific error signals
    
    if (error.contains("Resource error") || error.contains("Device not found")) {
        qCritical() << "🚨 Critical serial error on" << portName << "- device disconnected";
        
        // Update connection status based on port
        if (portName == "GPS") {
            onSerialGPSConnected(false);
        } else if (portName == "IMU") {
            onSerialIMUConnected(false);
        } else if (portName == "AutoSteer") {
            onSerialAutosteerConnected(false);
        }
    }
}

// ========== Enhanced UI Integration (Phase 4.5.4) ==========

// UI Status getters
QString AgIOService::gpsStatusText() const
{
    if (m_gpsConnected) {
        if (m_gpsQuality == 4) {
            return QString("GPS RTK FIX - %1 sats - %2s").arg(m_satellites).arg(m_age, 0, 'f', 1);
        } else if (m_gpsQuality == 5) {
            return QString("GPS RTK FLOAT - %1 sats - %2s").arg(m_satellites).arg(m_age, 0, 'f', 1);
        } else if (m_gpsQuality > 0) {
            return QString("GPS Connected - %1 sats - %2s").arg(m_satellites).arg(m_age, 0, 'f', 1);
        }
    }
    return "GPS Disconnected";
}

QString AgIOService::moduleStatusText() const
{
    int moduleCount = m_discoveredModules.size();
    if (moduleCount > 0) {
        return QString("%1 Modules Found").arg(moduleCount);
    }
    return "No Modules Detected";
}

QString AgIOService::serialStatusText() const
{
    QStringList connectedPorts;
    // This would need to track actual serial connection status
    // For now, return a basic status
    return "Serial: Ready";
}

QVariantList AgIOService::discoveredModules() const
{
    return m_discoveredModules;
}

// UI Control methods
void AgIOService::updateGPSStatus()
{
    QString newStatus = gpsStatusText();
    if (m_gpsStatusText != newStatus) {
        m_gpsStatusText = newStatus;
        emit statusChanged();
    }
}

void AgIOService::updateModuleStatus()
{
    QString newStatus = moduleStatusText();
    if (m_moduleStatusText != newStatus) {
        m_moduleStatusText = newStatus;
        emit statusChanged();
    }
}

void AgIOService::updateNTRIPStatus()
{
    QString newStatus;
    switch (m_ntripStatus) {
        case 0: newStatus = "NTRIP Off"; break;
        case 1: newStatus = "NTRIP Connecting"; break;
        case 2: newStatus = "NTRIP Connected"; break;
        case 3: newStatus = "NTRIP Receiving"; break;
        case 4: newStatus = "NTRIP Active"; break;
        default: newStatus = "NTRIP Error"; break;
    }
    
    if (m_ntripStatusTextInternal != newStatus) {
        m_ntripStatusTextInternal = newStatus;
        emit statusChanged();
    }
}

void AgIOService::updateSerialStatus()
{
    QString newStatus = serialStatusText();
    if (m_serialStatusText != newStatus) {
        m_serialStatusText = newStatus;
        emit statusChanged();
    }
}

void AgIOService::clearErrorDialog()
{
    if (m_showErrorDialog) {
        m_showErrorDialog = false;
        m_lastErrorMessage = "";
        emit errorOccurred("");
    }
}

void AgIOService::startModuleDiscovery()
{
    qDebug() << "🔍 UI: Starting module discovery";
    if (m_udpWorker) {
        // Call UDP worker's startModuleDiscovery via Qt::QueuedConnection
        QMetaObject::invokeMethod(m_udpWorker, "startModuleDiscovery", Qt::QueuedConnection);
    }
}

void AgIOService::stopModuleDiscovery()
{
    qDebug() << "🛑 UI: Stopping module discovery";
    if (m_udpWorker) {
        // Call UDP worker's stopModuleDiscovery via Qt::QueuedConnection
        QMetaObject::invokeMethod(m_udpWorker, "stopModuleDiscovery", Qt::QueuedConnection);
    }
}

// Enhanced UDP Worker slots
void AgIOService::onModuleDiscovered(const QString& moduleIP, const QString& moduleType)
{
    qDebug() << "🎯 Module discovered via UDP:" << moduleIP << "Type:" << moduleType;
    
    // Add to discovered modules list
    QVariantMap moduleInfo;
    moduleInfo["ip"] = moduleIP;
    moduleInfo["type"] = moduleType;
    moduleInfo["lastSeen"] = QDateTime::currentDateTime().toString();
    
    // Check if module already exists
    bool found = false;
    for (int i = 0; i < m_discoveredModules.size(); ++i) {
        QVariantMap existing = m_discoveredModules[i].toMap();
        if (existing["ip"].toString() == moduleIP) {
            // Update existing module
            m_discoveredModules[i] = moduleInfo;
            found = true;
            break;
        }
    }
    
    if (!found) {
        m_discoveredModules.append(moduleInfo);
    }
    
    updateModuleStatus();
    emit moduleDiscoveryChanged();
}

void AgIOService::onModuleTimeout(const QString& moduleIP)
{
    qDebug() << "⏰ Module timeout:" << moduleIP;
    
    // Remove from discovered modules list
    for (int i = m_discoveredModules.size() - 1; i >= 0; --i) {
        QVariantMap moduleInfo = m_discoveredModules[i].toMap();
        if (moduleInfo["ip"].toString() == moduleIP) {
            m_discoveredModules.removeAt(i);
            break;
        }
    }
    
    updateModuleStatus();
    emit moduleDiscoveryChanged();
}

void AgIOService::onNetworkScanCompleted(const QStringList& discoveredModules)
{
    qDebug() << "🌐 Network scan completed:" << discoveredModules.size() << "modules";
    updateModuleStatus();
}

void AgIOService::onPgnDataReceived(const QByteArray& pgnData)
{
    // Process PGN data from modules
    if (pgnData.size() >= 3) {
        quint8 pgnType = static_cast<quint8>(pgnData[1]);
        
        switch (pgnType) {
            case 0x83: // GPS data from module
                qDebug() << "📍 GPS PGN data received:" << pgnData.size() << "bytes";
                break;
                
            case 0x84: // IMU data from module
                qDebug() << "📐 IMU PGN data received:" << pgnData.size() << "bytes";
                break;
                
            case 0x85: // Section control data
                qDebug() << "🚜 Section PGN data received:" << pgnData.size() << "bytes";
                break;
                
            default:
                qDebug() << "📦 Unknown PGN data:" << QString::number(pgnType, 16);
                break;
        }
    }
}

// Multi-subnet discovery slots
void AgIOService::onModuleSubnetDiscovered(const QString& moduleIP, const QString& currentSubnet)
{
    qDebug() << "🎯 Module subnet discovered - IP:" << moduleIP << "Subnet:" << currentSubnet;
    
    // Update status for UI
    m_moduleStatusText = QString("Module found on subnet %1 (IP: %2)").arg(currentSubnet, moduleIP);
    emit statusChanged();
    emit statusMessageChanged("Module discovered on subnet " + currentSubnet);
}

void AgIOService::onSubnetScanCompleted(const QString& activeSubnet)
{
    if (activeSubnet.isEmpty()) {
        qDebug() << "❌ Subnet scan completed - no active modules found";
        m_moduleStatusText = "No AgOpenGPS modules found on network";
        emit statusMessageChanged("No modules found - please check module power and network connection");
    } else {
        qDebug() << "✅ Subnet scan completed - active subnet:" << activeSubnet;
        m_moduleStatusText = QString("Module active on subnet %1").arg(activeSubnet);
        emit statusMessageChanged("Module configured successfully on subnet " + activeSubnet);
    }
    
    emit statusChanged();
}