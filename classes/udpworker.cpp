#include "udpworker.h"
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QTime>
#include <QNetworkInterface>

#ifdef Q_OS_WIN
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

UDPWorker::UDPWorker(QObject *parent)
    : QObject(parent)
    , m_udpSocket(nullptr)
    , m_statusTimer(nullptr)
    , m_heartbeatTimer(nullptr)
    , m_tractorAddress(QHostAddress::LocalHost)
    , m_tractorPort(8888)  // Default AgOpenGPS port (sending to modules)
    , m_listenPort(9999)  // Default GPS listening port
    , m_isRunning(false)
    , m_isConnected(false)
    , m_lastDataTime(0)
    , m_dataTimeoutMs(10000) // 10 second timeout
    , m_bytesReceived(0)
    , m_bytesSent(0)
    , m_packetsReceived(0)
    , m_packetsSent(0)
    , m_receiveRate(0.0)
    , m_sendRate(0.0)
    // Advanced UDP module management (Phase 4.5.3)
    , m_discoveryTimer(nullptr)
    , m_timeoutTimer(nullptr)
    , m_discoveryActive(false)
    // Multi-subnet discovery management
    , m_subnetScanTimer(nullptr)
    , m_currentSubnetIndex(0)
    , m_subnetScanActive(false)
{
    qDebug() << "🔧 UDPWorker constructor - Thread:" << QThread::currentThread();
    
    // Create UDP socket
    m_udpSocket = new QUdpSocket(this);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UDPWorker::onUdpDataReceived);
    connect(m_udpSocket, QOverload<QAbstractSocket::SocketError>::of(&QUdpSocket::errorOccurred),
            this, &UDPWorker::onUdpError);
    
    // Status timer for connection monitoring
    m_statusTimer = new QTimer(this);
    m_statusTimer->setInterval(2000); // 2s status check
    connect(m_statusTimer, &QTimer::timeout, this, &UDPWorker::checkConnectionStatus);
    
    // Heartbeat timer for keep-alive
    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setInterval(5000); // 5s heartbeat
    connect(m_heartbeatTimer, &QTimer::timeout, this, &UDPWorker::sendHeartbeat);
    
    // Advanced UDP timers (Phase 4.5.3)
    m_discoveryTimer = new QTimer(this);
    m_discoveryTimer->setInterval(2000); // 2s discovery broadcast
    connect(m_discoveryTimer, &QTimer::timeout, this, &UDPWorker::broadcastHello);
    
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setInterval(10000); // 10s module timeout check
    connect(m_timeoutTimer, &QTimer::timeout, this, &UDPWorker::checkModuleTimeouts);
    
    // Multi-subnet discovery timer
    m_subnetScanTimer = new QTimer(this);
    m_subnetScanTimer->setInterval(5000); // 5s timeout per subnet (increased for slow modules)
    m_subnetScanTimer->setSingleShot(true);
    connect(m_subnetScanTimer, &QTimer::timeout, this, [this]() {
        // Timeout - try next subnet
        qDebug() << "⏰ Subnet scan timeout - trying next subnet";
        m_currentSubnetIndex++;
        scanAllSubnets();
    });
    
    // Initialize common subnets to scan
    m_commonSubnets << "192.168.1" << "192.168.5" << "192.168.4" << "192.168.0" << "10.0.0" << "172.16.0";
    
    qDebug() << "✅ UDPWorker created with advanced protocols and multi-subnet discovery";
}

UDPWorker::~UDPWorker()
{
    qDebug() << "🔧 UDPWorker destructor";
    stopUDP();
}

void UDPWorker::startUDP(const QString& address, int port)
{
    qDebug() << "🚀 Starting UDP worker - Address:" << address << "Port:" << port 
             << "Listen:" << m_listenPort << "Thread:" << QThread::currentThread();
    
    if (m_isRunning) {
        qDebug() << "⚠️ UDP worker already running";
        return;
    }
    
    // Parse target address
    if (!address.isEmpty()) {
        m_tractorAddress = QHostAddress(address);
        if (m_tractorAddress.isNull()) {
            m_tractorAddress = QHostAddress::LocalHost;
        }
    }
    
    if (port > 0) {
        m_tractorPort = port;
    }
    
    m_isRunning = true;
    resetStatistics();
    
    // Bind socket for listening
    if (bindSocket()) {
        m_isConnected = true;
        
        // Start timers
        m_statusTimer->start();
        m_heartbeatTimer->start();
        
        emit udpStatusChanged(true);
        qDebug() << "✅ UDP worker started successfully";
        
        // Send initial heartbeat
        sendHeartbeat();
    } else {
        m_isRunning = false;
        m_isConnected = false;
        emit udpStatusChanged(false);
        emit errorOccurred("Failed to bind UDP socket");
        qDebug() << "❌ Failed to start UDP worker";
    }
}

void UDPWorker::stopUDP()
{
    if (!m_isRunning) {
        return;
    }
    
    qDebug() << "🛑 Stopping UDP worker";
    
    m_isRunning = false;
    m_isConnected = false;
    
    // Stop timers
    m_statusTimer->stop();
    m_heartbeatTimer->stop();
    
    // Close socket
    cleanupConnection();
    
    emit udpStatusChanged(false);
    qDebug() << "✅ UDP worker stopped";
}

