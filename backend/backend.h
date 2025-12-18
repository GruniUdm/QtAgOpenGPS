#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QBindable>
#include <QQmlEngine>
#include <QMutex>

#include "fieldinfo.h"

#include "simpleproperty.h"

class Backend : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

    Q_PROPERTY(FieldInfo currentField READ currentField NOTIFY currentFieldChanged)

private:
    explicit Backend(QObject *parent = nullptr);
    ~Backend() override=default;

    //prevent copying
    Backend(const Backend &) = delete;
    Backend &operator=(const Backend &) = delete;

    static Backend *s_instance;
    static QMutex s_mutex;
    static bool s_cpp_created;

public:
    //allow direct access from C++
    FieldInfo m_currentField;

    static Backend *instance();
    static Backend *create (QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    //const getter for QML
    FieldInfo currentField() const { return m_currentField; }

    Q_INVOKABLE void clearDistanceUser() {
        m_currentField.distanceUser = 0;
        emit currentFieldChanged();
    }

private:
signals:
    //signals implicitly created by BINDABLE_PROPERTY() macro
    void currentFieldChanged();
};

#endif // BACKEND_H
