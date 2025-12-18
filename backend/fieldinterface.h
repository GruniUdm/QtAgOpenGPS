#ifndef FIELDINTERFACE_H
#define FIELDINTERFACE_H

#include <QObject>
#include <QQmlEngine>
#include <QMutex>
#include <QPropertyBinding>
#include "simpleproperty.h"

class FieldInterface : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
private:
    explicit FieldInterface(QObject *parent = nullptr);
    ~FieldInterface() override=default;

    //prevent copying
    FieldInterface(const FieldInterface &) = delete;
    FieldInterface &operator=(const FieldInterface &) = delete;

    static FieldInterface *s_instance;
    static QMutex s_mutex;
    static bool s_cpp_created;

public:
    static FieldInterface *instance();
    static FieldInterface *create (QQmlEngine *qmlEngine, QJSEngine *jsEngine);



signals:
};

#endif // FIELDINTERFACE_H
