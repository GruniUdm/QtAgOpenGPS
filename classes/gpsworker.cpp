#include "gpsworker.h"
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QtMath>

GPSWorker::GPSWorker(QObject *parent)
    : QObject(parent)
    , m_serialPort(nullptr)
    , m_nmeaParser(nullptr)
    , m_statusTimer(nullptr)
    , m_reconnectTimer(nullptr)
    , m_isConnected(false)
    , m_isRunning(false)
    , m_baudRate(115200)
    , m_reconnectAttempts(0)
    , m_lastDataTime(0)
    , m_sentenceCount(0)
    , m_dataTimeoutMs(5000)
    , m_latitude(0.0)
    , m_longitude(0.0)
    , m_heading(0.0)
    , m_speed(0.0)
    , m_gpsQuality(0)
    , m_satellites(0)
    , m_age(0.0)
    // Enhanced GPS state (Phase 4.5.2)
    , m_hdop(0.0)
    , m_pdop(0.0) 
    , m_vdop(0.0)
    , m_altitude(0.0)
    , m_geoidHeight(0.0)
    , m_accuracy(0.0)
    , m_rtkStatus("NONE")
    , m_roll(0.0)
    , m_pitch(0.0)
    , m_satellitesInView(0)
{
    qDebug() << "🔧 GPSWorker constructor - Thread:" << QThread::currentThread();
    
    // Create serial port
    m_serialPort = new QSerialPort(this);
    connect(m_serialPort, &QSerialPort::readyRead, this, &GPSWorker::onSerialDataReceived);
    connect(m_serialPort, QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::errorOccurred),
            this, &GPSWorker::onSerialError);
    
    // Create NMEA parser (from existing codebase)
    m_nmeaParser = new CNMEA(this);
    
    // Status timer for connection monitoring
    m_statusTimer = new QTimer(this);
    m_statusTimer->setInterval(1000); // 1Hz status check
    connect(m_statusTimer, &QTimer::timeout, this, &GPSWorker::checkConnectionStatus);
    
    // Reconnection timer
    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setInterval(5000); // 5s reconnect attempts
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &GPSWorker::attemptReconnect);
    
    qDebug() << "✅ GPSWorker created";
}

GPSWorker::~GPSWorker()
{
    qDebug() << "🔧 GPSWorker destructor";
    stopGPS();
}

void GPSWorker::startGPS(const QString& serialPort, int baudRate)
{
    qDebug() << "🚀 Starting GPS worker - Port:" << serialPort << "Baud:" << baudRate 
             << "Thread:" << QThread::currentThread();
    
    if (m_isRunning) {
        qDebug() << "⚠️ GPS worker already running";
        return;
    }
    
    m_portName = serialPort;
    m_baudRate = baudRate;
    m_isRunning = true;
    m_reconnectAttempts = 0;
    
    // Configure serial port
    m_serialPort->setPortName(m_portName);
    m_serialPort->setBaudRate(m_baudRate);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    
    // Try to open serial port
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        m_isConnected = true;
        m_lastDataTime = QDateTime::currentMSecsSinceEpoch();
        
        // Start status monitoring
        m_statusTimer->start();
        
        qDebug() << "✅ GPS serial port opened successfully";
        emit gpsStatusChanged(true, m_gpsQuality, m_satellites, m_age);
    } else {
        qDebug() << "❌ Failed to open GPS serial port:" << m_serialPort->errorString();
        m_isConnected = false;
        emit gpsStatusChanged(false, 0, 0, 0.0);
        emit errorOccurred("Failed to open GPS serial port: " + m_serialPort->errorString());
        
        // Schedule reconnection attempt
        if (m_reconnectAttempts < 10) {
            m_reconnectTimer->start();
        }
    }
}

void GPSWorker::stopGPS()
{
    if (!m_isRunning) {
        return;
    }
    
    qDebug() << "🛑 Stopping GPS worker";
    
    m_isRunning = false;
    m_isConnected = false;
    
    // Stop timers
    m_statusTimer->stop();
    m_reconnectTimer->stop();
    
    // Close serial port
    if (m_serialPort && m_serialPort->isOpen()) {
        m_serialPort->close();
    }
    
    // Reset data
    resetGpsData();
    
    emit gpsStatusChanged(false, 0, 0, 0.0);
    qDebug() << "✅ GPS worker stopped";
}