void UDPWorker::sendToTractor(const QByteArray& data)
{
    if (!m_isRunning || !m_udpSocket || data.isEmpty()) {
        return;
    }
    
    // Send data to tractor (AgOpenGPS)
    qint64 bytesWritten = m_udpSocket->writeDatagram(data, m_tractorAddress, m_tractorPort);
    
    if (bytesWritten == -1) {
        qDebug() << "❌ Failed to send UDP data:" << m_udpSocket->errorString();
        emit errorOccurred("Failed to send UDP data: " + m_udpSocket->errorString());
    } else {
        // Update statistics
        m_bytesSent += bytesWritten;
        m_packetsSent++;
        updateSendRate();
        
        // Debug output (limited frequency)
        static int sendCounter = 0;
        if (++sendCounter % 50 == 0) { // Every 50 packets
            qDebug() << "📤 UDP data sent:" << bytesWritten << "bytes to" 
                    << m_tractorAddress.toString() << ":" << m_tractorPort;
        }
    }
}

void UDPWorker::sendRawMessage(const QByteArray& data, const QString& address, int port)
{
    if (!m_isRunning || data.isEmpty()) {
        qDebug() << "🚨 Cannot send raw message - UDP not running or invalid data";
        return;
    }
    
    QHostAddress targetAddr(address);
    if (targetAddr.isNull()) {
        qDebug() << "❌ Invalid target address:" << address;
        return;
    }

    // Use multi-interface sending for cross-subnet capability (like AgIO C#)
    qDebug() << "📡 Sending raw message via multi-interface broadcast";
    sendToAllInterfaces(data, address, port);
}

void UDPWorker::onUdpDataReceived()
{
    while (m_udpSocket && m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        
        qint64 bytesRead = m_udpSocket->readDatagram(datagram.data(), datagram.size(), 
                                                    &sender, &senderPort);
        
        if (bytesRead > 0) {
            // Update statistics
            m_bytesReceived += bytesRead;
            m_packetsReceived++;
            m_lastDataTime = QDateTime::currentMSecsSinceEpoch();
            updateReceiveRate();
            
            // Multi-subnet discovery: detect module subnet from incoming data
            if (m_subnetScanActive && !sender.isNull()) {
                QString senderIP = sender.toString();
                // Handle IPv6-mapped IPv4 addresses (::ffff:192.168.1.126 -> 192.168.1.126)
                if (senderIP.startsWith("::ffff:")) {
                    senderIP = senderIP.mid(7); // Remove "::ffff:" prefix
                }
                
                // Extract subnet (e.g., 192.168.1.126 -> 192.168.1)
                QStringList ipParts = senderIP.split('.');
                if (ipParts.size() >= 3) {
                    QString detectedSubnet = ipParts[0] + "." + ipParts[1] + "." + ipParts[2];
                    
                    if (m_commonSubnets.contains(detectedSubnet)) {
                        qDebug() << "🎯 Module detected on subnet" << detectedSubnet << "IP:" << senderIP;
                        m_discoveredModuleIP = senderIP;
                        m_discoveredModuleSubnet = detectedSubnet;
                        m_subnetScanActive = false;
                        m_subnetScanTimer->stop();
                        
                        emit moduleSubnetDiscovered(senderIP, detectedSubnet);
                        emit subnetScanCompleted(detectedSubnet);
                        
                        // If we have pending PGN data, send it now
                        if (!m_pendingPgnData.isEmpty()) {
                            qDebug() << "📡 Sending pending PGN data to discovered subnet";
                            sendPgnToDiscoveredSubnet(m_pendingPgnData);
                            m_pendingPgnData.clear();
                        }
                        
                        // Continue processing the received data instead of returning
                        qDebug() << "🔄 Processing module response data after subnet detection";
                    }
                }
            }
            
            // Process received data
            processUdpData(datagram);
            
            // Debug output suppressed - info already in PGN processing and stats
            
            // Emit data for processing by main thread
            emit udpDataReceived(datagram);
        }
    }
}

void UDPWorker::onUdpError(QAbstractSocket::SocketError error)
{
    QString errorString = m_udpSocket->errorString();
    qDebug() << "❌ UDP socket error:" << error << errorString;
    
    emit errorOccurred("UDP socket error: " + errorString);
    
    // Try to recover
    if (m_isRunning) {
        cleanupConnection();
        
        // Attempt to rebind after a short delay
        QTimer::singleShot(2000, this, [this]() {
            if (m_isRunning) {
                bindSocket();
            }
        });
    }
}

void UDPWorker::checkConnectionStatus()
{
    if (!m_isRunning) {
        return;
    }
    
    // Check for data timeout (optional for UDP)
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 timeSinceData = currentTime - m_lastDataTime;
    
    // Update connection status based on recent activity
    bool previouslyConnected = m_isConnected;
    m_isConnected = (m_udpSocket && m_udpSocket->state() == QAbstractSocket::BoundState);
    
    if (previouslyConnected != m_isConnected) {
        emit udpStatusChanged(m_isConnected);
        qDebug() << "🔄 UDP connection status changed:" << m_isConnected;
    }
    
    // Log statistics periodically
    static int statusCounter = 0;
    if (++statusCounter % 25 == 0) { // Every 50 seconds - reduced spam
        qDebug() << "📊 UDP stats - RX:" << m_packetsReceived << "packets," 
                << QString::number(m_receiveRate, 'f', 1) << "B/s"
                << "TX:" << m_packetsSent << "packets," 
                << QString::number(m_sendRate, 'f', 1) << "B/s";
    }
}

