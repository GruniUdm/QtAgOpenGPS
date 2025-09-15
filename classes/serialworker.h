// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// SerialWorker - Multi-port serial communication worker for AgIOService
// Handles GPS, IMU, and AutoSteer serial communication on dedicated thread

#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QDebug>

class SerialWorker : public QObject
{
    Q_OBJECT

public:
    explicit SerialWorker(QObject *parent = nullptr);
    ~SerialWorker();

    // Port management
    Q_INVOKABLE bool openGPSPort(const QString& portName, int baudRate);
    Q_INVOKABLE bool openIMUPort(const QString& portName, int baudRate);
    Q_INVOKABLE bool openAutoSteerPort(const QString& portName, int baudRate);
    
    Q_INVOKABLE void closeGPSPort();
    Q_INVOKABLE void closeIMUPort();
    Q_INVOKABLE void closeAutoSteerPort();
    Q_INVOKABLE void closeAllPorts();
    
    Q_INVOKABLE QStringList scanAvailablePorts();
    Q_INVOKABLE bool isPortAvailable(const QString& portName);
    
    // Data transmission
    Q_INVOKABLE void writeToGPS(const QByteArray& data);
    Q_INVOKABLE void writeToIMU(const QByteArray& data);
    Q_INVOKABLE void writeToAutoSteer(const QByteArray& data);
    
    // Status queries
    Q_INVOKABLE bool isGPSConnected() const { return m_gpsConnected; }
    Q_INVOKABLE bool isIMUConnected() const { return m_imuConnected; }
    Q_INVOKABLE bool isAutoSteerConnected() const { return m_autosteerConnected; }
    
    Q_INVOKABLE QString getGPSPortName() const { return m_gpsPortName; }
    Q_INVOKABLE QString getIMUPortName() const { return m_imuPortName; }
    Q_INVOKABLE QString getAutoSteerPortName() const { return m_autosteerPortName; }

public slots:
    // Thread control
    void startWorker();
    void stopWorker();
    
    // Configuration
    void configureGPSPort(const QString& portName, int baudRate);
    void configureIMUPort(const QString& portName, int baudRate);
    void configureAutoSteerPort(const QString& portName, int baudRate);

signals:
    // Data reception signals
    void gpsDataReceived(const QString& nmea);
    void imuDataReceived(const QByteArray& imuData);
    void autosteerResponseReceived(const QByteArray& response);
    
    // Raw data for debugging
    void serialDataReceived(const QString& portName, const QByteArray& data);
    
    // Connection status signals
    void gpsConnected(bool connected);
    void imuConnected(bool connected);
    void autosteerConnected(bool connected);
    
    // Error signals
    void serialError(const QString& portName, const QString& error);
    void portOpenError(const QString& portName, const QString& error);
    
    // Status updates
    void workerStarted();
    void workerStopped();

private slots:
    // Data reception handlers
    void onGPSDataReady();
    void onIMUDataReady();
    void onAutoSteerDataReady();
    
    // Error handlers
    void onGPSError(QSerialPort::SerialPortError error);
    void onIMUError(QSerialPort::SerialPortError error);
    void onAutoSteerError(QSerialPort::SerialPortError error);
    
    // Connection monitoring
    void checkConnections();

private:
    // Serial ports
    QSerialPort* m_gpsPort;
    QSerialPort* m_imuPort;
    QSerialPort* m_autosteerPort;
    
    // Data buffers
    QByteArray m_gpsBuffer;
    QByteArray m_imuBuffer;
    QByteArray m_autosteerBuffer;
    
    // Connection status
    bool m_gpsConnected;
    bool m_imuConnected;
    bool m_autosteerConnected;
    
    // Port configurations
    QString m_gpsPortName;
    QString m_imuPortName;
    QString m_autosteerPortName;
    int m_gpsBaudRate;
    int m_imuBaudRate;
    int m_autosteerBaudRate;
    
    // Monitoring
    QTimer* m_connectionTimer;
    QMutex m_mutex;
    
    // Worker state
    bool m_isRunning;
    
    // Helper methods
    bool openPort(QSerialPort*& port, const QString& portName, int baudRate, const QString& description);
    void closePort(QSerialPort*& port, const QString& description);
    void processGPSBuffer();
    void processIMUBuffer();
    void processAutoSteerBuffer();
    void handleSerialError(QSerialPort* port, const QString& portName, QSerialPort::SerialPortError error);
    
    // Data parsing
    QStringList extractNMEASentences(QByteArray& buffer);
    bool isValidNMEASentence(const QString& sentence);
    QString calculateNMEAChecksum(const QString& sentence);
};

#endif // SERIALWORKER_H