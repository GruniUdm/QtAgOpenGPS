#ifndef MAINWINDOWSTATE_H
#define MAINWINDOWSTATE_H

#include <QObject>
#include <QPropertyBinding>
#include "simpleproperty.h"

class MainWindowState : public QObject
{
    Q_OBJECT
public:
    explicit MainWindowState(QObject *parent = nullptr);

    SIMPLE_BINDABLE_PROPERTY(bool, isBtnAutoSteerOn)

signals:

private:
    Q_OBJECT_BINDABLE_PROPERTY(MainWindowState, bool, m_isBtnAutoSteerOn)
};

#endif // MAINWINDOWSTATE_H