void UDPWorker::sendHeartbeat()
{
    if (!m_isRunning) {
        return;
    }
    
    // Create heartbeat packet
    QByteArray heartbeat = buildHeartbeatPacket();
    
    // Send heartbeat
    sendToTractor(heartbeat);
    
    // Debug output (limited frequency)
    static int heartbeatCounter = 0;
    if (++heartbeatCounter % 12 == 0) { // Every minute
        // Heartbeat sent log suppressed - too verbose for normal operation
    }
}

void UDPWorker::updateReceiveRate()
{
    static qint64 lastReceiveTime = 0;
    static qint64 lastBytesReceived = 0;
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    if (lastReceiveTime > 0) {
        qint64 timeDelta = currentTime - lastReceiveTime;
        qint64 bytesDelta = m_bytesReceived - lastBytesReceived;
        
        if (timeDelta > 0) {
            m_receiveRate = (bytesDelta * 1000.0) / timeDelta; // bytes/second
        }
    }
    
    lastReceiveTime = currentTime;
    lastBytesReceived = m_bytesReceived;
}

void UDPWorker::updateSendRate()
{
    static qint64 lastSendTime = 0;
    static qint64 lastBytesSent = 0;
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    if (lastSendTime > 0) {
        qint64 timeDelta = currentTime - lastSendTime;
        qint64 bytesDelta = m_bytesSent - lastBytesSent;
        
        if (timeDelta > 0) {
            m_sendRate = (bytesDelta * 1000.0) / timeDelta; // bytes/second
        }
    }
    
    lastSendTime = currentTime;
    lastBytesSent = m_bytesSent;
}

void UDPWorker::resetStatistics()
{
    m_bytesReceived = 0;
    m_bytesSent = 0;
    m_packetsReceived = 0;
    m_packetsSent = 0;
    m_receiveRate = 0.0;
    m_sendRate = 0.0;
    m_lastDataTime = 0;
}

void UDPWorker::processUdpData(const QByteArray& data)
{
    if (data.isEmpty()) {
        return;
    }

    // Check for PGN data format (0x80 0x81 header for AgOpenGPS modules)
    if (data.size() >= 5 && static_cast<uint8_t>(data[0]) == 0x80 && static_cast<uint8_t>(data[1]) == 0x81) {

        // Enhanced checksum validation from pull request improvements
        int Length = std::max(static_cast<int>(data[4]) + 5, 5);
        if (data.size() >= Length) {
            // Calculate checksum
            char CK_A = 0;
            for (int j = 2; j < Length; j++) {
                CK_A += data[j];
            }

            // Verify checksum
            if (data[Length] != CK_A) {
                qDebug() << "❌ UDP packet checksum validation failed - discarding packet";
                return;
            }

            qDebug() << "✅ UDP packet checksum validated - processing PGN data";
        } else {
            qDebug() << "⚠️ UDP packet too short for stated length - discarding";
            return;
        }

        // Enhanced PGN processing (Phase 4.5.3)
        handlePGNProtocol(data);

        // Legacy compatibility: 0x81 specific
        if (static_cast<uint8_t>(data[1]) == 0x81) {
            processAgOpenGpsData(data);
        }
    }
    // Check for NMEA sentences from GPS via UDP
    // $G* (GPS/GNSS), $P* (Proprietary like $PANDA, $PAOGI), $K* (KSXT)
    else if (data.size() >= 2 && 
             data[0] == '$' && 
             (data[1] == 'G' || data[1] == 'P' || data[1] == 'K')) {
        
        // This is NMEA data from GPS module via UDP
        processNmeaFromUdp(data);
    }
    else {
        // Unknown data format - emit for debugging
        emit udpDataReceived(data);
    }
}

void UDPWorker::processAgOpenGpsData(const QByteArray& data)
{
    // Process AgOpenGPS PGN (Protocol Group Number) format
    // Format: 0x80, 0x81, 0x7F/module_id, PGN, length, data...
    
    if (data.size() < 5) {
        return;
    }
    
    uint8_t moduleId = static_cast<uint8_t>(data[2]);
    uint8_t pgn = static_cast<uint8_t>(data[3]);
    
    // Debug specific PGN types
    switch (pgn) {
        case 120: // GPS module scan reply
            qDebug() << "🛰️ GPS module detected via UDP scan";
            break;
            
        case 121: // IMU module
            qDebug() << "🧭 IMU module data received";
            break;
            
        case 123: // Machine module
            qDebug() << "🚜 Machine module data received";
            break;
            
        case 126: // AutoSteer module
            qDebug() << "🎯 AutoSteer module data received";
            break;
            
        case 200: // Hello from AgIO (C8)
            qDebug() << "👋 Hello received from module";
            break;
        case 201: // IP configuration
            qDebug() << "⚙️ IP config received from module";
            break;
        case 202: // Module scan
            qDebug() << "🔍 Scan message received (shouldn't happen - we send these)";
            break;
        case 203: // Scan reply - CRITICAL for module detection!
            qDebug() << "🎯 SCAN REPLY received from module ID:" << QString::number(moduleId, 16);
            break;
            
        case 214: // GPS PGN 0xD6 - CRITICAL GPS binary data from hardware modules
            qDebug() << "📍 GPS binary PGN 0xD6 received from module ID:" << QString::number(moduleId, 16);
            processGpsPgn(data);
            break;
            
        default:
            // Forward all PGN data to main thread
            break;
    }
    
    // Emit for processing by AgIOService
    emit udpDataReceived(data);
}

