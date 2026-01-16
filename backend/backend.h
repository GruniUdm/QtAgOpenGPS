#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QBindable>
#include <QQmlEngine>
#include <QMutex>

#include "fieldinfo.h"
#include "fixframe.h"
#include "vec2.h"
#include "simpleproperty.h"
#include "cnmea.h"

class Backend : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

    Q_PROPERTY(FieldInfo currentField READ currentField NOTIFY currentFieldChanged)
    Q_PROPERTY(FixFrame fixFrame READ fixFrame NOTIFY fixFrameChanged)
    Q_PROPERTY(QObject* aogRenderer MEMBER aogRenderer NOTIFY aogRendererChanged) //only ever written to by QML

    Q_PROPERTY(CNMEA *pn READ pn CONSTANT)

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
    FixFrame m_fixFrame;
    CNMEA *m_pn;

    QObject *aogRenderer = nullptr;

    static Backend *instance();
    static Backend *create (QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    //const getter for QML
    FieldInfo currentField() const { return m_currentField; }
    FixFrame fixFrame() const { return m_fixFrame; }
    CNMEA *pn() const { return m_pn; }

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

    SIMPLE_BINDABLE_PROPERTY(bool, isJobStarted)
    SIMPLE_BINDABLE_PROPERTY(bool, applicationClosing)
    SIMPLE_BINDABLE_PROPERTY(double, distancePivotToTurnLine)
    SIMPLE_BINDABLE_PROPERTY(bool, isYouTurnRight)
    SIMPLE_BINDABLE_PROPERTY(bool, isYouTurnTriggered)

    //These don't seem to be used outside of FormGPS. When FormGPS
    //becomes CoreGPS singleton, consider moving them to CoreGPS
    SIMPLE_BINDABLE_PROPERTY(bool, imuCorrected)
    SIMPLE_BINDABLE_PROPERTY(bool, isReverseWithIMU)
    //Consider moving this to Tool
    SIMPLE_BINDABLE_PROPERTY(bool, isPatchesChangingColor)


signals:
    //signals implicitly created by BINDABLE_PROPERTY() macro
    void currentFieldChanged();
    void aogRendererChanged();
    void fixFrameChanged();

    void timedMessage(int timeout, QString s1, QString s2);

private:

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Backend, bool, m_isJobStarted, false, &Backend::isJobStartedChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Backend, bool, m_applicationClosing, false, &Backend::applicationClosingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Backend, double, m_distancePivotToTurnLine, 0, &Backend::distancePivotToTurnLineChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Backend, bool, m_isYouTurnRight, false, &Backend::isYouTurnRightChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Backend, bool, m_isYouTurnTriggered, false, &Backend::isYouTurnTriggeredChanged)

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Backend, bool, m_imuCorrected, false, &Backend::imuCorrectedChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Backend, bool, m_isReverseWithIMU, false, &Backend::isReverseWithIMUChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Backend, bool, m_isPatchesChangingColor, false, &Backend::isPatchesChangingColorChanged)
};

#endif // BACKEND_H
