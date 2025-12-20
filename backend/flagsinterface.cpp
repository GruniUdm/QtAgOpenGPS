// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#include <QCoreApplication>
#include "flagsinterface.h"

FlagsInterface *FlagsInterface::s_instance = nullptr;
QMutex FlagsInterface::s_mutex;
bool FlagsInterface::s_cpp_created = false;

FlagsInterface::FlagsInterface(QObject *parent)
    : QObject{parent}
{
    m_flagModel = new FlagModel(this);
}

FlagsInterface *FlagsInterface::instance()
{
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new FlagsInterface();
        s_cpp_created = true;
        // Ensure cleanup on app exit
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         s_instance, []() {
                             delete s_instance;
                             s_instance = nullptr;
                         });
    }
    return s_instance;
}

FlagsInterface *FlagsInterface::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if (!s_instance) {
        s_instance = new FlagsInterface();
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}

int FlagsInterface::count() const { return m_count.value(); }
void FlagsInterface::setCount(const int new_count) {
    m_count.setValue(new_count);
}
QBindable<int> FlagsInterface::bindableCount() {
    return &m_count;
}

int FlagsInterface::currentFlag() const { return m_currentFlag.value(); }
void FlagsInterface::setCurrentFlag(const int &index) {
    m_currentFlag.setValue(index);

    setCurrentColor(m_flagModel->flags[index].color);
    set_currentLatitude(m_flagModel->flags[index].latitude);
    set_currentLongitude(m_flagModel->flags[index].longitude);
    set_currentHeading(m_flagModel->flags[index].heading);
    set_currentEasting(m_flagModel->flags[index].easting);
    set_currentNorthing(m_flagModel->flags[index].northing);
    setCurrentNotes(m_flagModel->flags[index].notes);

}

QBindable<int> FlagsInterface::bindableCurrentFlag() {
    return &m_currentFlag;
}

QString FlagsInterface::currentNotes() {
    return m_currentNotes.value();
}

void FlagsInterface::setCurrentNotes(const QString &value) {
    //update our own value but also update the model
    m_currentNotes.setValue(value);
    m_flagModel->setNotes(currentFlag(), value);
}

QBindable<QString> FlagsInterface::bindableCurrentNotes() {
    return &m_currentNotes;
}


QColor FlagsInterface::currentColor() {
    return m_currentColor.value();
}

void FlagsInterface::setCurrentColor(const QColor &value) {
    //update our own value but also update the model
    m_currentColor.setValue(value);
    m_flagModel->setColor(currentFlag(), value);
}

QBindable<QColor> FlagsInterface::bindableCurrentColor() {
    return &m_currentColor;
}

void FlagsInterface::setNotes(int index, QString notes) {
    m_flagModel->setNotes(index, notes);
    if (index == currentFlag())
        setCurrentNotes(notes);
}

void FlagsInterface::setColor(int index, QColor color) {
    m_flagModel->setColor(index, color);
}

int FlagsInterface::drop(double latitude, double longitude,
                          double easting, double northing,
                          double heading, QColor color,
                          QString notes)
{
    //drop a flag at these coordinates
    int count = m_flagModel->count();
    FlagModel::Flag flag( { count, color, latitude, longitude, heading, easting, northing, notes });
    m_flagModel->addFlag(flag);

    //make the latest flag the current one.
    setCurrentFlag(count);
    setCount(count+1);
    return count; //return index of the added flag.
}

