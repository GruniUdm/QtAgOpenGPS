// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#ifndef SECTIONSMODEL_H
#define SECTIONSMODEL_H

#include <QAbstractListModel>
#include <QVector>

class SectionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum State {
        Off = 0,
        Auto = 1,
        On = 2
    };
    Q_ENUM(State)

    enum Roles {
        IndexRole = Qt::UserRole + 1,
        StateRole
    };
    Q_ENUM(Roles)

    struct SectionState {
        int index;
        int state;
    };

    explicit SectionsModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Data management
    void setSectionStates(const QVector<SectionState> &new_sectionstates);
    void addSectionState(const SectionState &sectionState);
    void removeRowAt(int at_index);
    void clear();

    void setState(int at_index, int state);
    void zeroStates();

    // Utility
    int count() const { return sectionStates.count(); }
    SectionState rowAt(int at_index) const;

    QVector<SectionState> sectionStates;

signals:
    void stateChanged(int index, int state);

private:
};

#endif // SECTIONSMODEL_H
