#ifndef NTRIPWORKER_H
#define NTRIPWORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @brief NTRIP client worker for RTCM corrections
 * 
 * Handles NTRIP caster connection in a separate thread.
 * Downloads RTCM correction data and forwards to GPS receiver.
 */
class NTRIPWorker : public QObject
{
    Q_OBJECT
    
    // NTRIP connection states
    enum ConnectionState {
        Disconnected = 0,
        Connecting = 1,
        Authenticating = 2,
        WaitingForData = 3,
        ReceivingData = 4,
        Error = 5
    };
    
public:
    explicit NTRIPWorker(QObject *parent = nullptr);
    ~NTRIPWorker();

public slots:
    void startNTRIP(const QString& url, const QString& user, 
                   const QString& password, const QString& mount, int port);
    void stopNTRIP();
    void onTcpConnected();
    void onTcpDisconnected();
    void onTcpDataReceived();
    void onTcpError(QAbstractSocket::SocketError error);

private slots:
    void checkConnectionStatus();
    void attemptReconnect();
    void sendNtripRequest();
    void processNtripResponse();

signals:
    void ntripStatusChanged(int status, const QString& statusText);
    void ntripDataReceived(const QByteArray& rtcmData);
    void errorOccurred(const QString& error);

private:
    QTcpSocket* m_tcpSocket;
    QTimer* m_statusTimer;
    QTimer* m_reconnectTimer;
    QByteArray m_receiveBuffer;
    
    // Connection settings
    QString m_ntripUrl;
    QString m_ntripUser;
    QString m_ntripPassword;
    QString m_ntripMount;
    int m_ntripPort;
    
    // Connection state
    ConnectionState m_connectionState;
    bool m_isRunning;
    int m_reconnectAttempts;
    qint64 m_lastDataTime;
    int m_dataTimeoutMs;
    bool m_headerReceived;
    
    // Statistics
    qint64 m_bytesReceived;
    int m_packetsReceived;
    double m_dataRate; // bytes/second
    
    void setState(ConnectionState newState);
    QString stateToString(ConnectionState state) const;
    void resetStatistics();
    void updateDataRate();
    
    // NTRIP protocol helpers
    QString buildNtripRequest() const;
    bool parseNtripResponse(const QByteArray& response);
    void processRtcmData(const QByteArray& data);
    
    // Connection management
    void cleanupConnection();
    bool validateSettings() const;
};

#endif // NTRIPWORKER_H