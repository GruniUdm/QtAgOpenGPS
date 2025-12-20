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
    Q_PROPERTY(QObject* timedMessage MEMBER timedMessage) //only ever written to by QML
    Q_PROPERTY(QObject* aogRenderer MEMBER aogRenderer NOTIFY aogRendererChanged) //only ever written to by QML
public:
    enum class ButtonStates {Off = 0,Auto = 1,On = 2};

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
    QObject *timedMessage = nullptr;
    QObject *aogRenderer = nullptr;

    static Backend *instance();
    static Backend *create (QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    //const getter for QML
    FieldInfo currentField() const { return m_currentField; }

    //mutation methods for currentField
    Q_INVOKABLE void currentField_setDistanceUser(double newdist) {
        m_currentField.distanceUser = newdist;
        emit currentFieldChanged();
    }

    Q_INVOKABLE void currentField_addWorkedAreaTotal(double netarea) {
        m_currentField.workedAreaTotal += netarea;
        emit currentFieldChanged();
    }

    Q_INVOKABLE void currentField_setWorkedAreaTotal(double area) {
        m_currentField.workedAreaTotal = area;
        emit currentFieldChanged();
    }

    Q_INVOKABLE void currentField_addWorkedAreaTotalUser(double netarea) {
        m_currentField.workedAreaTotalUser += netarea;
        emit currentFieldChanged();
    }

    Q_INVOKABLE void currentField_setWorkedAreaTotalUser(double area) {
        m_currentField.workedAreaTotalUser = area;
        emit currentFieldChanged();
    }

    Q_INVOKABLE void currentField_setActualAreaCovered(double area) {
        m_currentField.actualAreaCovered = area;
        emit currentFieldChanged();
    }

private:

signals:
    //signals implicitly created by BINDABLE_PROPERTY() macro
    void currentFieldChanged();
    void timedMessageChanged(); //may not be needed
    void aogRendererChanged(); //may not be needed
};

#endif // BACKEND_H
