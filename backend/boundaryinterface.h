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
    SIMPLE_BINDABLE_PROPERTY(double, createBndOffset)
    SIMPLE_BINDABLE_PROPERTY(bool, isDrawRightSide)

    SIMPLE_BINDABLE_PROPERTY(bool, isRecording)
    SIMPLE_BINDABLE_PROPERTY(double, area)
    SIMPLE_BINDABLE_PROPERTY(int, pointCount)

    SIMPLE_BINDABLE_PROPERTY(int, count)

    SIMPLE_BINDABLE_PROPERTY(QList<QVariant>, list)

    Q_INVOKABLE void calculateArea() { emit calculateAreaRequested(); }
    Q_INVOKABLE void updateList()    { emit updateListRequested(); }
    Q_INVOKABLE void start()         { emit startRequested(); }
    Q_INVOKABLE void stop()          { emit stopRequested(); }
    Q_INVOKABLE void addPoint()      { emit addPointRequested(); }
    Q_INVOKABLE void deleteLastPoint() {emit deleteLastPointRequested(); }
    Q_INVOKABLE void pause()         { emit pauseRequested(); }
    Q_INVOKABLE void record()        { emit recordRequested(); }
    Q_INVOKABLE void reset()         { emit resetRequested(); }
    Q_INVOKABLE void deleteBoundary(int id) { emit deleteBoundaryRequested(id); }
    Q_INVOKABLE void setDriveThrough(int id, bool drive_thru) {
        emit setDriveThroughRequested(id, drive_thru);
    }
    Q_INVOKABLE void deleteAll()     { emit deleteAllRequested(); }

private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(BoundaryInterface, bool, m_isOutOfBounds, false, &BoundaryInterface::isOutOfBoundsChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(BoundaryInterface, double, m_createBndOffset, 0, &BoundaryInterface::createBndOffsetChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(BoundaryInterface, bool, m_isDrawRightSide, false, &BoundaryInterface::isDrawRightSideChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(BoundaryInterface, bool, m_isRecording, false, &BoundaryInterface::isRecordingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(BoundaryInterface, double, m_area, false, &BoundaryInterface::areaChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(BoundaryInterface, int, m_pointCount, false, &BoundaryInterface::pointCountChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(BoundaryInterface, int, m_count, false, &BoundaryInterface::countChanged)
    Q_OBJECT_BINDABLE_PROPERTY(BoundaryInterface, QList<QVariant>, m_list, &BoundaryInterface::listChanged)

signals:
    void calculateAreaRequested();
    void updateListRequested();
    void startRequested();
    void stopRequested();
    void addPointRequested();
    void deleteLastPointRequested();
    void pauseRequested();
    void recordRequested();
    void resetRequested();
    void deleteBoundaryRequested(int id);
    void setDriveThroughRequested(int id, bool drive_thru);
    void deleteAllRequested();
};

#endif // BOUNDARYINTERFACE_H
