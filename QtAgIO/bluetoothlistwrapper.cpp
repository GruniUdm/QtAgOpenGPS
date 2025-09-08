#include "bluetoothlistwrapper.h"
#include "src/bluetoothdevicelist.h"

BluetoothListWrapper::BluetoothListWrapper(BluetoothDeviceList* list, QObject* parent)
    : QObject(parent), m_list(list)
{
    // Connect to any change signals from the original list
    // and forward them as our own signals
}

QVariant BluetoothListWrapper::getList() const
{
    return QVariant::fromValue(m_list);
}

// Emit this when the list changes
void BluetoothListWrapper::emitListChanged()
{
    emit setBluetooth_deviceListChanged();
}
