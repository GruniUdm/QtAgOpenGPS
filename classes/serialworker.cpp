// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// SerialWorker implementation - Multi-port serial communication

#include "serialworker.h"
#include <QCoreApplication>
#include <QThread>

SerialWorker::SerialWorker(QObject *parent)
    : QObject(parent)
    , m_gpsPort(nullptr)
    , m_imuPort(nullptr)
    , m_autosteerPort(nullptr)
    , m_gpsConnected(false)
    , m_imuConnected(false)
    , m_autosteerConnected(false)
    , m_gpsBaudRate(9600)
    , m_imuBaudRate(9600)
    , m_autosteerBaudRate(9600)
    , m_connectionTimer(nullptr)
    , m_isRunning(false)
{
    qDebug() << "🔧 SerialWorker constructor - Thread:" << QThread::currentThread();
    
    // Initialize connection monitoring timer
    m_connectionTimer = new QTimer(this);
    m_connectionTimer->setSingleShot(false);
    m_connectionTimer->setInterval(5000); // Check connections every 5 seconds
    connect(m_connectionTimer, &QTimer::timeout, this, &SerialWorker::checkConnections);
}

SerialWorker::~SerialWorker()
{
    qDebug() << "🔧 SerialWorker destructor";
    stopWorker();
    closeAllPorts();
}

void SerialWorker::startWorker()
{
    qDebug() << "🚀 Starting SerialWorker - Thread:" << QThread::currentThread();
    
    QMutexLocker locker(&m_mutex);
    
    if (m_isRunning) {
        qWarning() << "SerialWorker already running";
        return;
    }
    
    m_isRunning = true;
    m_connectionTimer->start();
    
    emit workerStarted();
    qDebug() << "✅ SerialWorker started successfully";
}

void SerialWorker::stopWorker()
{
    qDebug() << "🛑 Stopping SerialWorker";
    
    QMutexLocker locker(&m_mutex);
    
    if (!m_isRunning) {
        return;
    }
    
    m_isRunning = false;
    m_connectionTimer->stop();
    closeAllPorts();
    
    emit workerStopped();
    qDebug() << "✅ SerialWorker stopped successfully";
}

// Port Management Methods
bool SerialWorker::openGPSPort(const QString& portName, int baudRate)
{
    qDebug() << "📡 Opening GPS port:" << portName << "at" << baudRate << "baud";
    
    if (openPort(m_gpsPort, portName, baudRate, "GPS")) {
        m_gpsPortName = portName;
        m_gpsBaudRate = baudRate;
        m_gpsConnected = true;
        
        // Connect data reception
        connect(m_gpsPort, &QSerialPort::readyRead, this, &SerialWorker::onGPSDataReady);
        connect(m_gpsPort, QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::errorOccurred),
                this, &SerialWorker::onGPSError);
        
        emit gpsConnected(true);
        qDebug() << "✅ GPS port opened successfully:" << portName;
        return true;
    }
    
    return false;
}

bool SerialWorker::openIMUPort(const QString& portName, int baudRate)
{
    qDebug() << "🧭 Opening IMU port:" << portName << "at" << baudRate << "baud";
    
    if (openPort(m_imuPort, portName, baudRate, "IMU")) {
        m_imuPortName = portName;
        m_imuBaudRate = baudRate;
        m_imuConnected = true;
        
        // Connect data reception
        connect(m_imuPort, &QSerialPort::readyRead, this, &SerialWorker::onIMUDataReady);
        connect(m_imuPort, QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::errorOccurred),
                this, &SerialWorker::onIMUError);
        
        emit imuConnected(true);
        qDebug() << "✅ IMU port opened successfully:" << portName;
        return true;
    }
    
    return false;
}

