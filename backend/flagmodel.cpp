// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#include "flagmodel.h"

FlagModel::FlagModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int FlagModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return flags.count();
}

QVariant FlagModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= flags.count())
        return QVariant();

    const Flag &flag = flags[index.row()];

    switch (role) {
    case IdRole:
        return flag.id;
    case ColorRole:
        return flag.color;
    case LatitudeRole:
        return flag.latitude;
    case LongitudeRole:
        return flag.longitude;
    case HeadingRole:
        return flag.heading;
    case EastingRole:
        return flag.easting;
    case NorthingRole:
        return flag.northing;
    case NotesRole:
        return flag.notes;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> FlagModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "flagId";
    roles[ColorRole] = "color";
    roles[LatitudeRole] = "latitude";
    roles[LongitudeRole] = "longitude";
    roles[HeadingRole] = "heading";
    roles[EastingRole] = "easting";
    roles[NorthingRole] = "northing";
    roles[NotesRole] = "notes";
    return roles;
}

void FlagModel::setFlags(const QVector<Flag> &new_flags)
{
    beginResetModel();
    flags = new_flags;
    endResetModel();
}

void FlagModel::addFlag(const Flag &flag)
{
    beginInsertRows(QModelIndex(), flags.count(), flags.count());
    flags.append(flag);
    endInsertRows();
}

void FlagModel::removeFlag(int index)
{
    if (index < 0 || index >= flags.count())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    flags.remove(index);
    endRemoveRows();
}

void FlagModel::clear()
{
    beginResetModel();
    flags.clear();
    endResetModel();
}

void FlagModel::setNotes(int index, QString notes) {
    beginResetModel();
    flags[index].notes = notes;
    endResetModel();
}

void FlagModel::setColor(int index, QColor color) {
    beginResetModel();
    flags[index].color = color;
    endResetModel();
}

FlagModel::Flag FlagModel::flagAt(int index) const
{
    if (index >= 0 && index < flags.count())
        return flags[index];
    return Flag();
}