void GPSWorker::processNMEAString(const QString& nmeaSentence)
{
    if (nmeaSentence.isEmpty()) {
        return;
    }
    
    qDebug() << "📡 GPSWorker processing NMEA from SerialWorker:" << nmeaSentence;
    
    // Update last data time for connection status
    m_lastDataTime = QDateTime::currentMSecsSinceEpoch();
    m_sentenceCount++;
    
    // Process the NMEA sentence
    QString line = nmeaSentence.trimmed();
    if (line.startsWith("$GPGGA") || line.startsWith("$GNGGA")) {
        processGGALine(line);
    }
    else if (line.startsWith("$GPRMC") || line.startsWith("$GNRMC")) {
        processRMCLine(line);
    }
    else if (line.startsWith("$GPVTG") || line.startsWith("$GNVTG")) {
        processVTGLine(line);
    }
    // Enhanced NMEA parsing (Phase 4.5.2)
    else if (line.startsWith("$GPGST") || line.startsWith("$GNGST")) {
        processGSTLine(line);
    }
    else if (line.startsWith("$GPGSV") || line.startsWith("$GNGSV")) {
        processGSVLine(line);
    }
    else if (line.startsWith("$GPHDT") || line.startsWith("$GNHDT")) {
        processHDTLine(line);
    }
    else if (line.startsWith("$PANDA")) {
        parsePANDA(line);  // CRITIQUE pour F9P modules
    }
    
    // Update connection status if needed
    if (!m_isConnected) {
        m_isConnected = true;
        updateConnectionStatus();
    }
}

void GPSWorker::onSerialDataReceived()
{
    if (!m_isConnected || !m_serialPort) {
        return;
    }
    
    // Read available data
    QByteArray data = m_serialPort->readAll();
    if (data.isEmpty()) {
        return;
    }
    
    // Add to buffer
    m_serialBuffer.append(data);
    
    // Update last data timestamp
    m_lastDataTime = QDateTime::currentMSecsSinceEpoch();
    
    // Process complete NMEA sentences
    processSerialBuffer();
}

void GPSWorker::onSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
        return;
    }
    
    qDebug() << "❌ GPS serial error:" << error << m_serialPort->errorString();
    
    m_isConnected = false;
    emit gpsStatusChanged(false, 0, 0, 0.0);
    emit errorOccurred("GPS serial error: " + m_serialPort->errorString());
    
    // Schedule reconnection if running
    if (m_isRunning && m_reconnectAttempts < 10) {
        m_reconnectTimer->start();
    }
}

void GPSWorker::checkConnectionStatus()
{
    if (!m_isRunning) {
        return;
    }
    
    // Check for data timeout
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 timeSinceData = currentTime - m_lastDataTime;
    
    if (m_isConnected && timeSinceData > m_dataTimeoutMs) {
        qDebug() << "⚠️ GPS data timeout - last data" << timeSinceData << "ms ago";
        m_isConnected = false;
        resetGpsData();
        emit gpsStatusChanged(false, 0, 0, 0.0);
        
        // Schedule reconnection
        if (m_reconnectAttempts < 10) {
            m_reconnectTimer->start();
        }
    }
    
    // Update age calculation
    if (m_isConnected && m_lastDataTime > 0) {
        m_age = timeSinceData / 1000.0; // Convert to seconds
    }
}

void GPSWorker::processSerialBuffer()
{
    // Process complete NMEA sentences (ending with \r\n)
    while (m_serialBuffer.contains('\n')) {
        int newlineIndex = m_serialBuffer.indexOf('\n');
        QByteArray line = m_serialBuffer.left(newlineIndex);
        m_serialBuffer.remove(0, newlineIndex + 1);
        
        // Remove carriage return if present
        if (line.endsWith('\r')) {
            line.chop(1);
        }
        
        // Process NMEA sentence
        QString sentence = QString::fromLatin1(line);
        if (!sentence.isEmpty() && sentence.startsWith('$')) {
            processNmeaData(sentence.toLatin1());
        }
    }
    
    // Limit buffer size to prevent memory issues
    if (m_serialBuffer.size() > 4096) {
        m_serialBuffer = m_serialBuffer.right(2048);
    }
}

