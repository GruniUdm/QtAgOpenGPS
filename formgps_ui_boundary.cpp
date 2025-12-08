// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later

//GUI to backend boundary interface
#include "formgps.h"
#include "qmlutil.h"

void FormGPS::boundary_calculate_area() {
    int ptCount = bnd.bndBeingMadePts.count();
    double area = 0;

    if (ptCount > 0)
    {
        int j = ptCount - 1;  // The last vertex is the 'previous' one to the first

        for (int i = 0; i < ptCount; j = i++)
        {
            area += (bnd.bndBeingMadePts[j].easting + bnd.bndBeingMadePts[i].easting) * (bnd.bndBeingMadePts[j].northing - bnd.bndBeingMadePts[i].northing);
        }
        area = fabs(area / 2);
    }

    if (boundaryInterface) {
        boundaryInterface->setProperty("area", area);
        boundaryInterface->setProperty("pts", ptCount);
    }

    // Update properties - automatic Qt 6.8 notification
    if (ptCount >= 3) {
        setBoundaryArea(area);
    }
}

void FormGPS::boundary_update_list() {
    QList<QVariant> boundList;
    QMap<QString, QVariant> bndMap;

    int index = 0;

    for (CBoundaryList &b: bnd.bndList) {
        bndMap["index"] = index++;
        bndMap["area"] = b.area;
        bndMap["drive_through"] = b.isDriveThru;
        boundList.append(bndMap);
    }
    if (boundaryInterface) {
        boundaryInterface->setProperty("boundary_list", boundList);
    }
}

void FormGPS::boundary_start() {
    this->setCreateBndOffset(tool.width * 0.5);
    bnd.isBndBeingMade = true;
    bnd.bndBeingMadePts.clear();
    boundary_calculate_area();

    // Update properties - automatic Qt 6.8 notification
    setBoundaryIsRecording(true);
    setBoundaryPointCount(0);
}

void FormGPS::boundary_stop() {
    if (bnd.bndBeingMadePts.count() > 2)
    {
        CBoundaryList New;

        for (int i = 0; i < bnd.bndBeingMadePts.count(); i++)
        {
            New.fenceLine.append(bnd.bndBeingMadePts[i]);
        }

        New.CalculateFenceArea(bnd.bndList.count());
        New.FixFenceLine(bnd.bndList.count());

        bnd.bndList.append(New);
        fd.UpdateFieldBoundaryGUIAreas(bnd.bndList, mainWindow, this);

        //turn lines made from boundaries
        calculateMinMax();
        FileSaveBoundary();
        bnd.BuildTurnLines(fd, mainWindow, this);
    }

    //stop it all for adding
    bnd.isOkToAddPoints = false;
    bnd.isBndBeingMade = false;
    bnd.bndBeingMadePts.clear();
    boundary_update_list();
    if (boundaryInterface) {
        boundaryInterface->setProperty("count", bnd.bndList.count());
    }

    // Update properties - automatic Qt 6.8 notification
    setBoundaryIsRecording(false);
    if (bnd.bndList.count() > 0) {
        setBoundaryArea(bnd.bndList[0].area);
    }
}

void FormGPS::boundary_add_point() {
    bnd.isOkToAddPoints = true;
    AddBoundaryPoint();
    bnd.isOkToAddPoints = false;

    // Update properties - automatic Qt 6.8 notification
    setBoundaryPointCount(bnd.bndBeingMadePts.count());
}

void FormGPS::boundary_delete_last_point() {
    int ptCount = bnd.bndBeingMadePts.count();
    if (ptCount > 0)
        bnd.bndBeingMadePts.pop_back();
    boundary_calculate_area();

    // Update properties - automatic Qt 6.8 notification
    setBoundaryPointCount(bnd.bndBeingMadePts.count());
}

void FormGPS::boundary_pause(){
    bnd.isOkToAddPoints = false;
}

void FormGPS::boundary_record() {
    bnd.isOkToAddPoints = true;
}

void FormGPS::boundary_restart() {
    bnd.bndBeingMadePts.clear();
    boundary_calculate_area();

    // Reset properties - automatic Qt 6.8 notification
    setBoundaryIsRecording(false);
    setBoundaryPointCount(0);
    setBoundaryArea(0.0);
}

void FormGPS::boundary_delete(int which_boundary) {
    //boundary 0 is special.  It's the outer boundary.
    if (which_boundary == 0 && bnd.bndList.count() > 1)
        return; //must remove other boundaries first.

    bnd.bndList.remove(which_boundary);
    if (boundaryInterface) {
        boundaryInterface->setProperty("count", bnd.bndList.count());
    }
    boundary_update_list();
}

void FormGPS::boundary_set_drivethru(int which_boundary, bool drive_through) {
    bnd.bndList[which_boundary].isDriveThru = drive_through;
    boundary_update_list();
}

void FormGPS::boundary_delete_all() {
    bnd.bndList.clear();
    FileSaveBoundary();
    bnd.BuildTurnLines(fd, mainWindow, this);
    if (boundaryInterface) {
        boundaryInterface->setProperty("count", bnd.bndList.count());
    }
    boundary_update_list();
}
void FormGPS::boundary_new_from_KML(QString filename) {

    // qDebug() << "Opening KML file:" << filename;
    // QUrl fileUrl(filename);
    // QString localPath = fileUrl.toLocalFile();
    // FindLatLon(localPath);
    // pn.latStart = latK;
    // pn.lonStart = lonK;
    // if (timerSim.isActive())
    // {
    //     pn.latitude = pn.latStart;
    //     pn.longitude = pn.lonStart;
    //     sim.latitude = pn.latStart;
    //     settings->setValue(SETTINGS_gps_simLatitude, (double)pn.latStart);
    //     sim.longitude = pn.lonStart;
    //     settings->setValue(SETTINGS_gps_simLongitude, (double)pn.lonStart);
    // }
    // pn.SetLocalMetersPerDegree();
    // LoadKMLBoundary(localPath);
    // boundary_stop();
}
void FormGPS::addboundaryOSMPoint(double latitude, double longitude)
{   qDebug()<<"point.easting";
    double northing;
    double easting;
    pn.ConvertWGS84ToLocal(latitude, longitude, northing, easting, this);
    //save the north & east as previous
    Vec3 point(easting,northing,0);
    bnd.bndBeingMadePts.append(point);
    boundary_calculate_area();
}
