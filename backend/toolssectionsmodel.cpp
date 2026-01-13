// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#include "toolssectionsmodel.h"

ToolsSectionsModel::ToolsSectionsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ToolsSectionsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return tools.count();
}

QVariant ToolsSectionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= tools.count())
        return QVariant();

    const ToolSections &row = tools[index.row()];

    switch (role) {
    case IndexRole:
        return row.index;
    case SectionsModelRole:
        // Return the QObject pointer for QML
        return QVariant::fromValue(row.sectionsModel.data());
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ToolsSectionsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IndexRole] = "toolIndex";
    roles[SectionsModelRole] = "sectionsModel";
    return roles;
}

void ToolsSectionsModel::setToolsSections(const QVector<ToolSections> &new_toolsSections)
{
    beginResetModel();

    tools = new_toolsSections;
    // Force index to align with real index number
    for (int i = 0; i < tools.count(); i++) {
        tools[i].index = i;
    }
    endResetModel();
}

void ToolsSectionsModel::addToolSections(const ToolSections &toolSections)
{
    beginInsertRows(QModelIndex(), tools.count(), tools.count());

    // Make the index the same as the actual index number
    ToolSections new_tool = toolSections;
    new_tool.index = tools.count();
    tools.append(new_tool);

    endInsertRows();
}

void ToolsSectionsModel::addSectionsModel(SectionButtonsModel *model)
{
    ToolSections tool;
    tool.index = tools.count();
    tool.sectionsModel = model;
    addToolSections(tool);
}

void ToolsSectionsModel::removeRowAt(int at_index)
{
    if (at_index < 0 || at_index >= tools.count())
        return;

    beginRemoveRows(QModelIndex(), at_index, at_index);
    tools.removeAt(at_index);
    endRemoveRows();

    // Adjust indices to match real index number
    for (int i = 0; i < tools.count(); i++) {
        if (tools[i].index != i) {
            tools[i].index = i;
            QModelIndex idx = index(i);
            emit dataChanged(idx, idx, {IndexRole});
        }
    }
}

void ToolsSectionsModel::clear()
{
    beginResetModel();
    tools.clear();
    endResetModel();
}

ToolsSectionsModel::ToolSections ToolsSectionsModel::toolAt(int at_index) const
{
    if (at_index >= 0 && at_index < tools.count())
        return tools[at_index];
    return ToolSections();
}

SectionButtonsModel* ToolsSectionsModel::sectionsModelAt(int at_index) const
{
    if (at_index >= 0 && at_index < tools.count())
        return tools[at_index].sectionsModel.data();
    return nullptr;
}
