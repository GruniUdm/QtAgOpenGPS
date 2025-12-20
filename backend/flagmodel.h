// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#ifndef FLAGMODEL_H
#define FLAGMODEL_H

#include <QAbstractListModel>
#include <QColor>
#include <QVector>

class FlagModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        ColorRole,
        LatitudeRole,
        LongitudeRole,
        HeadingRole,
        EastingRole,
        NorthingRole,
        NotesRole
    };
    Q_ENUM(Roles)

    struct Flag {
        int id;
        QColor color;
        double latitude;
        double longitude;
        double heading;
        double easting;
        double northing;
        QString notes;
    };

    explicit FlagModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Data management
    void setFlags(const QVector<Flag> &flags);
    void addFlag(const Flag &flag);
    void removeFlag(int index);
    void clear();

    void setNotes(int index, QString notes);
    void setColor(int index, QColor color);

    // Utility
    int count() const { return flags.count(); }
    Flag flagAt(int index) const;

    QVector<Flag> flags;

private:
};

#endif // FLAGMODEL_H
