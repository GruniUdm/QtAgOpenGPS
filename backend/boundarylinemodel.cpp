// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#include "boundarylinemodel.h"

BoundaryLineModel::BoundaryLineModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int BoundaryLineModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_boundaries.count();
}

QVariant BoundaryLineModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_boundaries.count())
        return QVariant();

    const BoundaryLine &line = m_boundaries[index.row()];

    switch (role) {
    case IndexRole:
        return line.index;
    case ColorRole:
        return line.color;
    case WidthRole:
        return line.width;
    case PointsRole:
        return line.points;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> BoundaryLineModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IndexRole] = "boundaryIndex";
    roles[ColorRole] = "color";
    roles[WidthRole] = "width";
    roles[PointsRole] = "points";
    return roles;
}

void BoundaryLineModel::setBoundaries(const QVector<BoundaryLine> &lines)
{
    beginResetModel();
    m_boundaries = lines;
    endResetModel();
}

void BoundaryLineModel::clear()
{
    beginResetModel();
    m_boundaries.clear();
    endResetModel();
}
