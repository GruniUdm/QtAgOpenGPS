// Copyright (C) 2024 QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// SatelliteModel - QML model for displaying satellite information
// Data comes from NMEA GSV (satellites in view) and GSA (satellites in use) messages

#ifndef SATELLITEMODEL_H
#define SATELLITEMODEL_H

#include <QAbstractListModel>
#include <QVariant>
#include <QQmlEngine>
#include <QJSEngine>
#include <QtQml/qqmlregistration.h>

#include "pgnparser.h"

class SatelliteModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(int size READ rowCount NOTIFY sizeChanged)
    Q_PROPERTY(int inUseCount READ inUseCount NOTIFY sizeChanged)

public:
    enum SatelliteRoles {
        IdRole = Qt::UserRole + 1,
        RssiRole,
        ElevationRole,
        AzimuthRole,
        SystemIdRole,
        InUseRole,
        SystemNameRole
    };
    Q_ENUM(SatelliteRoles)

    struct SatelliteItem {
        int id = 0;           // PRN satellite ID
        int rssi = 0;         // Signal strength (dBHz)
        int elevation = 0;     // Elevation in degrees
        int azimuth = 0;       // Azimuth in degrees
        int systemId = 0;      // 1=GPS, 2=GLONASS, 3=Galileo, 4=BeiDou
        bool inUse = false;   // Used in position solution
    };

    explicit SatelliteModel(QObject* parent = nullptr);

    static SatelliteModel* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
    static SatelliteModel* instance();
    static SatelliteModel* s_instance;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void updateSatellites(const QList<PGNParser::SatelliteInfo>& satellites,
                                      int gsvTotal, int gsvCurrent);
    Q_INVOKABLE void setSatellitesInUse(const QList<int>& inUse);
    Q_INVOKABLE void clear();

    int inUseCount() const;

signals:
    void sizeChanged();

private:
    QList<SatelliteItem> m_satellites;
    QSet<QString> m_inUseIds;
    QSet<QString> m_allIds;

    int m_gsvTotalMessages = 0;
    int m_gsvExpectedCount = 0;
    int m_lastGsvMessage = 0;  // Track last GSV message number for cycle detection

    static QString getUid(const SatelliteItem& sat);
    QString systemName(int systemId) const;
};

#endif // SATELLITEMODEL_H
