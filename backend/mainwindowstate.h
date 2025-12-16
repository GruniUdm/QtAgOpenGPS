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
    SIMPLE_BINDABLE_PROPERTY(bool, isHeadlandOn)

signals:

private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(MainWindowState, bool, m_isBtnAutoSteerOn, false, &MainWindowState::isBtnAutoSteerOnChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(MainWindowState, bool, m_isHeadlandOn, false, &MainWindowState::isHeadlandOnChanged)

};

#endif // MAINWINDOWSTATE_H