bool SerialWorker::openAutoSteerPort(const QString& portName, int baudRate)
{
    qDebug() << "🎛️ Opening AutoSteer port:" << portName << "at" << baudRate << "baud";
    
    if (openPort(m_autosteerPort, portName, baudRate, "AutoSteer")) {
        m_autosteerPortName = portName;
        m_autosteerBaudRate = baudRate;
        m_autosteerConnected = true;
        
        // Connect data reception
        connect(m_autosteerPort, &QSerialPort::readyRead, this, &SerialWorker::onAutoSteerDataReady);
        connect(m_autosteerPort, QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::errorOccurred),
                this, &SerialWorker::onAutoSteerError);
        
        emit autosteerConnected(true);
        qDebug() << "✅ AutoSteer port opened successfully:" << portName;
        return true;
    }
    
    return false;
}

void SerialWorker::closeGPSPort()
{
    closePort(m_gpsPort, "GPS");
    m_gpsConnected = false;
    m_gpsPortName.clear();
    emit gpsConnected(false);
}

void SerialWorker::closeIMUPort()
{
    closePort(m_imuPort, "IMU");
    m_imuConnected = false;
    m_imuPortName.clear();
    emit imuConnected(false);
}

void SerialWorker::closeAutoSteerPort()
{
    closePort(m_autosteerPort, "AutoSteer");
    m_autosteerConnected = false;
    m_autosteerPortName.clear();
    emit autosteerConnected(false);
}

void SerialWorker::closeAllPorts()
{
    qDebug() << "🔌 Closing all serial ports";
    closeGPSPort();
    closeIMUPort();
    closeAutoSteerPort();
}

QStringList SerialWorker::scanAvailablePorts()
{
    QStringList availablePorts;
    
    const auto serialPorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : serialPorts) {
        availablePorts << portInfo.portName();
    }
    
    qDebug() << "🔍 Available serial ports:" << availablePorts;
    return availablePorts;
}

bool SerialWorker::isPortAvailable(const QString& portName)
{
    const auto serialPorts = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &portInfo : serialPorts) {
        if (portInfo.portName() == portName) {
            return true;
        }
    }
    return false;
}

// Data Transmission Methods
void SerialWorker::writeToGPS(const QByteArray& data)
{
    if (m_gpsPort && m_gpsPort->isOpen()) {
        qint64 bytesWritten = m_gpsPort->write(data);
        if (bytesWritten == -1) {
            qWarning() << "Failed to write to GPS port:" << m_gpsPort->errorString();
            emit serialError("GPS", m_gpsPort->errorString());
        } else {
            qDebug() << "📤 GPS data sent:" << bytesWritten << "bytes";
        }
    } else {
        qWarning() << "GPS port not open for writing";
        emit serialError("GPS", "Port not open");
    }
}

void SerialWorker::writeToIMU(const QByteArray& data)
{
    if (m_imuPort && m_imuPort->isOpen()) {
        qint64 bytesWritten = m_imuPort->write(data);
        if (bytesWritten == -1) {
            qWarning() << "Failed to write to IMU port:" << m_imuPort->errorString();
            emit serialError("IMU", m_imuPort->errorString());
        } else {
            qDebug() << "📤 IMU data sent:" << bytesWritten << "bytes";
        }
    } else {
        qWarning() << "IMU port not open for writing";
        emit serialError("IMU", "Port not open");
    }
}

void SerialWorker::writeToAutoSteer(const QByteArray& data)
{
    if (m_autosteerPort && m_autosteerPort->isOpen()) {
        qint64 bytesWritten = m_autosteerPort->write(data);
        if (bytesWritten == -1) {
            qWarning() << "Failed to write to AutoSteer port:" << m_autosteerPort->errorString();
            emit serialError("AutoSteer", m_autosteerPort->errorString());
        } else {
            qDebug() << "📤 AutoSteer data sent:" << bytesWritten << "bytes";
        }
    } else {
        qWarning() << "AutoSteer port not open for writing";
        emit serialError("AutoSteer", "Port not open");
    }
}

// Configuration Methods
void SerialWorker::configureGPSPort(const QString& portName, int baudRate)
{
    if (m_gpsConnected) {
        closeGPSPort();
    }
    openGPSPort(portName, baudRate);
}

