#ifndef MAINWINDOWSTATE_H
#define MAINWINDOWSTATE_H

#include <QObject>

class MainWindowState : public QObject
{
    Q_OBJECT
public:
    explicit MainWindowState(QObject *parent = nullptr);

signals:
};

#endif // MAINWINDOWSTATE_H
