#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QBindable>
#include <QQmlEngine>
#include <QMutex>

#include "simpleproperty.h"

class Backend : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

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
    static Backend *instance();
    static Backend *create (QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    /* field data (11 properties) and related methods */
    /**************************************************/
    SIMPLE_BINDABLE_PROPERTY(double, areaOuterBoundary)
    SIMPLE_BINDABLE_PROPERTY(double, areaBoundaryOuterLessInner)
    SIMPLE_BINDABLE_PROPERTY(double, workedAreaTotal)
    void addWorkedAreaTotal(double netarea) { set_workedAreaTotal(m_workedAreaTotal + netarea); }

    SIMPLE_BINDABLE_PROPERTY(double, workedAreaTotalUser)
    void addWorkedAreaTotalUser(double netarea) { set_workedAreaTotalUser(m_workedAreaTotalUser + netarea); }
    //Q_INVOKABLE void setWorkedAreaTotalUser(const QString &value); //not used in qml

    SIMPLE_BINDABLE_PROPERTY(double, distanceUser)
    //Q_INVOKABLE void setDistanceUser(const QString& value); //not used in QML

    SIMPLE_BINDABLE_PROPERTY(double, actualAreaCovered)
    SIMPLE_BINDABLE_PROPERTY(double, userSquareMetersAlarm)
        SIMPLE_BINDABLE_PROPERTY(int, sensorData)

    /* Boundary Properties */
    /***********************/
    SIMPLE_BINDABLE_PROPERTY(bool, isOutOfBounds)
    SIMPLE_BINDABLE_PROPERTY(double, createBndOffset)
    SIMPLE_BINDABLE_PROPERTY(bool, isDrawRightSide)

    /* Vehicle state */
    /*****************/
    SIMPLE_BINDABLE_PROPERTY(bool, isHydLiftOn)
    SIMPLE_BINDABLE_PROPERTY(bool, hydLiftDown)
    SIMPLE_BINDABLE_PROPERTY(bool, isChangingDirection)
    SIMPLE_BINDABLE_PROPERTY(bool, isReverse)
    SIMPLE_BINDABLE_PROPERTY(QList<QVariant>, vehicleList)


private:
    //field data (11 properties
    Q_OBJECT_BINDABLE_PROPERTY(Backend, double, m_areaOuterBoundary, &Backend::areaOuterBoundaryChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, double, m_areaBoundaryOuterLessInner, &Backend::areaBoundaryOuterLessInnerChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, double, m_workedAreaTotal, &Backend::workedAreaTotalChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, double, m_workedAreaTotalUser, &Backend::workedAreaTotalUserChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, double, m_distanceUser, &Backend::distanceUserChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, double, m_actualAreaCovered, &Backend::actualAreaCoveredChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, double, m_userSquareMetersAlarm, &Backend::userSquareMetersAlarmChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, int, m_sensorData, &Backend::sensorDataChanged)

    //boundary properties
    //vehicle state
    Q_OBJECT_BINDABLE_PROPERTY(Backend, bool, m_isOutOfBounds, &Backend::isOutOfBoundsChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, double, m_createBndOffset, &Backend::createBndOffsetChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, bool, m_isDrawRightSide, &Backend::isDrawRightSideChanged)

    //vehicle state
    Q_OBJECT_BINDABLE_PROPERTY(Backend, bool, m_isHydLiftOn, &Backend::isHydLiftOnChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, bool, m_hydLiftDown, &Backend::hydLiftDownChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, bool, m_isChangingDirection, &Backend::isChangingDirectionChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, bool, m_isReverse, &Backend::isReverseChanged)
    Q_OBJECT_BINDABLE_PROPERTY(Backend, QList<QVariant>, m_vehicleList, &Backend::vehicleListChanged)

signals:
    //signals implicitly created by BINDABLE_PROPERTY() macro
};

#endif // BACKEND_H