void UDPWorker::processNmeaFromUdp(const QByteArray& data)
{
    // Convert to string and add to NMEA buffer
    QString nmea = QString::fromLatin1(data);
    m_nmeaBuffer += nmea;
    
    // Process complete sentences (ending with \r\n or \n)
    while (m_nmeaBuffer.contains("\r\n") || m_nmeaBuffer.contains("\n")) {
        int endPos = m_nmeaBuffer.indexOf("\r\n");
        if (endPos == -1) {
            endPos = m_nmeaBuffer.indexOf("\n");
        }
        
        if (endPos > 0) {
            QString sentence = m_nmeaBuffer.left(endPos);
            m_nmeaBuffer = m_nmeaBuffer.mid(endPos + 1);
            
            // Skip empty lines
            if (sentence.trimmed().isEmpty()) continue;
            
            // Verify it's a valid NMEA sentence
            if (sentence.startsWith("$")) {
                // Log common GPS sentences
                if (sentence.startsWith("$GPGGA") || sentence.startsWith("$GNGGA")) {
                    static int ggaCounter = 0;
                    if (++ggaCounter % 10 == 0) { // Every 10 GGA sentences
                        qDebug() << "🛰️ GPS GGA via UDP:" << sentence.left(50);
                    }
                }
                else if (sentence.startsWith("$GPVTG") || sentence.startsWith("$GNVTG")) {
                    static int vtgCounter = 0;
                    if (++vtgCounter % 30 == 0) { // Every 30 VTG sentences
                        qDebug() << "🧭 GPS VTG via UDP:" << sentence.left(30);
                    }
                }
                
                // Emit NMEA for processing by GPSWorker/AgIOService
                emit nmeaReceived(sentence);
            }
        }
        else {
            break;
        }
    }
    
    // Prevent buffer overflow
    if (m_nmeaBuffer.length() > 2048) {
        qDebug() << "⚠️ NMEA buffer overflow, clearing";
        m_nmeaBuffer.clear();
    }
}

void UDPWorker::sendModuleData(const QByteArray& moduleData)
{
    // Send module-specific data to AgOpenGPS
    sendToTractor(moduleData);
}

QByteArray UDPWorker::buildHeartbeatPacket() const
{
    // Build a simple heartbeat packet
    QByteArray heartbeat;
    heartbeat.resize(8);
    
    // Header
    heartbeat[0] = 0xAA; // Start byte
    heartbeat[1] = 0x55; // Start byte
    
    // Packet type (heartbeat)
    heartbeat[2] = 0xFF;
    
    // Data length
    heartbeat[3] = 0x02;
    
    // Heartbeat data
    heartbeat[4] = 0x01; // AgIO identifier
    heartbeat[5] = 0x00; // Status
    
    // Simple checksum
    quint8 checksum = 0;
    for (int i = 2; i < 6; ++i) {
        checksum ^= static_cast<quint8>(heartbeat[i]);
    }
    heartbeat[6] = static_cast<char>(checksum);
    
    // End byte
    heartbeat[7] = 0xCC;
    
    return heartbeat;
}

bool UDPWorker::bindSocket()
{
    if (!m_udpSocket) {
        return false;
    }
    
    // Check if already bound
    if (m_udpSocket->state() == QAbstractSocket::BoundState) {
        qDebug() << "⚠️ UDP socket already bound to port" << m_udpSocket->localPort();
        return true;
    }
    
    // IMPORTANT: Always bind to Any address to receive from all interfaces
    // The m_tractorAddress is only used for SENDING broadcasts
    QHostAddress listenAddress = QHostAddress::Any;
    
    // UDPWorker will listen for GPS on port 9999
    // FormLoop listens on port 15550 for AgIO app data
    qDebug() << "📡 UDPWorker will listen on port" << m_listenPort << "for GPS data";
    
    // Try to bind to listen port
    if (m_udpSocket->bind(listenAddress, m_listenPort)) {
        qDebug() << "✅ UDP socket bound to port" << m_listenPort;
        qDebug() << "  Architecture:";
        qDebug() << "    - AgIOService/UDPWorker: GPS reception on port" << m_listenPort;
        qDebug() << "    - FormLoop: AgIO app data on port 15550";
        qDebug() << "    - Broadcasting to:" << m_tractorAddress.toString() << ":" << m_tractorPort;
        return true;
    } else {
        qDebug() << "❌ Failed to bind UDP socket to port" << m_listenPort 
                << ":" << m_udpSocket->errorString();
        
        // Try alternative ports - Phase 4.5: FormLoop eliminated, 9999 allowed
        QList<int> alternativePorts = {9999, 10001, 10002, 10003, 7777, 6666};
        for (int port : alternativePorts) {
            // Phase 4.5: FormLoop eliminated, AgIOService owns port 9999
            
            if (m_udpSocket->bind(listenAddress, port)) {
                qDebug() << "✅ UDP socket bound to alternative port" << port;
                qDebug() << "  Phase 4.5: AgIOService owns port" << port;
                m_listenPort = port; // Update listen port
                return true;
            }
            qDebug() << "⚠️ Port" << port << "failed:" << m_udpSocket->errorString();
        }
        
        return false;
    }
}