void GPSWorker::processNmeaData(const QByteArray& data)
{
    QString sentence = QString::fromLatin1(data);
    
    // Validate checksum
    if (!validateChecksum(sentence)) {
        return;
    }
    
    // Count valid sentences
    m_sentenceCount++;
    
    // Debug output (limited frequency)
    static int debugCounter = 0;
    if (++debugCounter % 30 == 0) { // Every 30 sentences (~10s at 3Hz)
        qDebug() << "📡 NMEA sentence:" << sentence.left(50) << "... Thread:" << QThread::currentThread();
    }
    
    // Process specific sentence types
    if (sentence.startsWith("$GPGGA") || sentence.startsWith("$GNGGA")) {
        processGGALine(sentence);
    } else if (sentence.startsWith("$GPRMC") || sentence.startsWith("$GNRMC")) {
        processRMCLine(sentence);
    } else if (sentence.startsWith("$GPVTG") || sentence.startsWith("$GNVTG")) {
        processVTGLine(sentence);
    }
}

void GPSWorker::processGGALine(const QString& line)
{
    // Parse GGA sentence: $GPGGA,time,lat,latNS,lon,lonEW,quality,satellites,hdop,altitude,M,geoid,M,dgps_time,dgps_id*checksum
    QStringList fields = parseNmeaSentence(line);
    if (fields.size() < 15) {
        return;
    }
    
    // Extract GPS quality and satellite count
    bool qualityOk, satOk;
    int quality = fields[6].toInt(&qualityOk);
    int satellites = fields[7].toInt(&satOk);
    
    if (qualityOk && satOk) {
        m_gpsQuality = quality;
        m_satellites = satellites;
        
        // Enhanced GGA parsing (Phase 4.5.2): Extract HDOP, altitude, geoid height
        bool hdopOk, altOk, geoidOk;
        double hdop = fields[8].toDouble(&hdopOk);
        double altitude = fields[9].toDouble(&altOk);
        double geoidHeight = fields[11].toDouble(&geoidOk);
        
        if (hdopOk) m_hdop = hdop;
        if (altOk) m_altitude = altitude; 
        if (geoidOk) m_geoidHeight = geoidHeight;
        
        // Extract position if quality > 0
        if (quality > 0 && !fields[2].isEmpty() && !fields[4].isEmpty()) {
            double lat = convertNmeaLatLon(fields[2], fields[3]);
            double lon = convertNmeaLatLon(fields[4], fields[5]);
            
            if (lat != 0.0 && lon != 0.0) {
                m_latitude = lat;
                m_longitude = lon;
                
                // Enhanced processing (Phase 4.5.2)
                m_lastValidFix = QDateTime::currentDateTime();
                calculateGPSAge();
                processDifferentialCorrections();
                calculatePositionAccuracy();
                handleRTKStatus();
                validateGPSFix();
                
                emitGpsData();
            }
        }
        
        // Emit enhanced status update
        emit gpsStatusChanged(quality > 0, quality, satellites, m_age);
    }
}

void GPSWorker::processRMCLine(const QString& line)
{
    // Parse RMC sentence: $GPRMC,time,status,lat,latNS,lon,lonEW,speed,course,date,magvar,magvarEW*checksum
    QStringList fields = parseNmeaSentence(line);
    if (fields.size() < 12) {
        return;
    }
    
    // Check if data is valid (status 'A' = active)
    if (fields[2] != "A") {
        return;
    }
    
    // Extract speed and heading
    bool speedOk, headingOk;
    double speedKnots = fields[7].toDouble(&speedOk);
    double heading = fields[8].toDouble(&headingOk);
    
    if (speedOk && headingOk) {
        m_speed = speedKnots * 1.852; // Convert knots to km/h
        m_heading = heading;
        
        // Extract position if available
        if (!fields[3].isEmpty() && !fields[5].isEmpty()) {
            double lat = convertNmeaLatLon(fields[3], fields[4]);
            double lon = convertNmeaLatLon(fields[5], fields[6]);
            
            if (lat != 0.0 && lon != 0.0) {
                m_latitude = lat;
                m_longitude = lon;
                emitGpsData();
            }
        }
    }
}

