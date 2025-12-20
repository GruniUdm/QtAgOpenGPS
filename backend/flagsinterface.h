// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#ifndef FLAGSINTERFACE_H
#define FLAGSINTERFACE_H

#include <QObject>
#include <QProperty>
#include <QQmlEngine>
#include <QMutex>

#include "simpleproperty.h"
#include "flagmodel.h"



class FlagsInterface : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

private:
    explicit FlagsInterface(QObject *parent = nullptr);
    ~FlagsInterface() override = default;

    // Prevent copying
    FlagsInterface(const FlagsInterface &) = delete;
    FlagsInterface &operator=(const FlagsInterface &) = delete;

    static FlagsInterface *s_instance;
    static QMutex s_mutex;
    static bool s_cpp_created;

public:
    static FlagsInterface *instance();
    static FlagsInterface *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    Q_PROPERTY(FlagModel* flagModel READ flagModel CONSTANT)
    FlagModel *flagModel() const { return m_flagModel; }
    FlagModel *m_flagModel;

    Q_PROPERTY(int count READ count WRITE setCount
                   NOTIFY countChanged BINDABLE bindableCount)
    int count() const;
    void setCount( const int new_count);
    QBindable<int> bindableCount();

    Q_PROPERTY(int currentFlag READ currentFlag WRITE setCurrentFlag
                   NOTIFY currentFlagChanged BINDABLE bindableCurrentFlag)
    int currentFlag() const;
    void setCurrentFlag(const int &index);
    QBindable<int> bindableCurrentFlag();

    Q_PROPERTY(QString currentNotes READ currentNotes WRITE setCurrentNotes
                   NOTIFY currentNotesChanged BINDABLE bindableCurrentNotes)
    QString currentNotes();
    void setCurrentNotes(const QString &value);
    QBindable<QString> bindableCurrentNotes();

    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor
                   NOTIFY currentColorChanged BINDABLE bindableCurrentColor)
    QColor currentColor();
    void setCurrentColor(const QColor &value);
    QBindable<QColor> bindableCurrentColor();

    SIMPLE_BINDABLE_PROPERTY(double, currentLatitude)
    SIMPLE_BINDABLE_PROPERTY(double, currentLongitude)
    SIMPLE_BINDABLE_PROPERTY(double, currentHeading)
    SIMPLE_BINDABLE_PROPERTY(double, currentEasting)
    SIMPLE_BINDABLE_PROPERTY(double, currentNorthing)

    //methods for QML to use
    Q_INVOKABLE void setNotes(int index, QString notes);
    Q_INVOKABLE void setColor(int index, QColor color);

    Q_INVOKABLE int drop(double latitude, double longitude, double easting, double northing, double heading, QColor color, QString notes);


signals:
    void countChanged();
    void currentFlagChanged();
    void currentNotesChanged();
    void currentColorChanged();

private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FlagsInterface, int, m_count, 0, &FlagsInterface::countChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FlagsInterface, int, m_currentFlag, -1, &FlagsInterface::currentFlagChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FlagsInterface, double, m_currentLatitude, 255, &FlagsInterface::currentLatitudeChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FlagsInterface, double, m_currentLongitude, 255, &FlagsInterface::currentLongitudeChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FlagsInterface, double, m_currentEasting, 99999999, &FlagsInterface::currentEastingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FlagsInterface, double, m_currentNorthing, 99999999, &FlagsInterface::currentNorthingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FlagsInterface, double, m_currentHeading, 999, &FlagsInterface::currentHeadingChanged)
    Q_OBJECT_BINDABLE_PROPERTY(FlagsInterface, QString, m_currentNotes, &FlagsInterface::currentNotesChanged)
    Q_OBJECT_BINDABLE_PROPERTY(FlagsInterface, QColor, m_currentColor, &FlagsInterface::currentColorChanged)
};

#endif // FLAGSINTERFACE_H