void UDPWorker::cleanupConnection()
{
    if (m_udpSocket) {
        if (m_udpSocket->state() != QAbstractSocket::UnconnectedState) {
            m_udpSocket->close();
        }
    }
}

// ========== Advanced UDP Protocols (Phase 4.5.3) ==========

void UDPWorker::startModuleDiscovery()
{
    if (m_discoveryActive) {
        qDebug() << "⚠️ Module discovery already active";
        return;
    }
    
    qDebug() << "🔍 Starting module discovery";
    m_discoveryActive = true;
    m_discoveredModules.clear();
    m_moduleLastSeen.clear();
    
    // Start discovery timer
    m_discoveryTimer->start();
    m_timeoutTimer->start();
    
    // Immediate first broadcast
    broadcastHello();
}

void UDPWorker::stopModuleDiscovery()
{
    qDebug() << "🛑 Stopping module discovery";
    m_discoveryActive = false;
    m_discoveryTimer->stop();
    m_timeoutTimer->stop();
    
    emit networkScanCompleted(m_discoveredModules);
}

void UDPWorker::scanSubnet(const QString& baseIP)
{
    m_currentSubnet = baseIP;
    qDebug() << "🔍 Scanning subnet:" << baseIP;
    
    // Extract base IP (e.g., "192.168.1" from "192.168.1.100")
    QStringList ipParts = baseIP.split('.');
    if (ipParts.size() >= 3) {
        QString subnet = QString("%1.%2.%3").arg(ipParts[0], ipParts[1], ipParts[2]);
        
        // Ping common AgOpenGPS module IPs in this subnet
        QStringList commonIPs = {
            subnet + ".100",  // Common module IP
            subnet + ".101",  // Secondary module
            subnet + ".102",  // Tertiary module
            subnet + ".200",  // Alternative range
            subnet + ".201"
        };
        
        for (const QString& ip : commonIPs) {
            pingModule(ip);
        }
    }
}

void UDPWorker::pingModule(const QString& moduleIP)
{
    qDebug() << "🏓 Pinging module:" << moduleIP;
    
    // Send hello message to specific module IP
    sendHelloMessage(moduleIP);
}

void UDPWorker::wakeUpModules(const QString& broadcastIP)
{
    qDebug() << "🔊 Waking up dormant modules with broadcast to:" << broadcastIP;
    
    if (!m_udpSocket) {
        qDebug() << "⚠️ UDP socket not available for wakeup broadcast";
        return;
    }
    
    // PGN 202 Scan Message (like AgIO FormUDP.cs line 135) - WORKING format!
    QByteArray helloMessage;
    helloMessage.append(char(0x80)); // PGN header 1
    helloMessage.append(char(0x81)); // PGN header 2  
    helloMessage.append(char(0x7F)); // Source AgIO
    helloMessage.append(char(202));  // PGN 202 scan (0xCA = 202 decimal) - MATCHES working test!
    helloMessage.append(char(3));    // Length = 3 bytes payload
    helloMessage.append(char(202));  // Payload 1 (PGN repeat)
    helloMessage.append(char(202));  // Payload 2 (PGN repeat)  
    helloMessage.append(char(5));    // Payload 3 (like AgIO FormUDP.cs)
    helloMessage.append(char(0x47)); // Checksum (0x47 from working script)
    
    // Send wakeup broadcast via all interfaces for cross-subnet capability (like AgIO C#)
    qDebug() << "📡 Sending wakeup broadcast to" << broadcastIP << "via all interfaces";
    sendToAllInterfaces(helloMessage, broadcastIP, 8888);
    
    // Wait briefly for immediate response (dormant server responds quickly)
    QTimer::singleShot(500, this, [this]() {
        qDebug() << "✅ Wakeup sent - modules should respond on next UDP cycle";
    });
}

void UDPWorker::broadcastHello()
{
    if (!m_discoveryActive || !m_udpSocket) {
        return;
    }
    
    // Create PGN 202 Scan message (like AgIO FormUDP.cs line 135) - WORKING format!
    QByteArray helloMessage;
    helloMessage.append(char(0x80)); // PGN header 1
    helloMessage.append(char(0x81)); // PGN header 2  
    helloMessage.append(char(0x7F)); // Source AgIO
    helloMessage.append(char(202));  // PGN 202 scan (0xCA = 202 decimal) - MATCHES working test!
    helloMessage.append(char(3));    // Length = 3 bytes payload
    helloMessage.append(char(202));  // Payload 1 (PGN repeat)
    helloMessage.append(char(202));  // Payload 2 (PGN repeat)  
    helloMessage.append(char(5));    // Payload 3 (like AgIO FormUDP.cs)
    helloMessage.append(char(0x47)); // Checksum (0x47 from working script)
    
    // Broadcast to subnet
    QHostAddress broadcastAddr("255.255.255.255");
    qint64 sent = m_udpSocket->writeDatagram(helloMessage, broadcastAddr, 8888);
    
    if (sent > 0) {
        qDebug() << "📡 Broadcast hello sent (" << sent << "bytes)";
    } else {
        qDebug() << "⚠️ Broadcast hello failed:" << m_udpSocket->errorString();
    }
}