void GPSWorker::processVTGLine(const QString& line)
{
    // Parse VTG sentence: $GPVTG,course,T,course,M,speed_knots,N,speed_kmh,K*checksum
    QStringList fields = parseNmeaSentence(line);
    if (fields.size() < 9) {
        return;
    }
    
    // Extract true heading and speed
    bool headingOk, speedOk;
    double heading = fields[1].toDouble(&headingOk);
    double speedKmh = fields[7].toDouble(&speedOk);
    
    if (headingOk && speedOk) {
        m_heading = heading;
        m_speed = speedKmh;
        
        // Emit the raw VTG sentence for display
        emit nmeaSentenceReceived("VTG", line);
    }
}

// Enhanced NMEA parsing (Phase 4.5.2)
void GPSWorker::processGSTLine(const QString& line)
{
    // Parse GST sentence: $GPGST,time,rms,smjr,smnr,orient,lat_err,lon_err,alt_err*checksum
    QStringList fields = parseNmeaSentence(line);
    if (fields.size() < 8) {
        return;
    }
    
    // Extract position accuracy from GST
    bool latErrOk, lonErrOk, altErrOk;
    double latErr = fields[6].toDouble(&latErrOk);
    double lonErr = fields[7].toDouble(&lonErrOk);
    double altErr = fields[8].toDouble(&altErrOk);
    
    if (latErrOk && lonErrOk) {
        // Calculate position accuracy as RMS of lat/lon errors
        m_accuracy = qSqrt(latErr * latErr + lonErr * lonErr);
        qDebug() << "📊 GPS Accuracy (GST):" << m_accuracy << "m";
    }
}

void GPSWorker::processGSVLine(const QString& line)
{
    // Parse GSV sentence: $GPGSV,msgs,msg#,sats,sat1,elev1,az1,snr1,...*checksum
    QStringList fields = parseNmeaSentence(line);
    if (fields.size() < 4) {
        return;
    }
    
    // Extract total satellites in view
    bool satsOk;
    int satellitesInView = fields[3].toInt(&satsOk);
    if (satsOk) {
        m_satellitesInView = satellitesInView;
        qDebug() << "🛰️ Satellites in view (GSV):" << m_satellitesInView;
    }
}

void GPSWorker::processHDTLine(const QString& line)
{
    // Parse HDT sentence: $GPHDT,heading,T*checksum  
    QStringList fields = parseNmeaSentence(line);
    if (fields.size() < 2) {
        return;
    }
    
    // Extract true heading
    bool headingOk;
    double heading = fields[1].toDouble(&headingOk);
    if (headingOk) {
        m_heading = heading;
        qDebug() << "🧭 True Heading (HDT):" << m_heading << "°";
    }
}

