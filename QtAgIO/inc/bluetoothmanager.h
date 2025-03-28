#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothSocket>
#include "formloop.h"


class BluetoothManager : public QObject
{
    Q_OBJECT
private:
    FormLoop* formLoop;

public:
    explicit BluetoothManager(FormLoop* loop, QObject* parent = nullptr);    ~BluetoothManager();
public slots:
    void userConnectBluetooth(const QString &device);
    void userRemoveDevice(const QString &device);
    void kill();

private:
    QBluetoothSocket *socket;
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;

    QString connectedDeviceName;
    QString rawBuffer;

    QStringList devicesNotAvailable;

    QStringList devicesUserWants;
    QStringList deviceList;

    bool deviceConnected = false;
    bool deviceConnecting = false;

    bool consoleDebug = false;

private slots:
    void connectToDevice(const QBluetoothDeviceInfo &device);
    void connected();
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void disconnected();
    void discoveryFinished();
    void onSocketErrorOccurred(QBluetoothSocket::SocketError error);
    void readData();
    void bluetooth_console_debug(bool doWeDebug);
public slots:
    void startBluetoothDiscovery();
};
#endif  //BLUETOOTHMANAGER_H