void UDPWorker::sendHelloMessage(const QString& moduleIP)
{
    if (!m_udpSocket) return;
    
    // Create targeted hello message
    QByteArray helloMessage;
    helloMessage.append(char(0x80));  // PGN header
    helloMessage.append(char(0x81));  // AgIO hello
    helloMessage.append(char(0x7F));  // Source address
    helloMessage.append("HELLO_AgIO"); // Identifier
    helloMessage.append(QDateTime::currentDateTime().toString().toUtf8());
    
    QHostAddress moduleAddr(moduleIP);
    qint64 sent = m_udpSocket->writeDatagram(helloMessage, moduleAddr, 8888);
    
    if (sent > 0) {
        qDebug() << "📧 Hello sent to" << moduleIP << "(" << sent << "bytes)";
    }
}

void UDPWorker::processModuleResponse(const QByteArray& data)
{
    if (data.size() < 4) {
        return; // Too short for valid response
    }
    
    // Check if it's a module response
    if (data[0] == char(0x80) && data[1] == char(0x82)) {
        // Extract module info from response
        QString moduleType = "Unknown";
        if (data.contains("AutoSteer")) {
            moduleType = "AutoSteer";
        } else if (data.contains("GPS")) {
            moduleType = "GPS";  
        } else if (data.contains("IMU")) {
            moduleType = "IMU";
        } else if (data.contains("Section")) {
            moduleType = "Section";
        }
        
        // Extract sender IP from UDP packet (this is simplified)
        // In real implementation, we'd get this from QUdpSocket sender info
        QString senderIP = "192.168.1.100"; // Placeholder
        
        // Record module discovery
        if (!m_discoveredModules.contains(senderIP)) {
            m_discoveredModules.append(senderIP);
            m_moduleLastSeen[senderIP] = QDateTime::currentDateTime();
            
            qDebug() << "🎯 Module discovered:" << senderIP << "Type:" << moduleType;
            emit moduleDiscovered(senderIP, moduleType);
        } else {
            // Update last seen time
            m_moduleLastSeen[senderIP] = QDateTime::currentDateTime();
        }
    }
}

void UDPWorker::handlePGNProtocol(const QByteArray& data)
{
    if (data.size() < 3) {
        return; // Too short for PGN
    }
    
    // Check PGN header
    if (data[0] == char(0x80)) {
        quint8 pgnType = static_cast<quint8>(data[1]);
        quint8 sourceAddr = static_cast<quint8>(data[2]);
        
        qDebug() << "📦 PGN received - Type:" << QString::number(pgnType, 16) 
                 << "Source:" << QString::number(sourceAddr, 16);
        
        switch (pgnType) {
            case 0x81: // Hello message
                qDebug() << "👋 Hello message from" << QString::number(sourceAddr, 16);
                break;
                
            case 0x82: // Module response
                processModuleResponse(data);
                break;
                
            case 0x83: // GPS data
                qDebug() << "📍 GPS data from module" << QString::number(sourceAddr, 16);
                emit pgnDataReceived(data);
                break;
                
            case 0x84: // IMU data
                qDebug() << "📐 IMU data from module" << QString::number(sourceAddr, 16);
                emit pgnDataReceived(data);
                break;
                
            case 0x85: // Section control
                qDebug() << "🚜 Section data from module" << QString::number(sourceAddr, 16);
                emit pgnDataReceived(data);
                break;
                
            default:
                qDebug() << "❓ Unknown PGN type:" << QString::number(pgnType, 16);
                emit pgnDataReceived(data);
                break;
        }
    }
}

void UDPWorker::checkModuleTimeouts()
{
    if (!m_discoveryActive) {
        return;
    }
    
    QDateTime now = QDateTime::currentDateTime();
    QStringList timedOutModules;
    
    for (auto it = m_moduleLastSeen.begin(); it != m_moduleLastSeen.end(); ++it) {
        qint64 msecsElapsed = it.value().msecsTo(now);
        
        if (msecsElapsed > 30000) { // 30 second timeout
            timedOutModules.append(it.key());
            qDebug() << "⏰ Module timeout:" << it.key();
            emit moduleTimeout(it.key());
        }
    }
    
    // Remove timed out modules
    for (const QString& module : timedOutModules) {
        m_moduleLastSeen.remove(module);
        m_discoveredModules.removeAll(module);
    }
}

void UDPWorker::validateModuleConnection()
{
    // Validate connections to discovered modules
    for (const QString& moduleIP : m_discoveredModules) {
        // Send ping to verify module is still responsive
        sendHelloMessage(moduleIP);
    }
}

void UDPWorker::manageModuleTimeouts()
{
    // This method can be used for advanced timeout management
    // For now, checkModuleTimeouts handles the basic functionality
    checkModuleTimeouts();
}

// ========== Multi-Subnet Discovery Implementation ==========

