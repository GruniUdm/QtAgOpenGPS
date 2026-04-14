#ifndef RECORDEDPATHINTERFACE_H
#define RECORDEDPATHINTERFACE_H

#include <QObject>
#include <QQmlEngine>
#include <QMutex>

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

    Q_INVOKABLE void recPathLoad();
    Q_INVOKABLE void recPathClear();
    Q_INVOKABLE void recPathFollowStop();
    Q_INVOKABLE void recPathRecordStop();
    Q_INVOKABLE void recPathResumeStyle();
    Q_INVOKABLE void recPathSwapAB();
    Q_INVOKABLE void recPathPick();

private:
signals:
    void recordStateChanged(bool isRecording);
    void showPathNewDialog();
};

#endif // RECORDEDPATHINTERFACE_H