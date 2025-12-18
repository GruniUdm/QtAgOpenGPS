#ifndef BOUNDARYINTERFACE_H
#define BOUNDARYINTERFACE_H

#include <QObject>
#include <QPropertyBinding>
#include <QQmlEngine>
#include <QMutex>
#include "simpleproperty.h"

class BoundaryInterface : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
private:
    explicit BoundaryInterface(QObject *parent = nullptr);
    ~BoundaryInterface() override=default;

    //prevent copying
    BoundaryInterface(const BoundaryInterface &) = delete;
    BoundaryInterface &operator=(const BoundaryInterface &) = delete;

    static BoundaryInterface *s_instance;
    static QMutex s_mutex;
    static bool s_cpp_created;

public:
    static BoundaryInterface *instance();
    static BoundaryInterface *create (QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    SIMPLE_BINDABLE_PROPERTY(bool, isOutOfBounds)

private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(BoundaryInterface, bool, m_isOutOfBounds, false, &BoundaryInterface::isOutOfBoundsChanged)

signals:
};

#endif // BOUNDARYINTERFACE_H