void UDPWorker::scanAllSubnets()
{
    if (!m_udpSocket || !m_isRunning) {
        qDebug() << "🚨 Cannot scan subnets - UDP not running";
        return;
    }

    if (!m_subnetScanActive) {
        // Start new scan
        qDebug() << "🔍 Starting multi-subnet discovery scan...";
        m_subnetScanActive = true;
        m_currentSubnetIndex = 0;
        m_discoveredModuleIP.clear();
        m_discoveredModuleSubnet.clear();
    }

    if (m_currentSubnetIndex >= m_commonSubnets.size()) {
        // Scan completed - no module found
        qDebug() << "❌ Multi-subnet scan completed - no module found";
        m_subnetScanActive = false;
        emit subnetScanCompleted("");  // Empty string indicates no module found
        return;
    }

    QString subnet = m_commonSubnets[m_currentSubnetIndex];
    QString broadcastIP = subnet + ".126"; // Test direct module IP au lieu de broadcast
    
    qDebug() << "🔍 Scanning subnet" << subnet << "(" << (m_currentSubnetIndex + 1) << "/" << m_commonSubnets.size() << ")";

    // Build PGN 202 Scan message (like AgIO FormUDP.cs line 135) - WORKING format!
    QByteArray helloMessage;
    helloMessage.append(char(0x80)); // PGN header 1
    helloMessage.append(char(0x81)); // PGN header 2  
    helloMessage.append(char(0x7F)); // Source AgIO
    helloMessage.append(char(202));  // PGN 202 scan (0xCA = 202 decimal) - MATCHES working test!
    helloMessage.append(char(3));    // Length = 3 bytes payload
    helloMessage.append(char(202));  // Payload 1 (PGN repeat)
    helloMessage.append(char(202));  // Payload 2 (PGN repeat)  
    helloMessage.append(char(5));    // Payload 3 (like AgIO FormUDP.cs)
    helloMessage.append(char(0x47)); // Checksum (0x47 from working script)

    // Send scan broadcast to current subnet via all interfaces (like AgIO C#)
    qDebug() << "📡 Sending Hello to subnet" << subnet << "via all interfaces";
    sendToAllInterfaces(helloMessage, broadcastIP, 8888);
    
    // Wait for response (2 seconds timeout)
    m_subnetScanTimer->start();
}

void UDPWorker::sendPgnToDiscoveredSubnet(const QByteArray& pgnData)
{
    if (!m_udpSocket || !m_isRunning) {
        qDebug() << "🚨 Cannot send PGN - UDP not running";
        return;
    }

    if (m_discoveredModuleSubnet.isEmpty()) {
        qDebug() << "🔍 No subnet discovered yet - storing PGN data and starting scan...";
        m_pendingPgnData = pgnData;
        scanAllSubnets();
        return;
    }

    // Send PGN data to discovered subnet using multi-interface broadcasting
    QString broadcastIP = m_discoveredModuleSubnet + ".255";
    sendToAllInterfaces(pgnData, broadcastIP, 8888);
    
    qDebug() << "📡 PGN data sent to discovered subnet" << m_discoveredModuleSubnet << "via all interfaces";
    m_packetsSent++;
    m_bytesSent += pgnData.size();
    updateSendRate();
}

// ========== Multi-Interface Broadcasting (AgIO C# Style) ==========

void UDPWorker::sendToAllInterfaces(const QByteArray& data, const QString& targetIP, int port)
{
    if (data.isEmpty()) {
        qDebug() << "🚨 Cannot send empty data";
        return;
    }

    qDebug() << "🌐 Sending to all network interfaces - Target:" << targetIP << "Port:" << port << "Size:" << data.size() << "bytes";

    int interfaceCount = 0;
    int successCount = 0;

    // Get all network interfaces (avoiding 'interface' macro conflict)
    const QList<QNetworkInterface> networkInterfaces = QNetworkInterface::allInterfaces();
    
    for (const QNetworkInterface& netIface : networkInterfaces) {
        
        // Only process IPv4 interfaces that are up and running
        if (netIface.flags().testFlag(QNetworkInterface::IsUp) &&
            netIface.flags().testFlag(QNetworkInterface::IsRunning) &&
            !netIface.flags().testFlag(QNetworkInterface::IsLoopBack)) {

            // Get all IP addresses for this interface
            const QList<QNetworkAddressEntry> addresses = netIface.addressEntries();
            
            for (const QNetworkAddressEntry& addressEntry : addresses) {
                
                if (addressEntry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    
                    QHostAddress interfaceIP = addressEntry.ip();
                    interfaceCount++;

                    qDebug() << "📡 Trying interface:" << netIface.name() 
                             << "IP:" << interfaceIP.toString()
                             << "Netmask:" << addressEntry.netmask().toString();

                    // Create a dedicated socket for this interface (like AgIO C#)
                    QUdpSocket* interfaceSocket = new QUdpSocket(this);
                    
                    // Bind to this specific interface (like AgIO C# line 247)
                    if (interfaceSocket->bind(interfaceIP, 0)) { // Port 0 = auto-assign
                        
                        // Enable broadcast using Windows/Unix compatible method
                        int sockfd = interfaceSocket->socketDescriptor();
                        if (sockfd != -1) {
                            int broadcast = 1;
#ifdef Q_OS_WIN
                            setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast));
#else
                            setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
#endif
                        }
                        
                        // Send data from this interface
                        QHostAddress target(targetIP);
                        qint64 sent = interfaceSocket->writeDatagram(data, target, port);
                        
                        if (sent > 0) {
                            qDebug() << "✅ Sent" << sent << "bytes from interface" << interfaceIP.toString();
                            successCount++;
                            
                            // Update statistics
                            m_packetsSent++;
                            m_bytesSent += sent;
                        } else {
                            qDebug() << "❌ Send failed from interface" << interfaceIP.toString() << ":" << interfaceSocket->errorString();
                        }
                    } else {
                        qDebug() << "❌ Bind failed on interface" << interfaceIP.toString() << ":" << interfaceSocket->errorString();
                    }

                    // Clean up socket
                    interfaceSocket->deleteLater();
                }
            }
        }
    }

    qDebug() << "🌐 Multi-interface send completed:" << successCount << "/" << interfaceCount << "interfaces succeeded";
    
    if (successCount > 0) {
        updateSendRate();
    } else {
        emit errorOccurred(QString("Failed to send on all %1 interfaces").arg(interfaceCount));
    }
}

