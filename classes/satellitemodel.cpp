// Copyright (C) 2024 QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// SatelliteModel - QML model for displaying satellite information

#include "satellitemodel.h"
#include "agioservice.h"
#include <QQmlEngine>
#include <QJSEngine>
#include <QMutex>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(satellite, "satellite")

using namespace Qt::StringLiterals;

SatelliteModel::SatelliteModel(QObject* parent)
    : QAbstractListModel{parent}
{
}

SatelliteModel* SatelliteModel::s_instance = nullptr;

SatelliteModel* SatelliteModel::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)
    if (!s_instance) {
        s_instance = new SatelliteModel(nullptr);
    }
    return s_instance;
}

SatelliteModel* SatelliteModel::instance()
{
    return s_instance;
}

int SatelliteModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(m_satellites.size());
}

QVariant SatelliteModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();

    const SatelliteItem& sat = m_satellites.at(index.row());
    switch (role) {
    case IdRole:
        return sat.id;
    case RssiRole:
        return sat.rssi;
    case ElevationRole:
        return sat.elevation;
    case AzimuthRole:
        return sat.azimuth;
    case SystemIdRole:
        return sat.systemId;
    case SystemNameRole:
        return systemName(sat.systemId);
    case InUseRole:
        return sat.inUse;
    }

    return QVariant();
}

QHash<int, QByteArray> SatelliteModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {RssiRole, "rssi"},
        {ElevationRole, "elevation"},
        {AzimuthRole, "azimuth"},
        {SystemIdRole, "systemId"},
        {SystemNameRole, "system"},
        {InUseRole, "inUse"}
    };
}

QString SatelliteModel::systemName(int systemId) const
{
    switch (systemId) {
    case 1: return u"GPS"_s;
    case 2: return u"GLONASS"_s;
    case 3: return u"Galileo"_s;
    case 4: return u"BeiDou"_s;
    default: return u"Unknown"_s;
    }
}

void SatelliteModel::updateSatellites(const QList<PGNParser::SatelliteInfo>& satellites,
                                       int gsvTotal, int gsvCurrent)
{
    if (satellites.isEmpty()) {
        return;
    }

    qCDebug(satellite) << "📡 updateSatellites" << satellites.size() << "sats, gsvTotal" << gsvTotal << "current" << gsvCurrent
                        << "| total in model:" << m_satellites.size() << "| inUseIds:" << m_inUseIds.size();

    // First message - clear old data
    if (gsvCurrent == 1) {
        if (!m_satellites.isEmpty()) {
            qCDebug(satellite) << "🗑️ Clearing old satellite data:" << m_satellites.size();
            beginResetModel();
            m_satellites.clear();
            m_allIds.clear();
            // DON'T clear m_inUseIds here - GSA might come later with the same cycle data
            endResetModel();
        }
        m_gsvTotalMessages = gsvTotal;
    }

    // Add satellites from this GSV message
    for (const PGNParser::SatelliteInfo& satInfo : satellites) {
        SatelliteItem sat;
        sat.id = satInfo.prn;
        sat.rssi = satInfo.snr;
        sat.elevation = satInfo.elevation;
        sat.azimuth = satInfo.azimuth;
        sat.systemId = satInfo.systemId;
        sat.inUse = m_inUseIds.contains(getUid(sat));

        auto uid = getUid(sat);
        if (m_allIds.contains(uid)) {
            // Update existing satellite
            for (int i = 0; i < m_satellites.size(); ++i) {
                if (getUid(m_satellites.at(i)) == uid) {
                    m_satellites[i] = sat;
                    emit dataChanged(index(i), index(i), {RssiRole, ElevationRole, AzimuthRole, InUseRole});
                    break;
                }
            }
        } else {
            // Add new satellite
            beginInsertRows(QModelIndex(), m_satellites.size(), m_satellites.size());
            m_satellites.append(sat);
            m_allIds.insert(uid);
            endInsertRows();
            qCDebug(satellite) << "  + Added sat:" << satInfo.prn << "systemId:" << satInfo.systemId << "snr:" << satInfo.snr;
        }
    }

    qCDebug(satellite) << "✅ SatelliteModel updated: total =" << m_satellites.size() << "inUse =" << inUseCount();
    emit sizeChanged();
}

void SatelliteModel::setSatellitesInUse(const QList<int>& inUse)
{
    qCDebug(satellite) << "📡 setSatellitesInUse:" << inUse << "| current satellites:" << m_satellites.size();

    // Clear and rebuild inUseIds
    m_inUseIds.clear();
    for (int prn : inUse) {
        // For GSA, we don't know the system - try all systems
        m_inUseIds.insert(QString("1-%1").arg(prn));   // GPS
        m_inUseIds.insert(QString("2-%1").arg(prn));   // GLONASS
        m_inUseIds.insert(QString("3-%1").arg(prn));   // Galileo
        m_inUseIds.insert(QString("4-%1").arg(prn));   // BeiDou
    }

    qCDebug(satellite) << "   m_inUseIds now has:" << m_inUseIds.size() << "entries";

    // Update inUse status for ALL satellites
    bool changed = false;
    for (int i = 0; i < m_satellites.size(); ++i) {
        bool newInUse = m_inUseIds.contains(getUid(m_satellites.at(i)));
        if (m_satellites[i].inUse != newInUse) {
            m_satellites[i].inUse = newInUse;
            changed = true;
            emit dataChanged(index(i), index(i), {InUseRole});
        }
    }

    if (changed || !inUse.isEmpty()) {
        qCDebug(satellite) << "✅ SatelliteModel inUse updated:" << inUseCount() << "sats in solution";
        emit sizeChanged();
    }
}

void SatelliteModel::clear()
{
    if (!m_satellites.isEmpty()) {
        beginResetModel();
        m_satellites.clear();
        m_allIds.clear();
        m_inUseIds.clear();
        endResetModel();
        emit sizeChanged();
    }
}

int SatelliteModel::inUseCount() const
{
    int count = 0;
    for (const SatelliteItem& sat : m_satellites) {
        if (sat.inUse) ++count;
    }
    return count;
}

QString SatelliteModel::getUid(const SatelliteItem& sat)
{
    return QString("%1-%2").arg(sat.systemId).arg(sat.id);
}
