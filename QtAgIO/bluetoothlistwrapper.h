#ifndef BLUETOOTHLISTWRAPPER_H
#define BLUETOOTHLISTWRAPPER_H

#include <QObject>
#include <QVariant>

class BluetoothDeviceList;

class BluetoothListWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant list READ getList NOTIFY setBluetooth_deviceListChanged)

public:
    explicit BluetoothListWrapper(BluetoothDeviceList* list, QObject* parent = nullptr);
    QVariant getList() const;
    void emitListChanged();

signals:
    void setBluetooth_deviceListChanged();

private:
    BluetoothDeviceList* m_list;
};

#endif // BLUETOOTHLISTWRAPPER_H