void SerialWorker::configureIMUPort(const QString& portName, int baudRate)
{
    if (m_imuConnected) {
        closeIMUPort();
    }
    openIMUPort(portName, baudRate);
}

void SerialWorker::configureAutoSteerPort(const QString& portName, int baudRate)
{
    if (m_autosteerConnected) {
        closeAutoSteerPort();
    }
    openAutoSteerPort(portName, baudRate);
}

// Data Reception Handlers
void SerialWorker::onGPSDataReady()
{
    if (!m_gpsPort) return;
    
    QByteArray data = m_gpsPort->readAll();
    m_gpsBuffer.append(data);
    
    emit serialDataReceived("GPS", data);
    processGPSBuffer();
}

void SerialWorker::onIMUDataReady()
{
    if (!m_imuPort) return;
    
    QByteArray data = m_imuPort->readAll();
    m_imuBuffer.append(data);
    
    emit serialDataReceived("IMU", data);
    processIMUBuffer();
}

void SerialWorker::onAutoSteerDataReady()
{
    if (!m_autosteerPort) return;
    
    QByteArray data = m_autosteerPort->readAll();
    m_autosteerBuffer.append(data);
    
    emit serialDataReceived("AutoSteer", data);
    processAutoSteerBuffer();
}

// Error Handlers
void SerialWorker::onGPSError(QSerialPort::SerialPortError error)
{
    handleSerialError(m_gpsPort, "GPS", error);
}

void SerialWorker::onIMUError(QSerialPort::SerialPortError error)
{
    handleSerialError(m_imuPort, "IMU", error);
}

void SerialWorker::onAutoSteerError(QSerialPort::SerialPortError error)
{
    handleSerialError(m_autosteerPort, "AutoSteer", error);
}

void SerialWorker::checkConnections()
{
    // Check if ports are still connected and functional
    if (m_gpsPort && !m_gpsPort->isOpen()) {
        m_gpsConnected = false;
        emit gpsConnected(false);
    }
    
    if (m_imuPort && !m_imuPort->isOpen()) {
        m_imuConnected = false;
        emit imuConnected(false);
    }
    
    if (m_autosteerPort && !m_autosteerPort->isOpen()) {
        m_autosteerConnected = false;
        emit autosteerConnected(false);
    }
}

// Helper Methods
bool SerialWorker::openPort(QSerialPort*& port, const QString& portName, int baudRate, const QString& description)
{
    // Close existing port if open
    if (port && port->isOpen()) {
        port->close();
        delete port;
        port = nullptr;
    }
    
    // Create new port
    port = new QSerialPort(this);
    port->setPortName(portName);
    port->setBaudRate(baudRate);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);
    
    if (port->open(QIODevice::ReadWrite)) {
        qDebug() << "✅" << description << "port opened:" << portName << "at" << baudRate << "baud";
        return true;
    } else {
        QString errorStr = port->errorString();
        qWarning() << "❌ Failed to open" << description << "port:" << portName << "Error:" << errorStr;
        emit portOpenError(portName, errorStr);
        
        delete port;
        port = nullptr;
        return false;
    }
}

void SerialWorker::closePort(QSerialPort*& port, const QString& description)
{
    if (port) {
        if (port->isOpen()) {
            port->close();
            qDebug() << "🔌" << description << "port closed";
        }
        delete port;
        port = nullptr;
    }
}

void SerialWorker::processGPSBuffer()
{
    // Extract complete NMEA sentences from buffer
    QStringList sentences = extractNMEASentences(m_gpsBuffer);
    
    for (const QString& sentence : sentences) {
        if (isValidNMEASentence(sentence)) {
            emit gpsDataReceived(sentence);
        } else {
            qDebug() << "⚠️ Invalid NMEA sentence:" << sentence;
        }
    }
}

