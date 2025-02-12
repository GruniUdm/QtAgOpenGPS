#include "ccamera.h"
#include <QOpenGLContext>
#include "newsettings.h"
#include <math.h>
#include "glm.h"

CCamera::CCamera()
    :camPosZ(0.0)
{
    camFollowing = true;
}

void CCamera::loadSettings()
{
    camPitch = settings->value("display/camPitch").value<double>();
    zoomValue = settings->value("display/camZoom").value<double>();
    camSmoothFactor = (settings->value("display/camSmooth").value<double>() * 0.004) + 0.2;
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

