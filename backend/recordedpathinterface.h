#ifndef RECORDEDPATHINTERFACE_H
#define RECORDEDPATHINTERFACE_H

#include <QObject>
#include <QQmlEngine>
#include <QQmlContext>
#include <QMutex>
#include <QStringList>
#include "simpleproperty.h"
#include "recordedpathproperties.h"

class RecordedPathInterface : public QObject
{
    Q_OBJECT
    QML_SINGLETON
private:
    explicit RecordedPathInterface(QObject *parent = nullptr);
    ~RecordedPathInterface() override = default;

    RecordedPathInterface(const RecordedPathInterface &) = delete;
    RecordedPathInterface &operator=(const RecordedPathInterface &) = delete;

    static RecordedPathInterface *s_instance;
    static QMutex s_mutex;
    static bool s_cpp_created;

public:
    static RecordedPathInterface *instance();
    static RecordedPathInterface *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    // Bindable properties - like BoundaryInterface
    SIMPLE_BINDABLE_PROPERTY(bool, isRecording)
    SIMPLE_BINDABLE_PROPERTY(bool, isDriving)
    SIMPLE_BINDABLE_PROPERTY(int, resumeState)
    SIMPLE_BINDABLE_PROPERTY(int, pointCount)
    
    SIMPLE_BINDABLE_PROPERTY_PTR(RecordedPathProperties*, properties)

    // Actions available as Q_INVOKABLE
    Q_INVOKABLE void recPathLoad();
    Q_INVOKABLE void recPathClear();
    Q_INVOKABLE void recPathFollowStop();
    Q_INVOKABLE void recPathRecordStop();
    Q_INVOKABLE void recPathResumeStyle();
    Q_INVOKABLE void recPathSwapAB();
    Q_INVOKABLE void recPathPick();
    Q_INVOKABLE void pathOpen(const QString &pathName);
    Q_INVOKABLE void pathDelete(const QString &pathName);
    Q_INVOKABLE QStringList scanPathFiles();

signals:
    // Signals for QML to connect to - like BoundaryInterface
    void recordStateChanged(bool isRecording);
    void showPathNewDialog();
    void pathListChanged();

private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(RecordedPathInterface, bool, m_isRecording, false, &RecordedPathInterface::isRecordingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(RecordedPathInterface, bool, m_isDriving, false, &RecordedPathInterface::isDrivingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(RecordedPathInterface, int, m_resumeState, 0, &RecordedPathInterface::resumeStateChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(RecordedPathInterface, int, m_pointCount, 0, &RecordedPathInterface::pointCountChanged)
    Q_OBJECT_BINDABLE_PROPERTY(RecordedPathInterface, RecordedPathProperties*, m_properties, &RecordedPathInterface::propertiesChanged)
};

#endif // RECORDEDPATHINTERFACE_H