void SerialWorker::processIMUBuffer()
{
    // For IMU, we typically process binary data or specific protocols
    // This is a simplified version - actual implementation depends on IMU protocol
    
    if (m_imuBuffer.size() >= 32) { // Assuming 32-byte IMU packets
        QByteArray packet = m_imuBuffer.left(32);
        m_imuBuffer.remove(0, 32);
        
        emit imuDataReceived(packet);
    }
}

void SerialWorker::processAutoSteerBuffer()
{
    // Process AutoSteer responses - typically status or acknowledgment messages
    
    while (m_autosteerBuffer.contains('\n')) {
        int index = m_autosteerBuffer.indexOf('\n');
        QByteArray line = m_autosteerBuffer.left(index);
        m_autosteerBuffer.remove(0, index + 1);
        
        if (!line.isEmpty()) {
            emit autosteerResponseReceived(line);
        }
    }
}

void SerialWorker::handleSerialError(QSerialPort* port, const QString& portName, QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
        return;
    }
    
    QString errorString;
    switch (error) {
        case QSerialPort::DeviceNotFoundError:
            errorString = "Device not found";
            break;
        case QSerialPort::PermissionError:
            errorString = "Permission denied";
            break;
        case QSerialPort::OpenError:
            errorString = "Unable to open port";
            break;
        case QSerialPort::WriteError:
            errorString = "Write error";
            break;
        case QSerialPort::ReadError:
            errorString = "Read error";
            break;
        case QSerialPort::ResourceError:
            errorString = "Resource error (device disconnected)";
            break;
        case QSerialPort::UnsupportedOperationError:
            errorString = "Unsupported operation";
            break;
        case QSerialPort::TimeoutError:
            errorString = "Timeout error";
            break;
        default:
            errorString = "Unknown error";
            break;
    }
    
    qWarning() << "⚠️ Serial error on" << portName << "port:" << errorString;
    emit serialError(portName, errorString);
    
    // Handle critical errors by closing the port
    if (error == QSerialPort::ResourceError || error == QSerialPort::DeviceNotFoundError) {
        if (port && port->isOpen()) {
            port->close();
        }
        
        // Update connection status
        if (portName == "GPS") {
            m_gpsConnected = false;
            emit gpsConnected(false);
        } else if (portName == "IMU") {
            m_imuConnected = false;
            emit imuConnected(false);
        } else if (portName == "AutoSteer") {
            m_autosteerConnected = false;
            emit autosteerConnected(false);
        }
    }
}

// NMEA Processing Helpers
QStringList SerialWorker::extractNMEASentences(QByteArray& buffer)
{
    QStringList sentences;
    
    while (buffer.contains('\n')) {
        int index = buffer.indexOf('\n');
        QByteArray line = buffer.left(index);
        buffer.remove(0, index + 1);
        
        // Remove carriage return if present
        if (line.endsWith('\r')) {
            line.chop(1);
        }
        
        QString sentence = QString::fromLatin1(line);
        if (!sentence.isEmpty()) {
            sentences << sentence;
        }
    }
    
    return sentences;
}

bool SerialWorker::isValidNMEASentence(const QString& sentence)
{
    // Basic NMEA validation
    if (sentence.length() < 7) return false;
    if (!sentence.startsWith('$')) return false;
    if (!sentence.contains('*')) return false;
    
    // Extract and verify checksum
    int checksumIndex = sentence.lastIndexOf('*');
    if (checksumIndex == -1 || checksumIndex >= sentence.length() - 2) return false;
    
    QString datapart = sentence.mid(1, checksumIndex - 1);
    QString providedChecksum = sentence.mid(checksumIndex + 1, 2);
    QString calculatedChecksum = calculateNMEAChecksum(datapart);
    
    return (providedChecksum.toUpper() == calculatedChecksum.toUpper());
}

QString SerialWorker::calculateNMEAChecksum(const QString& sentence)
{
    uint8_t checksum = 0;
    for (const QChar &ch : sentence) {
        checksum ^= ch.toLatin1();
    }
    return QString("%1").arg(checksum, 2, 16, QChar('0')).toUpper();
}