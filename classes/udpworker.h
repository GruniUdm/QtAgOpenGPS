#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QByteArray>

/**
 * @brief UDP communication worker for AgOpenGPS data exchange
 * 
 * Handles UDP communication in a separate thread.
 * Receives data from and sends data to AgOpenGPS application.
 */
class UDPWorker : public QObject
{
    Q_OBJECT
    
public:
    explicit UDPWorker(QObject *parent = nullptr);
    ~UDPWorker();
    
    // Configuration methods
    void setListenPort(int port) { m_listenPort = port; }
    int getListenPort() const { return m_listenPort; }

public slots:
    void startUDP(const QString& address, int port);
    void stopUDP();
    void sendToTractor(const QByteArray& data);
    void onUdpDataReceived();
    void onUdpError(QAbstractSocket::SocketError error);
    
    // Raw message sending (for PGN commands)
    Q_INVOKABLE void sendRawMessage(const QByteArray& data, const QString& address, int port);
    
    // Advanced UDP protocols (Phase 4.5.3)
    Q_INVOKABLE void startModuleDiscovery();
    Q_INVOKABLE void scanSubnet(const QString& baseIP);
    Q_INVOKABLE void pingModule(const QString& moduleIP);
    Q_INVOKABLE void wakeUpModules(const QString& broadcastIP = "192.168.1.255");
    Q_INVOKABLE void stopModuleDiscovery();
    
    // Multi-subnet discovery for PGN 201 (subnet configuration)
    Q_INVOKABLE void scanAllSubnets();
    Q_INVOKABLE void sendPgnToDiscoveredSubnet(const QByteArray& pgnData);
    
    // Multi-interface broadcasting (like AgIO C#)
    Q_INVOKABLE void sendToAllInterfaces(const QByteArray& data, const QString& targetIP, int port);

private slots:
    void checkConnectionStatus();
    void sendHeartbeat();

signals:
    void udpDataReceived(const QByteArray& data);
    void nmeaReceived(const QString& nmea);  // GPS NMEA via UDP
    void udpStatusChanged(bool connected);
    void errorOccurred(const QString& error);
    
    // Advanced UDP protocols signals (Phase 4.5.3)
    void moduleDiscovered(const QString& moduleIP, const QString& moduleType);
    void moduleTimeout(const QString& moduleIP);
    void networkScanCompleted(const QStringList& discoveredModules);
    void pgnDataReceived(const QByteArray& pgnData);
    
    // Multi-subnet discovery signals
    void moduleSubnetDiscovered(const QString& moduleIP, const QString& currentSubnet);
    void subnetScanCompleted(const QString& activeSubnet);

private:
    QUdpSocket* m_udpSocket;
    QTimer* m_statusTimer;
    QTimer* m_heartbeatTimer;
    
    // Connection settings
    QHostAddress m_tractorAddress;
    int m_tractorPort;
    int m_listenPort;
    
    // Connection state
    bool m_isRunning;
    bool m_isConnected;
    qint64 m_lastDataTime;
    int m_dataTimeoutMs;
    
    // Statistics
    qint64 m_bytesReceived;
    qint64 m_bytesSent;
    int m_packetsReceived;
    int m_packetsSent;
    double m_receiveRate; // bytes/second
    double m_sendRate;    // bytes/second
    
    // Data processing
    QByteArray m_receiveBuffer;
    QString m_nmeaBuffer;  // Buffer for NMEA sentences from UDP GPS
    
    // Advanced UDP module management (Phase 4.5.3)
    QMap<QString, QDateTime> m_moduleLastSeen;
    QTimer* m_discoveryTimer;
    QTimer* m_timeoutTimer;
    QString m_currentSubnet;
    QStringList m_discoveredModules;
    bool m_discoveryActive;
    
    // Multi-subnet discovery management
    QStringList m_commonSubnets;           // Common subnets to scan
    QString m_discoveredModuleSubnet;      // Current subnet of the module
    QString m_discoveredModuleIP;          // IP of discovered module
    QTimer* m_subnetScanTimer;             // Timer for subnet scanning
    int m_currentSubnetIndex;              // Index in subnet list
    bool m_subnetScanActive;               // Flag for active subnet scan
    QByteArray m_pendingPgnData;           // PGN data to send after discovery
    
    void updateReceiveRate();
    void updateSendRate();
    
    // Advanced protocol methods (Phase 4.5.3)
    void handlePGNProtocol(const QByteArray& data);
    void sendHelloMessage(const QString& moduleIP);
    void processModuleResponse(const QByteArray& data);
    void validateModuleConnection();
    void manageModuleTimeouts();
    void broadcastHello();
    void checkModuleTimeouts();
    void resetStatistics();
    void processUdpData(const QByteArray& data);
    
    // Phase 4.6: GPS binary PGN processing (replaces FormGPS UDP 0xD6)
    void processGpsPgn(const QByteArray& data);
    void processNmeaFromUdp(const QByteArray& data);  // Process NMEA via UDP
    
    // AgOpenGPS protocol helpers
    void processAgOpenGpsData(const QByteArray& data);
    void sendModuleData(const QByteArray& moduleData);
    QByteArray buildHeartbeatPacket() const;
    
    // Connection management
    bool bindSocket();
    void cleanupConnection();
};

#endif // UDPWORKER_H