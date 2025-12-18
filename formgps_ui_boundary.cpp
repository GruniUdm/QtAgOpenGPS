// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later

//GUI to backend boundary interface
#include "formgps.h"
#include "qmlutil.h"
#include "classes/settingsmanager.h"
#include "backend.h"

void FormGPS::boundary_new_from_KML(QString filename) {

    qDebug() << "Opening KML file:" << filename;
    QUrl fileUrl(filename);
    QString localPath = fileUrl.toLocalFile();
    FindLatLon(localPath);

    // Phase 6.3.1: Use PropertyWrapper for safe property access
    this->setLatStart(latK);
    // Phase 6.3.1: Use PropertyWrapper for safe property access
    this->setLonStart(lonK);
    if (timerSim.isActive())
    {
        pn.latitude = this->latStart();
        pn.longitude = this->lonStart();

        sim.latitude = this->latStart();
        SettingsManager::instance()->setGps_simLatitude(this->latStart());
        sim.longitude = this->lonStart();
        SettingsManager::instance()->setGps_simLongitude(this->lonStart());
    }
    // Phase 6.3.1: Use PropertyWrapper for safe QObject access
    pn.SetLocalMetersPerDegree(this);
    LoadKMLBoundary(localPath);
    bnd.stop();
}
void FormGPS::addboundaryOSMPoint(double latitude, double longitude)
{   qDebug()<<"point.easting";
    double northing;
    double easting;
    pn.ConvertWGS84ToLocal(latitude, longitude, northing, easting, this);
    //save the north & east as previous
    Vec3 point(easting,northing,0);
    bnd.bndBeingMadePts.append(point);
    bnd.calculateArea();
}
