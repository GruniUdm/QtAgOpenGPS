#include "ccamera.h"
#include <QOpenGLContext>
#include "classes/settingsmanager.h"
#include <math.h>
#include "glm.h"

CCamera::CCamera()
    :camPosZ(0.0)
{
    camFollowing = true;
}

void CCamera::loadSettings()
{
    camPitch = SettingsManager::instance()->value(SETTINGS_display_camPitch).value<double>();
    zoomValue = SettingsManager::instance()->value(SETTINGS_display_camZoom).value<double>();
    camSmoothFactor = (SettingsManager::instance()->value(SETTINGS_display_camSmooth).value<double>() * 0.004) + 0.2;
}

void CCamera::SetWorldCam(QMatrix4x4 &modelview,
                          double _fixPosX, double _fixPosY,
                          double _fixHeading)
{

    camPosX = _fixPosX;
    camPosY = _fixPosY;
    camYaw = _fixHeading;

    //back the camera up
    modelview.translate(0,0,camSetDistance * 0.5);

    //rotate the camera down to look at fix
    modelview.rotate(camPitch, 1.0, 0, 0);

    //pan if set
    modelview.translate(panX, panY, 0);

    //following game style or N fixed cam
    if(camFollowing) {
        modelview.rotate(camYaw, 0,0,1);
        modelview.translate(-camPosX, -camPosY, -camPosZ);
    } else {
        modelview.translate(-camPosX, -camPosY, -camPosZ);
    }
}

void CCamera::SetZoom()
{
    //match grid to cam distance and redo perspective
    if (camSetDistance <= -20000) gridZoom = 2000;
    if (camSetDistance >= -20000 && camSetDistance < -10000) gridZoom =   2012;
    if (camSetDistance >= -10000 && camSetDistance < -5000) gridZoom =    1006;
    if (camSetDistance >= -5000 && camSetDistance < -2000) gridZoom =     503;
    if (camSetDistance >= -2000 && camSetDistance < -1000) gridZoom =     201.2;
    if (camSetDistance >= -1000 && camSetDistance < -500) gridZoom =      100.6;
    if (camSetDistance >= -500 && camSetDistance < -250) gridZoom =       50.3;
    if (camSetDistance >= -250 && camSetDistance < -150) gridZoom =       25.15;
    if (camSetDistance >= -150 && camSetDistance < -50) gridZoom =         10.06;
    if (camSetDistance >= -50 && camSetDistance < -1) gridZoom = 5.03;
    //1.216 2.532
}


