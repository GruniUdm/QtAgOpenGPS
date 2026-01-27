// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Boundary properties for FieldViewItem - represents a single boundary

#ifndef BOUNDARYPROPERTIES_H
#define BOUNDARYPROPERTIES_H

#include <QObject>
#include <QProperty>
#include <QBindable>

class BoundaryProperties : public QObject
{
    Q_OBJECT

public:
    explicit BoundaryProperties(QObject *parent = nullptr);

signals:
    void boundaryChanged();
};

#endif // BOUNDARYPROPERTIES_H