void GPSWorker::parsePANDA(const QString& line)
{
    // Parse PANDA sentence: $PANDA,103013.00,5001.234567,N,00425.345678,E,4,12,0.8,123.4,M,45.6,M,2.1,0*3F
    // CRITIQUE pour modules F9P - contient Roll/Pitch dans les champs 12-13
    QStringList fields = parseNmeaSentence(line);
    if (fields.size() < 15) {
        qDebug() << "⚠️ PANDA sentence too short:" << fields.size() << "fields";
        return;
    }
    
    qDebug() << "🔥 PANDA F9P Processing:" << line;
    
    // Extract position data (standard GPS)
    double lat = convertNmeaLatLon(fields[2], fields[3]);  
    double lon = convertNmeaLatLon(fields[4], fields[5]);
    bool qualityOk, satellitesOk, hdopOk;
    int quality = fields[6].toInt(&qualityOk);
    int satellites = fields[7].toInt(&satellitesOk);  
    double hdop = fields[8].toDouble(&hdopOk);
    
    if (lat != 0.0 && lon != 0.0) {
        m_latitude = lat;
        m_longitude = lon;
        qDebug() << "📍 F9P Position:" << lat << lon;
    }
    
    if (qualityOk && satellitesOk && hdopOk) {
        m_gpsQuality = quality;
        m_satellites = satellites;
        m_hdop = hdop;
    }
    
    // CRITIQUE: Extract F9P Roll/Pitch from fields 12-13
    if (fields.size() >= 14) {
        bool rollOk, pitchOk;
        double roll = fields[12].toDouble(&rollOk);
        double pitch = fields[13].toDouble(&pitchOk);
        
        if (rollOk && pitchOk) {
            m_roll = roll;
            m_pitch = pitch;
            qDebug() << "🎯 F9P Roll/Pitch:" << roll << "/" << pitch << "°";
            
            // Emit enhanced GPS data with F9P IMU
            emit gpsDataReceived(m_latitude, m_longitude, m_heading, m_speed);
        }
    }
    
    // Update last valid fix time
    m_lastValidFix = QDateTime::currentDateTime();
    calculateGPSAge();
}

// GPS quality management
void GPSWorker::calculateGPSAge()
{
    if (m_lastValidFix.isValid()) {
        qint64 msecsSinceLastFix = m_lastValidFix.msecsTo(QDateTime::currentDateTime());
        m_age = msecsSinceLastFix / 1000.0;  // Convert to seconds
    } else {
        m_age = 999.9;  // No valid fix
    }
}

void GPSWorker::validateGPSFix() 
{
    // Validate GPS fix quality
    bool hasValidFix = (m_gpsQuality > 0) && (m_satellites >= 4) && (m_age < 5.0);
    
    if (!hasValidFix && m_isConnected) {
        qDebug() << "⚠️ GPS fix invalid - Quality:" << m_gpsQuality 
                 << "Sats:" << m_satellites << "Age:" << m_age;
        handleGPSLoss();
    }
}

void GPSWorker::handleGPSLoss()
{
    qDebug() << "🚨 GPS signal lost - initiating recovery";
    m_age = 999.9;
    m_gpsQuality = 0;
    
    // Emit status change
    emit gpsStatusChanged(false, m_gpsQuality, m_satellites, m_age);
}

// Advanced processing
void GPSWorker::processDifferentialCorrections()
{
    // Process RTK/DGPS corrections based on quality
    switch (m_gpsQuality) {
        case 4: // RTK Fixed
            m_rtkStatus = "FIX";
            m_accuracy = 0.02;  // 2cm accuracy
            break;
        case 5: // RTK Float  
            m_rtkStatus = "FLOAT";
            m_accuracy = 0.5;   // 50cm accuracy
            break;
        case 2: // DGPS
            m_rtkStatus = "DGPS"; 
            m_accuracy = 1.0;   // 1m accuracy
            break;
        default:
            m_rtkStatus = "SINGLE";
            m_accuracy = 3.0;   // 3m accuracy
            break;
    }
}

void GPSWorker::calculatePositionAccuracy()
{
    // Calculate position accuracy based on HDOP and fix quality
    if (m_hdop > 0.0) {
        double baseAccuracy = 1.0;  // Base accuracy in meters
        
        switch (m_gpsQuality) {
            case 4: baseAccuracy = 0.02; break;  // RTK Fixed
            case 5: baseAccuracy = 0.5; break;   // RTK Float
            case 2: baseAccuracy = 1.0; break;   // DGPS
            default: baseAccuracy = 3.0; break;  // Autonomous
        }
        
        m_accuracy = baseAccuracy * m_hdop;
    }
}

void GPSWorker::handleRTKStatus()
{
    // Handle RTK status changes and emit updates
    static QString lastRtkStatus = "";
    if (m_rtkStatus != lastRtkStatus) {
        qDebug() << "🎯 RTK Status changed:" << lastRtkStatus << "→" << m_rtkStatus;
        lastRtkStatus = m_rtkStatus;
        
        // Emit enhanced GPS status with RTK info
        emit gpsStatusChanged(m_isConnected, m_gpsQuality, m_satellites, m_age);
    }
}

