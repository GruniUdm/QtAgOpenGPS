#ifndef CTOOL_H
#define CTOOL_H

#include <QString>
#include "csection.h"
#include "cpatches.h"
#include "common.h"
#include <QColor>
#include "qmlblockage.h"
#include "btnenum.h"
#include "vec3.h"

class QOpenGLFunctions;
class QMatrix4x4;
class CVehicle;
class CCamera;
class CTram;


struct PatchBuffer;
struct PatchInBuffer;


class CTool
{
public:
    ///---- in settings
    double width;
    ///----
    double halfWidth, contourWidth;
    double farLeftPosition = 0;
    double farLeftSpeed = 0;
    double farRightPosition = 0;
    double farRightSpeed = 0;

    ///---- in settings
    double overlap;
    double trailingHitchLength, tankTrailingHitchLength;
    double trailingToolToPivotLength;
    double offset;

    double lookAheadOffSetting, lookAheadOnSetting;
    double turnOffDelay;
    ///----

    double lookAheadDistanceOnPixelsLeft, lookAheadDistanceOnPixelsRight;
    double lookAheadDistanceOffPixelsLeft, lookAheadDistanceOffPixelsRight;

    ///---- in settings
    bool isToolTrailing, isToolTBT;
    bool isToolRearFixed, isToolFrontFixed;

    bool isMultiColoredSections, isSectionOffWhenOut;
    ///----

    QString toolAttachType;

    ///---- in settings
    double hitchLength;

    //how many individual sections
    int numOfSections;

    //used for super section off on
    int minCoverage;
    ///----

    bool areAllSectionBtnsOn = true;

    bool isLeftSideInHeadland = true, isRightSideInHeadland = true, isSectionsNotZones;

    //read pixel values
    int rpXPosition;

    int rpWidth;

    ///---- in settings
    QColor secColors[16];

    int zones;
    QVector<int> zoneRanges;
    ///----

    bool isDisplayTramControl;

    double hydLiftLookAheadDistanceLeft = 0.0;
    double hydLiftLookAheadDistanceRight = 0.0;

    Vec3 toolPivotPos;
    Vec3 toolPos;
    Vec3 tankPos;

    //moved the following from the main form to here
    CSection section[MAXSECTIONS+1];
    btnStates sectionButtonState[65];
    qmlblockage blockageRowState;
    int blockage_avg;
    int blockage_min1;
    int blockage_min2;
    int blockage_max;
    int blockage_min1_i;
    int blockage_min2_i;
    int blockage_max_i;
    int blockage_blocked;

    //list of patches to save to disk at next opportunity
    QVector<QSharedPointer<PatchTriangleList>> patchSaveList;

    //list of patches, one per section.  each one has a list of
    //individual patches.
    QVector<CPatches> triStrip = QVector<CPatches>( { CPatches() } );

    bool patchesBufferDirty = true;

    void sectionCalcWidths();
    void sectionCalcMulti();
    void sectionSetPositions();
    void loadSettings();
    void saveSettings();

    CTool();
    //this class needs modelview and projection as separate matrices because some
    //additiona transformations need to be done.
    void DrawTool(QOpenGLFunctions *gl,
                  QMatrix4x4 modelview,
                  QMatrix4x4 projection,
                  bool isJobStarted, bool isHydLiftOn,
                  CCamera &camera, CTram &tram);

    void DrawPatches(QOpenGLFunctions *gl,
                     QMatrix4x4 mvp,
                     int patchCounter,
                     const CCamera &camera,
                     QElapsedTimer &swFrame
                     );

    void DrawPatchesTriangles(QOpenGLFunctions *gl,
                     QMatrix4x4 mvp,
                     int patchCounter,
                     const CCamera &camera,
                     QElapsedTimer &swFrame
                     );

    void clearPatches();
    //void loadPatches();

private:
    QVector<QVector<PatchInBuffer>> patchesInBuffer;
    QVector<PatchBuffer> patchBuffer;

};

#endif // CTOOL_H
