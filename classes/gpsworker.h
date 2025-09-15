#ifndef GPSWORKER_H
#define GPSWORKER_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QByteArray>
#include "cnmea.h"

/**
 * @brief GPS serial communication worker
 * 
 * Handles GPS serial port communication in a separate thread.
 * Parses NMEA sentences and emits position data.
 */
class GPSWorker : public QObject
{
    Q_OBJECT
    
public:
    explicit GPSWorker(QObject *parent = nullptr);
    ~GPSWorker();

public slots:
    void startGPS(const QString& serialPort, int baudRate);
    void stopGPS();
    void onSerialDataReceived();
    void onSerialError(QSerialPort::SerialPortError error);
    
    // Slot for receiving NMEA data from SerialWorker
    void processNMEAString(const QString& nmeaSentence);

private slots:
    void checkConnectionStatus();
    void processSerialBuffer();

signals:
    void gpsDataReceived(double latitude, double longitude, double heading, double speed);
    void gpsStatusChanged(bool connected, int quality, int satellites, double age);
    void nmeaSentenceReceived(const QString& sentenceType, const QString& rawSentence);
    void errorOccurred(const QString& error);

private:
    QSerialPort* m_serialPort;
    CNMEA* m_nmeaParser;
    QTimer* m_statusTimer;
    QTimer* m_reconnectTimer;
    QByteArray m_serialBuffer;
    
    // Connection tracking
    bool m_isConnected;
    bool m_isRunning;
    QString m_portName;
    int m_baudRate;
    int m_reconnectAttempts;
    
    // NMEA data tracking
    qint64 m_lastDataTime;
    int m_sentenceCount;
    int m_dataTimeoutMs;
    
    // GPS status
    double m_latitude, m_longitude, m_heading, m_speed;
    int m_gpsQuality, m_satellites;
    double m_age;
    
    // Enhanced GPS state (Phase 4.5.2)
    double m_hdop, m_pdop, m_vdop;  // Dilution of Precision
    double m_altitude, m_geoidHeight;
    double m_accuracy;  // Position accuracy in meters
    QString m_rtkStatus;  // RTK status: "FLOAT", "FIX", "SINGLE"
    double m_roll, m_pitch;  // From F9P PANDA sentences
    int m_satellitesInView;  // Total satellites in view (GSV)
    QDateTime m_lastValidFix;  // Last valid GPS fix time
    
    void processNmeaData(const QByteArray& data);
    void processGGALine(const QString& line);
    void processRMCLine(const QString& line);
    void processVTGLine(const QString& line);
    
    // Enhanced NMEA parsing (Phase 4.5.2)
    void processGSTLine(const QString& line);
    void processGSVLine(const QString& line);
    void processHDTLine(const QString& line);
    void parsePANDA(const QString& line);  // CRITIQUE pour F9P modules
    
    // GPS quality management
    void calculateGPSAge();
    void validateGPSFix();
    void handleGPSLoss();
    
    // Advanced processing
    void processDifferentialCorrections();
    void calculatePositionAccuracy();
    void handleRTKStatus();
    
    void attemptReconnect();
    void updateConnectionStatus();
    void resetGpsData();
    void emitGpsData();
    
    // NMEA parsing helpers
    QStringList parseNmeaSentence(const QString& sentence);
    double convertNmeaLatLon(const QString& value, const QString& hemisphere);
    bool validateChecksum(const QString& sentence);
};

#endif // GPSWORKER_H