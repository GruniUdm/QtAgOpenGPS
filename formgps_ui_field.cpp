// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// GUI to backend field interface
#include "formgps.h"
#include "qmlutil.h"
#include "aogproperty.h"


void FormGPS::field_update_list() {
    QString directoryName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                            + "/" + QCoreApplication::applicationName() + "/Fields";

    QObject *fieldInterface = qmlItem(mainWindow, "fieldInterface");
    QList<QVariant> fieldList;
    QMap<QString, QVariant> field;
    int index = 0;

    QDirIterator it(directoryName, QStringList() << "Field.txt", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()){
        field = FileFieldInfo(it.next());
        if(field.contains("latitude")) {
            field["index"] = index;
            fieldList.append(field);
            index++;
        }
    }
    fieldInterface->setProperty("field_list", fieldList);
}

void FormGPS::field_close() {
    FileSaveEverythingBeforeClosingField();
}

void FormGPS::field_open(QString field_name) {
    FileSaveEverythingBeforeClosingField();
    if (! FileOpenField(field_name)) {
        TimedMessageBox(8000, tr("Saved field does not exist."), QString(tr("Cannot find the requested saved field.")) + " " +
                                                                field_name);

        property_setF_CurrentDir = "Default";
    }
}

void FormGPS::field_new(QString field_name) {
    //assume the GUI will vet the name a little bit
    lock.lockForWrite();
    FileSaveEverythingBeforeClosingField();
    currentFieldDirectory = field_name.trimmed();
    property_setF_CurrentDir = currentFieldDirectory;
    JobNew();

    pn.latStart = pn.latitude;
    pn.lonStart = pn.longitude;
    pn.SetLocalMetersPerDegree();

    FileCreateField();
    FileCreateSections();
    FileCreateRecPath();
    FileCreateContour();
    FileCreateElevation();
    FileSaveFlags();
    FileCreateBoundary();
    FileSaveTram();
    lock.unlock();
}

void FormGPS::field_new_from(QString existing, QString field_name, int flags) {
    lock.lockForWrite();
    FileSaveEverythingBeforeClosingField();
    if (! FileOpenField(existing,flags)) { //load whatever is requested from existing field
        TimedMessageBox(8000, tr("Existing field cannot be found"), QString(tr("Cannot find the existing saved field.")) + " " +
                                                                existing);
    }
    //change to new name
    currentFieldDirectory = field_name;
    property_setF_CurrentDir = currentFieldDirectory;

    FileCreateField();
    FileCreateSections();
    FileCreateElevation();
    FileSaveFlags();
    FileSaveABLines();
    FileSaveCurveLines();

    contourSaveList.clear();
    contourSaveList.append(ct.ptList);
    FileSaveContour();

    FileSaveRecPath();
    FileSaveTram();

    //some how we have to write the existing patches to the disk.
    //FileSaveSections only write pending triangles

    for(QSharedPointer<PatchTriangleList> &l: triStrip[0].patchList) {
        tool.patchSaveList.append(l);
    }
    FileSaveSections();
    lock.unlock();
}

void FormGPS::field_delete(QString field_name) {
    QString directoryName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                            + "/" + QCoreApplication::applicationName() + "/Fields/" + field_name;

    QDir fieldDir(directoryName);

    if(! fieldDir.exists()) {
        TimedMessageBox(8000,tr("Cannot find saved field"),QString(tr("Cannot find saved field to delete.")) + " " + field_name);
        return;
    }
    if(!QFile::moveToTrash(directoryName)){
        fieldDir.removeRecursively();
    }
    field_update_list();
}