// Phase 4.6: GPS binary PGN processing (replaces FormGPS UDP 0xD6)
void UDPWorker::processGpsPgn(const QByteArray& data)
{
    if (data.size() < 57) {
        qDebug() << "⚠️ GPS PGN 0xD6 data too short:" << data.size() << "bytes (expected 57)";
        return;
    }
    
    // PGN 0xD6 GPS binary format (from QtAgIO FormLoop_nmea.cpp)
    // Bytes 0-4:   Header (0x80, 0x81, 0x7C, 0xD6, 0x33)
    // Bytes 5-12:  Longitude (double)
    // Bytes 13-20: Latitude (double)
    // Bytes 21-24: Heading dual antenna (float)
    // Bytes 25-28: Heading single antenna (float)
    // Bytes 29-32: Speed in km/h (float)
    // Bytes 33-36: Roll (float)
    // Bytes 37-40: Altitude (float)
    // Bytes 41-42: Satellites tracked (ushort)
    // Byte 43:     Fix quality (byte)
    // Bytes 44-45: HDOP x100 (ushort)
    // Bytes 46-47: Age x100 (ushort)
    // Bytes 48-49: IMU heading (ushort)
    // Bytes 50-51: IMU roll (short)
    // Bytes 52-53: IMU pitch (short)
    
    // Extract GPS data
    double longitude, latitude;
    float headingDual, headingSingle, speed, roll, altitude;
    quint16 satellites, hdop, age, imuHeading;
    quint8 fixQuality;
    qint16 imuRoll, imuPitch;
    
    // Convert binary data to values
    std::memcpy(&longitude, data.constData() + 5, 8);
    std::memcpy(&latitude, data.constData() + 13, 8);
    std::memcpy(&headingDual, data.constData() + 21, 4);
    std::memcpy(&headingSingle, data.constData() + 25, 4);
    std::memcpy(&speed, data.constData() + 29, 4);
    std::memcpy(&roll, data.constData() + 33, 4);
    std::memcpy(&altitude, data.constData() + 37, 4);
    std::memcpy(&satellites, data.constData() + 41, 2);
    fixQuality = data[43];
    std::memcpy(&hdop, data.constData() + 44, 2);
    std::memcpy(&age, data.constData() + 46, 2);
    std::memcpy(&imuHeading, data.constData() + 48, 2);
    std::memcpy(&imuRoll, data.constData() + 50, 2);
    std::memcpy(&imuPitch, data.constData() + 52, 2);
    
    // Create NMEA-like format string for AgIOService processing
    QString nmeaString = QString("$PANDA,%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13*%14")
        .arg(QTime::currentTime().toString("hhmmss.zzz"))  // Time
        .arg(latitude, 0, 'f', 8)                          // Latitude
        .arg(latitude >= 0 ? 'N' : 'S')                    // Lat hemisphere
        .arg(longitude, 0, 'f', 8)                         // Longitude
        .arg(longitude >= 0 ? 'E' : 'W')                   // Lon hemisphere
        .arg(fixQuality)                                   // Fix quality
        .arg(satellites)                                   // Satellites
        .arg(hdop / 100.0, 0, 'f', 1)                     // HDOP
        .arg(altitude, 0, 'f', 1)                          // Altitude
        .arg(age / 100.0, 0, 'f', 1)                      // Age
        .arg(headingSingle, 0, 'f', 1)                     // Heading
        .arg(roll, 0, 'f', 1)                             // Roll IMU
        .arg(imuPitch / 100.0, 0, 'f', 1)                 // Pitch IMU
        .arg("XX");                                        // Checksum placeholder
    
    qDebug() << "📍 GPS PGN 0xD6 processed:" 
             << "Lat:" << latitude 
             << "Lon:" << longitude 
             << "Heading:" << headingSingle
             << "Fix:" << fixQuality 
             << "Sats:" << satellites;
    
    // Emit as NMEA for AgIOService GPS processing
    emit nmeaReceived(nmeaString);
    
    // Also emit raw PGN data for any other processing needs
    emit pgnDataReceived(data);
}