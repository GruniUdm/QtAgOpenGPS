#ifndef RECORDEDPATH_H
#define RECORDEDPATH_H

#include <QObject>
#include <QQmlEngine>
#include <QProperty>
#include <QMutex>
#include "backend/recordedpathmodel.h"
#include "simpleproperty.h"

class RecordedPath : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
private:
    explicit RecordedPath(QObject *parent = nullptr);
    ~RecordedPath() override = default;

    // Prevent copying
    RecordedPath(const RecordedPath &) = delete;
    RecordedPath &operator=(const RecordedPath &) = delete;

    static RecordedPath *s_instance;
    static QMutex s_mutex;
    static bool s_cpp_created;

public:
    static RecordedPath *instance();
    static RecordedPath *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    // Model-based recorded path list
    Q_PROPERTY(RecordedPathModel* model READ model CONSTANT)
    RecordedPathModel* model() const { return m_model; }

    SIMPLE_BINDABLE_PROPERTY(bool, isDrivingRecordedPath)
    SIMPLE_BINDABLE_PROPERTY(QString, currentPathName)

signals:
    // Signals for actions that C++ code can connect to
    void updateLines();
    void open(QString name);
    void remove(QString name);
    void startDriving();
    void stopDriving();
    void clear();

private:
    RecordedPathModel *m_model;

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(RecordedPath, bool, m_isDrivingRecordedPath, false, &RecordedPath::isDrivingRecordedPathChanged)
    Q_OBJECT_BINDABLE_PROPERTY(RecordedPath, QString, m_currentPathName, &RecordedPath::currentPathNameChanged)

};

#endif // RECORDEDPATH_H
