#ifndef MAINWINDOWSTATE_H
#define MAINWINDOWSTATE_H

#include <QObject>
#include <QPropertyBinding>
#include <QQmlEngine>
#include <QMutex>
#include "simpleproperty.h"

//convenience macro
#define MAINWINDOW MainWindowState::instance()

class MainWindowState : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(MainWindowState)
    QML_SINGLETON
private:
    explicit MainWindowState(QObject *parent = nullptr);
    ~MainWindowState() override=default;

    //prevent copying
    MainWindowState(const MainWindowState &) = delete;
    MainWindowState &operator=(const MainWindowState &) = delete;

    static MainWindowState *s_instance;
    static QMutex s_mutex;
    static bool s_cpp_created;

public:
    static MainWindowState *instance();
    static MainWindowState *create (QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    enum class ButtonStates {
        Off = 0,
        Auto = 1,
        On = 2
    };
    Q_ENUM(ButtonStates)

    SIMPLE_BINDABLE_PROPERTY(bool, isBtnAutoSteerOn)
    SIMPLE_BINDABLE_PROPERTY(bool, isHeadlandOn)
    SIMPLE_BINDABLE_PROPERTY(bool, isContourBtnOn)
    SIMPLE_BINDABLE_PROPERTY(bool, isYouTurnBtnOn)
    SIMPLE_BINDABLE_PROPERTY(bool, btnIsContourLocked)
    SIMPLE_BINDABLE_PROPERTY(ButtonStates, autoBtnState)
    SIMPLE_BINDABLE_PROPERTY(ButtonStates, manualBtnState)

    //should move this into CSim
    //SIMPLE_BINDABLE_PROPERTY(double, simSteerAngle)

private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(MainWindowState, bool, m_isBtnAutoSteerOn, false, &MainWindowState::isBtnAutoSteerOnChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(MainWindowState, bool, m_isHeadlandOn, false, &MainWindowState::isHeadlandOnChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(MainWindowState, bool, m_isContourBtnOn, false, &MainWindowState::isContourBtnOnChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(MainWindowState, bool, m_isYouTurnBtnOn, false, &MainWindowState::isYouTurnBtnOnChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(MainWindowState, bool, m_btnIsContourLocked, false, &MainWindowState::btnIsContourLockedChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(MainWindowState, ButtonStates, m_autoBtnState, ButtonStates::Off, &MainWindowState::autoBtnStateChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(MainWindowState, ButtonStates, m_manualBtnState, ButtonStates::Off, &MainWindowState::manualBtnStateChanged)

    //Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(MainWindowState, double, m_simSteerAngle, 0, &MainWindowState::simSteerAngleChanged)

signals:

};

extern MainWindowState *mainWindow;

#endif // MAINWINDOWSTATE_H