void GPSWorker::attemptReconnect()
{
    if (!m_isRunning || m_isConnected) {
        return;
    }
    
    m_reconnectAttempts++;
    qDebug() << "🔄 GPS reconnection attempt" << m_reconnectAttempts;
    
    // Close existing connection
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
    
    // Try to reopen
    if (m_serialPort->open(QIODevice::ReadWrite)) {
        m_isConnected = true;
        m_lastDataTime = QDateTime::currentMSecsSinceEpoch();
        m_reconnectAttempts = 0;
        
        qDebug() << "✅ GPS reconnected successfully";
        emit gpsStatusChanged(true, m_gpsQuality, m_satellites, m_age);
    } else {
        qDebug() << "❌ GPS reconnection failed:" << m_serialPort->errorString();
        
        // Schedule next attempt if not exceeded limit
        if (m_reconnectAttempts < 10) {
            m_reconnectTimer->start();
        } else {
            qDebug() << "❌ GPS reconnection attempts exceeded limit";
            emit errorOccurred("GPS reconnection failed after " + QString::number(m_reconnectAttempts) + " attempts");
        }
    }
}

void GPSWorker::updateConnectionStatus()
{
    // Called periodically to update connection status
}

void GPSWorker::resetGpsData()
{
    m_latitude = 0.0;
    m_longitude = 0.0;
    m_heading = 0.0;
    m_speed = 0.0;
    m_gpsQuality = 0;
    m_satellites = 0;
    m_age = 0.0;
    m_sentenceCount = 0;
}

void GPSWorker::emitGpsData()
{
    // Emit GPS data with Qt::DirectConnection for real-time performance
    emit gpsDataReceived(m_latitude, m_longitude, m_heading, m_speed);
    
    // Debug output (limited frequency)
    static int dataCounter = 0;
    if (++dataCounter % 10 == 0) { // Every 10 updates
        qDebug() << "📍 GPS data emitted:" << m_latitude << m_longitude 
                << m_heading << m_speed << "Thread:" << QThread::currentThread();
    }
}

// NMEA parsing helpers
QStringList GPSWorker::parseNmeaSentence(const QString& sentence)
{
    // Remove checksum part (*XX)
    QString data = sentence;
    int checksumIndex = data.indexOf('*');
    if (checksumIndex > 0) {
        data = data.left(checksumIndex);
    }
    
    // Split by comma
    return data.split(',');
}

double GPSWorker::convertNmeaLatLon(const QString& value, const QString& hemisphere)
{
    if (value.isEmpty()) {
        return 0.0;
    }
    
    bool ok;
    double coord = value.toDouble(&ok);
    if (!ok) {
        return 0.0;
    }
    
    // Convert DDMM.MMMMM to DD.DDDDDD
    int degrees = static_cast<int>(coord / 100);
    double minutes = coord - (degrees * 100);
    double result = degrees + (minutes / 60.0);
    
    // Apply hemisphere
    if (hemisphere == "S" || hemisphere == "W") {
        result = -result;
    }
    
    return result;
}

bool GPSWorker::validateChecksum(const QString& sentence)
{
    // Find checksum position
    int checksumIndex = sentence.indexOf('*');
    if (checksumIndex < 0 || checksumIndex + 3 != sentence.length()) {
        return false; // No checksum or invalid format
    }
    
    // Extract checksum
    QString checksumStr = sentence.mid(checksumIndex + 1, 2);
    bool ok;
    int expectedChecksum = checksumStr.toInt(&ok, 16);
    if (!ok) {
        return false;
    }
    
    // Calculate checksum
    int calculatedChecksum = 0;
    for (int i = 1; i < checksumIndex; ++i) { // Start after '$'
        calculatedChecksum ^= sentence.at(i).toLatin1();
    }
    
    return calculatedChecksum == expectedChecksum;
}