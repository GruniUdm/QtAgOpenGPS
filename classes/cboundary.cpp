#include "cboundary.h"
#include "classes/settingsmanager.h"
#include "boundaryinterface.h"
#include "backend.h"
#include "glm.h"
#include "cvehicle.h"

CBoundary::CBoundary(QObject *parent) : QObject(parent)
{
    turnSelected = 0;

    //automatically connect to BoundaryInterface for QML interaction
    connect(BoundaryInterface::instance(), &BoundaryInterface::calculateAreaRequested, this, &CBoundary::calculateArea);
    connect(BoundaryInterface::instance(), &BoundaryInterface::updateListRequested, this, &CBoundary::updateList);
    connect(BoundaryInterface::instance(), &BoundaryInterface::startRequested, this, &CBoundary::start);
    connect(BoundaryInterface::instance(), &BoundaryInterface::stopRequested, this, &CBoundary::stop);
    connect(BoundaryInterface::instance(), &BoundaryInterface::addPointRequested, this, &CBoundary::addPoint);
    connect(BoundaryInterface::instance(), &BoundaryInterface::deleteLastPointRequested, this, &CBoundary::deleteLastPoint);
    connect(BoundaryInterface::instance(), &BoundaryInterface::pauseRequested, this, &CBoundary::pause);
    connect(BoundaryInterface::instance(), &BoundaryInterface::recordRequested, this, &CBoundary::record);
    connect(BoundaryInterface::instance(), &BoundaryInterface::resetRequested, this, &CBoundary::reset);
    connect(BoundaryInterface::instance(), &BoundaryInterface::deleteBoundaryRequested, this, &CBoundary::deleteBoundary);
    connect(BoundaryInterface::instance(), &BoundaryInterface::setDriveThroughRequested, this, &CBoundary::setDriveThrough);
    connect(BoundaryInterface::instance(), &BoundaryInterface::deleteAllRequested, this, &CBoundary::deleteAll);
}

void CBoundary::loadSettings() {
    isSectionControlledByHeadland = SettingsManager::instance()->headland_isSectionControlled();
}


void CBoundary::AddCurrentPoint(double min_dist) {
    if (min_dist > 0 && glm::Distance(CVehicle::instance()->pivotAxlePos, prevBoundaryPos) < min_dist) {
        //we haven't traveled far enough to record another point
        return;
    }

    prevBoundaryPos.easting = CVehicle::instance()->pivotAxlePos.easting;
    prevBoundaryPos.northing = CVehicle::instance()->pivotAxlePos.northing;

    //build the boundary line
    if (isOkToAddPoints)
    {
        if (BoundaryInterface::instance()->isDrawRightSide())
        {
            //Right side
            Vec3 point(CVehicle::instance()->pivotAxlePos.easting + sin(CVehicle::instance()->pivotAxlePos.heading - glm::PIBy2) * -BoundaryInterface::instance()->createBndOffset(),
                       CVehicle::instance()->pivotAxlePos.northing + cos(CVehicle::instance()->pivotAxlePos.heading - glm::PIBy2) * -BoundaryInterface::instance()->createBndOffset(),
                       CVehicle::instance()->pivotAxlePos.heading);
            bndBeingMadePts.append(point);
        }

        //draw on left side
        else
        {
            //Right side
            Vec3 point(CVehicle::instance()->pivotAxlePos.easting + sin(CVehicle::instance()->pivotAxlePos.heading - glm::PIBy2) * BoundaryInterface::instance()->createBndOffset(),
                       CVehicle::instance()->pivotAxlePos.northing + cos(CVehicle::instance()->pivotAxlePos.heading - glm::PIBy2) * BoundaryInterface::instance()->createBndOffset(),
                       CVehicle::instance()->pivotAxlePos.heading);
            bndBeingMadePts.append(point);
        }
        calculateArea(); //update area for GUI
    }
}

void CBoundary::UpdateFieldBoundaryGUIAreas() {
    if (bndList.count() > 0)
    {
        Backend::instance()->m_currentField.areaOuterBoundary = bndList[0].area;
        Backend::instance()->m_currentField.areaBoundaryOuterLessInner = bndList[0].area;

        for (int i = 1; i < bndList.count(); i++)
        {
            Backend::instance()->m_currentField.areaBoundaryOuterLessInner -= bndList[i].area;
        }
    }
    else
    {
        Backend::instance()->m_currentField.areaOuterBoundary = 0;
        Backend::instance()->m_currentField.areaBoundaryOuterLessInner = 0;
    }

    emit Backend::instance()->currentFieldChanged();
}

