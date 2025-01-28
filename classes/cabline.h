#ifndef CABLINE_H
#define CABLINE_H

#include <QObject>
#include <QVector>
#include "vec2.h"
#include "vec3.h"
#include <QString>

class QOpenGLFunctions;
//namespace AgOpenGPS

class CVehicle;
class CBoundary;
class CYouTurn;
class CTool;
class CNMEA;
class CTram;
class CCamera;
class CAHRS;
class CGuidance;
class CTrk;

class CABLines
{
public:
    Vec2 origin;
    double heading = 0;
    QString Name = "aa";
    bool isVisible = true;
};

class CABLine: public QObject
{
    Q_OBJECT
private:
    int counter2;
    double shadowOffset = 0;
    double widthMinusOverlap = 0;

public:
    double abFixHeadingDelta = 0;
    double abHeading = 0.0, abLength;
    double angVel = 0;

    bool isABValid, isLateralTriggered;


    //the current AB guidance line
    Vec3 currentLinePtA = Vec3(0.0, 0.0, 0.0);
    Vec3 currentLinePtB = Vec3(0.0, 1.0, 0.0);

    double distanceFromCurrentLinePivot;
    double distanceFromRefLine = 0.0;

    //pure pursuit values
    Vec2 goalPointAB = Vec2(0, 0);

    double howManyPathsAway = 0.0;
    bool isMakingABLine;
    bool isHeadingSameWay = true;

    double ppRadiusAB;

    Vec2 radiusPointAB;
    double rEastAB, rNorthAB;

    double snapDistance, lastSecond = 0;
    double steerAngleAB;
    int lineWidth;

    //design
    Vec2 desPtA = Vec2(0.2, 0.15);
    Vec2 desPtB = Vec2(0.2, 0.15);

    Vec2 desLineEndA = Vec2(0.3, 0.3);
    Vec2 desLineEndB = Vec2(0.3, 0.3);

    Vec2 refNudgePtA = Vec2(1,1);
    Vec2 refNudgePtB = Vec2(2,2);

    double desHeading = 0;
    QString desName = "";

    double pivotDistanceError, pivotDistanceErrorLast, pivotDerivative, pivotDerivativeSmoothed;

    //derivative counters
    double inty;
    double steerAngleSmoothed, pivotErrorTotal;
    double distSteerError, lastDistSteerError, derivativeDistError;

    //Color tramColor = Color.YellowGreen;
    int tramPassEvery;

public:
    explicit CABLine(QObject *parent = 0);

    void BuildCurrentABLineList(Vec3 pivot,
                                double secondsSinceStart,
                                CTrk &track,
                                const CYouTurn &yt,
                                const CVehicle &vehicle);
    void GetCurrentABLine(Vec3 pivot, Vec3 steer,
                          bool isBtnAutoSteerOn,
                          CVehicle &vehicle,
                          CYouTurn &yt,
                          const CAHRS &ahrs,
                          CGuidance &gyd,
                          CNMEA &pn);
    void DrawABLineNew(QOpenGLFunctions *gl, const QMatrix4x4 &mvp,
                       const CCamera &camera);

    void DrawABLines(QOpenGLFunctions *gl, const QMatrix4x4 &mvp,
                     bool isFontOn,
                     const CTrk &track,
                     CYouTurn &yt,
                     const CCamera &camera,
                     const CGuidance &gyd);
    void BuildTram(const CTrk &track, CBoundary &bnd, CTram &tram);

    CABLine &operator= (CABLine &src)
    {
        counter2 = src.counter2;
        shadowOffset = src.shadowOffset;
        widthMinusOverlap = src.widthMinusOverlap;

        abFixHeadingDelta = src.abFixHeadingDelta;
        abHeading = src.abHeading;
        abLength = src.abLength;
        angVel = src.angVel;

        isABValid = src.isABValid;
        isLateralTriggered = src.isLateralTriggered;

        currentLinePtA = src.currentLinePtA;
        currentLinePtB = src.currentLinePtB;

        distanceFromCurrentLinePivot = src.distanceFromCurrentLinePivot;
        distanceFromRefLine = src.distanceFromRefLine;

        goalPointAB = src.goalPointAB;

        howManyPathsAway = src.howManyPathsAway;
        isMakingABLine = src.isMakingABLine;
        isHeadingSameWay = src.isHeadingSameWay;

        ppRadiusAB = src.ppRadiusAB;

        radiusPointAB = src.radiusPointAB;
        rEastAB = src.rEastAB;
        rNorthAB = src.rNorthAB;

        snapDistance = src.snapDistance;
        lastSecond = src.lastSecond;
        steerAngleAB = src.steerAngleAB;
        lineWidth = src.lineWidth;

        desPtA = src.desPtA;
        desPtB = src.desPtB;

        desLineEndA = src.desLineEndA;
        desLineEndB = src.desLineEndB;

        refNudgePtA = src.refNudgePtA;
        refNudgePtB = src.refNudgePtB;

        desHeading = src.desHeading;
        desName = src.desName;

        pivotDistanceError = src.pivotDistanceError;
        pivotDistanceErrorLast = src.pivotDistanceErrorLast;
        pivotDerivative = src.pivotDerivative;
        pivotDerivativeSmoothed = src.pivotDerivativeSmoothed;

        inty = src.inty;
        steerAngleSmoothed = src.steerAngleSmoothed;
        pivotErrorTotal = src.pivotErrorTotal;
        distSteerError = src.distSteerError;
        lastDistSteerError = src.lastDistSteerError;
        derivativeDistError = src.derivativeDistError;

        tramPassEvery = src.tramPassEvery;

        return *this;
    }

signals:
    void stopAutosteer();
    void TimedMessage(int,QString,QString);
};

#endif // CABLINE_H
