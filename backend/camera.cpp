// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later

#include "camera.h"
#include "settingsmanager.h"
#include <QCoreApplication>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(camera_log, "camera.qtagopengps")

Camera *Camera::s_instance = nullptr;
QMutex Camera::s_mutex;
bool Camera::s_cpp_created = false;

Camera::Camera(QObject *parent)
    : QObject(parent)
{
    m_camFollowing = true;
}

Camera *Camera::instance()
{
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new Camera();
        qDebug(camera_log) << "Camera singleton created by C++ code.";
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

Camera *Camera::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if (!s_instance) {
        s_instance = new Camera();
        qDebug(camera_log) << "Camera singleton created by QML engine.";
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}

void Camera::loadSettings()
{
}

void Camera::SetWorldCam(QMatrix4x4 &modelview,
                         double _fixPosX, double _fixPosY)
{
    // Back the camera up
    modelview.translate(0, 0, m_camSetDistance * 0.5);

    // Rotate the camera down to look at fix
    modelview.rotate(static_cast<float>(Camera::camPitch()), 1.0f, 0.0f, 0.0f);

    // Pan if set
    modelview.translate(static_cast<float>(m_panX), static_cast<float>(m_panY), 0.0f);

    // Following game style or N fixed cam
    if (m_camFollowing) {
        modelview.rotate(static_cast<float>(m_camHeading), 0.0f, 0.0f, 1.0f);
        modelview.translate(static_cast<float>(-_fixPosX),
                           static_cast<float>(-_fixPosY),
                           static_cast<float>(-0));
    } else {
        modelview.translate(static_cast<float>(-_fixPosX),
                           static_cast<float>(-_fixPosY),
                           static_cast<float>(-0));
    }
}

void Camera::SetZoom()
{
    // Match grid to cam distance and redo perspective
    if (m_camSetDistance <= -20000) m_gridZoom = 2000;
    else if (m_camSetDistance >= -20000 && m_camSetDistance < -10000) m_gridZoom = 2012;
    else if (m_camSetDistance >= -10000 && m_camSetDistance < -5000) m_gridZoom = 1006;
    else if (m_camSetDistance >= -5000 && m_camSetDistance < -2000) m_gridZoom = 503;
    else if (m_camSetDistance >= -2000 && m_camSetDistance < -1000) m_gridZoom = 201.2;
    else if (m_camSetDistance >= -1000 && m_camSetDistance < -500) m_gridZoom = 100.6;
    else if (m_camSetDistance >= -500 && m_camSetDistance < -250) m_gridZoom = 50.3;
    else if (m_camSetDistance >= -250 && m_camSetDistance < -150) m_gridZoom = 25.15;
    else if (m_camSetDistance >= -150 && m_camSetDistance < -50) m_gridZoom = 10.06;
    else if (m_camSetDistance >= -50 && m_camSetDistance < -1) m_gridZoom = 5.03;

    emit gridZoomChanged();
}
