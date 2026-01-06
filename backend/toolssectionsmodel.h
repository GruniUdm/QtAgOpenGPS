// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#ifndef TOOLSSECTIONSMODEL_H
#define TOOLSSECTIONSMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QPointer>
#include "sectionsmodel.h"

class ToolsSectionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IndexRole = Qt::UserRole + 1,
        SectionsModelRole
    };
    Q_ENUM(Roles)

    struct ToolSections {
        int index;
        QPointer<SectionsModel> sectionsModel;
    };

    explicit ToolsSectionsModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Data management
    void setToolsSections(const QVector<ToolSections> &new_toolsSections);
    void addToolSections(const ToolSections &tool);
    void addSectionsModel(SectionsModel *model);
    void removeRowAt(int at_index);
    void clear();

    // Utility
    int count() const { return tools.count(); }
    ToolSections toolAt(int at_index) const;
    SectionsModel* sectionsModelAt(int at_index) const;

    QVector<ToolSections> tools;

private:
};

#endif // TOOLSSECTIONSMODEL_H
