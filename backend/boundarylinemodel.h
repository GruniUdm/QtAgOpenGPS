// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#ifndef BOUNDARYLINEMODEL_H
#define BOUNDARYLINEMODEL_H

#include <QAbstractListModel>
#include <QColor>
#include <QVariantList>
#include <QVector>

class BoundaryLineModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IndexRole = Qt::UserRole + 1,
        ColorRole,
        WidthRole,
        PointsRole
    };
    Q_ENUM(Roles)

    struct BoundaryLine {
        int index;
        QColor color;
        int width;
        QVariantList points; // List of QPoint
    };

    explicit BoundaryLineModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Data management
    void setBoundaries(const QVector<BoundaryLine> &lines);
    void clear();

private:
    QVector<BoundaryLine> m_boundaries;
};

#endif // BOUNDARYLINEMODEL_H