bool CBoundary::CalculateMinMax() {
    Backend::instance()->m_currentField.minX = 9999999;
    Backend::instance()->m_currentField.minY = 9999999;
    Backend::instance()->m_currentField.maxX = -9999999;
    Backend::instance()->m_currentField.maxY = -9999999;

    if (bndList.count() == 0) return false;

    int bndCnt = bndList[0].fenceLine.count();
    for (int i = 0; i < bndCnt; i++)
    {
        double x = bndList[0].fenceLine[i].easting;
        double y = bndList[0].fenceLine[i].northing;

        //also tally the max/min of field x and z
        if (Backend::instance()->m_currentField.minX > x) Backend::instance()->m_currentField.minX = x;
        if (Backend::instance()->m_currentField.maxX < x) Backend::instance()->m_currentField.maxX = x;
        if (Backend::instance()->m_currentField.minY > y) Backend::instance()->m_currentField.minY = y;
        if (Backend::instance()->m_currentField.maxY < y) Backend::instance()->m_currentField.maxY = y;
    }

    //calculate center of field and max distance
    double maxFieldDistance;
    double fieldCenterX, fieldCenterY;

    //the largest distancew across field
    double dist = fabs(Backend::instance()->m_currentField.minX - Backend::instance()->m_currentField.maxX);
    double dist2 = fabs(Backend::instance()->m_currentField.minY - Backend::instance()->m_currentField.maxY);

    if (dist > dist2) maxFieldDistance = (dist);
    else maxFieldDistance = (dist2);

    if (maxFieldDistance < 100) maxFieldDistance = 100;
    if (maxFieldDistance > 19900) maxFieldDistance = 19900;

    Backend::instance()->m_currentField.calcCenter();

    Backend::instance()->m_currentField.maxDistance = maxFieldDistance;


    emit Backend::instance()->currentFieldChanged();
    return true;
}

//former methods from formgps_ui_boundary
void CBoundary::calculateArea() {
    int ptCount = bndBeingMadePts.count();
    double area = 0;

    if (ptCount > 0)
    {
        int j = ptCount - 1;  // The last vertex is the 'previous' one to the first

        for (int i = 0; i < ptCount; j = i++)
        {
            area += (bndBeingMadePts[j].easting + bndBeingMadePts[i].easting) * (bndBeingMadePts[j].northing - bndBeingMadePts[i].northing);
        }
        area = fabs(area / 2);
    }

    BoundaryInterface::instance()->set_area(area);

    // Update properties - automatic Qt 6.8 notification
    if (ptCount >= 3) {
        BoundaryInterface::instance()->set_pointCount(ptCount);
    }
}

void CBoundary::updateList() {
    QList<QVariant> boundaryList;
    QMap<QString, QVariant> bndMap;

    int index = 0;

    for (CBoundaryList &b: bndList) {
        bndMap["index"] = index++;
        bndMap["area"] = b.area;
        bndMap["drive_through"] = b.isDriveThru;

        boundaryList.append(bndMap);
    }

    BoundaryInterface::instance()->set_list(boundaryList);
}

void CBoundary::start() {
    double tool_width = SettingsManager::instance()->vehicle_toolWidth();
    BoundaryInterface::instance()->set_createBndOffset(tool_width * 0.5);
    isBndBeingMade = true;
    bndBeingMadePts.clear();
    calculateArea();

    // Update properties - automatic Qt 6.8 notification
    BoundaryInterface::instance()->set_isRecording(true);
    BoundaryInterface::instance()->set_pointCount(0);
}

void CBoundary::stop() {
    if (bndBeingMadePts.count() > 2)
    {
        CBoundaryList New;

        for (int i = 0; i < bndBeingMadePts.count(); i++)
        {
            New.fenceLine.append(bndBeingMadePts[i]);
        }

        New.CalculateFenceArea(bndList.count());
        New.FixFenceLine(bndList.count());

        bndList.append(New);
        //this is really our business, but the question is where to store it.
        UpdateFieldBoundaryGUIAreas();

        //turn lines made from boundaries
        CalculateMinMax();
        emit saveBoundaryRequested();
        BuildTurnLines();
    }

    //stop it all for adding
    isOkToAddPoints = false;
    isBndBeingMade = false;
    bndBeingMadePts.clear();
    updateList();
    BoundaryInterface::instance()->set_count(bndList.count());
    BoundaryInterface::instance()->set_isRecording(false);
    if (bndList.count() > 0) {
        BoundaryInterface::instance()->set_area(bndList[0].area);
    }
}

void CBoundary::addPoint() {
    //Add a manual point from button click

    isOkToAddPoints = true;
    AddCurrentPoint(0);
    isOkToAddPoints = false; //manual add stops automatic recording.

    // Update properties - automatic Qt 6.8 notification
    BoundaryInterface::instance()->set_pointCount(bndBeingMadePts.count());
}

void CBoundary::deleteLastPoint() {
    int ptCount = bndBeingMadePts.count();
    if (ptCount > 0)
        bndBeingMadePts.pop_back();
    calculateArea();

    // Update properties - automatic Qt 6.8 notification
    BoundaryInterface::instance()->set_pointCount(bndBeingMadePts.count());
}

void CBoundary::pause() {
    isOkToAddPoints = false;
}

void CBoundary::record() {
    isOkToAddPoints = true;
}

void CBoundary::reset() {
    bndBeingMadePts.clear();
    calculateArea();

    // Reset properties - automatic Qt 6.8 notification
    BoundaryInterface::instance()->set_isRecording(false);
    BoundaryInterface::instance()->set_pointCount(0);
    BoundaryInterface::instance()->set_area(0);
}

void CBoundary::deleteBoundary(int which_boundary) {
    //boundary 0 is special.  It's the outer boundary.
    if (which_boundary == 0 && bndList.count() > 1)
        return; //must remove other boundaries first.

    bndList.remove(which_boundary);
    BoundaryInterface::instance()->set_count(bndList.count());
    updateList();
}

void CBoundary::setDriveThrough(int which_boundary, bool drive_thru) {
    bndList[which_boundary].isDriveThru = drive_thru;
    updateList();
}

void CBoundary::deleteAll() {
    bndList.clear();
    emit saveBoundaryRequested();
    BuildTurnLines();
    BoundaryInterface::instance()->set_count(0);
    updateList();

}
