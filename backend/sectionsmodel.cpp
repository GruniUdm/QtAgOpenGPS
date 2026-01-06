// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#include "sectionsmodel.h"

SectionsModel::SectionsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int SectionsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return sectionStates.count();
}

QVariant SectionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= sectionStates.count())
        return QVariant();

    const SectionState &section = sectionStates[index.row()];

    switch (role) {
    case IndexRole:
        return section.index;
    case StateRole:
        return section.state;
    default:
        return QVariant();
    }
}

bool SectionsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= sectionStates.count())
        return false;

    SectionState &row = sectionStates[index.row()];

    switch (role) {
    case StateRole:
        if (row.state != value.toInt()) {
            row.state = value.toInt();
            emit dataChanged(index, index, {StateRole});
            emit stateChanged(index.row(), row.state);
            return true;
        }
        break;
    case IndexRole:
        // Index is read-only, managed internally
        return false;
    }

    return false;
}

Qt::ItemFlags SectionsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> SectionsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IndexRole] = "secNumber";
    roles[StateRole] = "state";
    return roles;
}

void SectionsModel::setSectionStates(const QVector<SectionState> &new_sectionstates)
{
    beginResetModel();

    sectionStates = new_sectionstates;
    //force secNumber to align with real index number
    for (int i = 0 ; i < sectionStates.count(); i++) {
        sectionStates[i].index = i;
    }
    endResetModel();
}

void SectionsModel::addSectionState(const SectionState &sectionState)
{
    beginInsertRows(QModelIndex(), sectionStates.count(), sectionStates.count());

    //make the secNumber the same as the actual index number
    SectionState new_state = sectionState;
    new_state.index = sectionStates.count();
    sectionStates.append(new_state);

    endInsertRows();
}

void SectionsModel::removeRowAt(int at_index)
{
    beginRemoveRows(QModelIndex(), at_index, at_index);
    sectionStates.removeAt(at_index);
    endRemoveRows();

    //adjust secNumbers to match real index number
    for (int i = 0; i < sectionStates.count(); i++) {
        if (sectionStates[i].index != i) {
            sectionStates[i].index = i;
            QModelIndex idx = index(at_index);
            emit dataChanged(idx, idx, {StateRole});
        }
    }
}

void SectionsModel::zeroStates() {
    for (int i=0; i < sectionStates.count(); i++) {
        setState(i,0);
    }
}

void SectionsModel::clear()
{
    beginResetModel();
    sectionStates.clear();
    endResetModel();
}

void SectionsModel::setState(int at_index, int state)
{
    if (at_index < 0 || at_index >= sectionStates.count())
        return;

    sectionStates[at_index].index = at_index;
    sectionStates[at_index].state = state;
    QModelIndex idx = index(at_index);
    emit dataChanged(idx, idx, {StateRole});
    emit stateChanged(at_index, state);
}

SectionsModel::SectionState SectionsModel::rowAt(int at_index) const
{
    if (sectionStates.count() > at_index)
        return sectionStates[at_index];
    return SectionState();
}
