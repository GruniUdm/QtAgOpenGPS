#include "ploughmodel.h"
#include <QDebug>

PloughModel::PloughModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // Инициализация одним плугом по умолчанию
    PloughData defaultPlough;
    defaultPlough.id = 0;
    defaultPlough.currentWidth = 0;
    defaultPlough.targetWidth = 200;
    defaultPlough.ploughMode = 0;
    defaultPlough.ploughModeName = getModeName(0);
    defaultPlough.ploughEnabled = false;
    defaultPlough.autoWidthEnabled = false;
    defaultPlough.autoLiftEnabled = false;
    defaultPlough.isCalibrated = false;
    defaultPlough.minWidth = 0;
    defaultPlough.maxWidth = 400;
    defaultPlough.deadBand = 10;
    defaultPlough.sectionOn = false;
    defaultPlough.lineDistance = 0;
    defaultPlough.error = 0;
    defaultPlough.command = 0;

    m_ploughs.append(defaultPlough);
}

int PloughModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_ploughs.count();
}

QVariant PloughModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_ploughs.count())
        return QVariant();

    const PloughData &plough = m_ploughs[index.row()];

    switch (role) {
    case IdRole:
        return plough.id;
    case CurrentWidthRole:
        return plough.currentWidth;
    case TargetWidthRole:
        return plough.targetWidth;
    case PloughModeRole:
        return plough.ploughMode;
    case PloughModeNameRole:
        return plough.ploughModeName;
    case PloughEnabledRole:
        return plough.ploughEnabled;
    case AutoWidthEnabledRole:
        return plough.autoWidthEnabled;
    case AutoLiftEnabledRole:
        return plough.autoLiftEnabled;
    case IsCalibratedRole:
        return plough.isCalibrated;
    case MinWidthRole:
        return plough.minWidth;
    case MaxWidthRole:
        return plough.maxWidth;
    case DeadBandRole:
        return plough.deadBand;
    case SectionOnRole:
        return plough.sectionOn;
    case LineDistanceRole:
        return plough.lineDistance;
    case ErrorRole:
        return plough.error;
    case CommandRole:
        return plough.command;
    default:
        return QVariant();
    }
}

QVariantMap PloughModel::get(int index) const
{
    QVariantMap map;
    if (index >= 0 && index < m_ploughs.count()) {
        const PloughData &plough = m_ploughs[index];
        map["ploughId"] = plough.id;
        map["currentWidth"] = plough.currentWidth;
        map["targetWidth"] = plough.targetWidth;
        map["ploughMode"] = plough.ploughMode;
        map["ploughModeName"] = plough.ploughModeName;
        map["ploughEnabled"] = plough.ploughEnabled;
        map["autoWidthEnabled"] = plough.autoWidthEnabled;
        map["autoLiftEnabled"] = plough.autoLiftEnabled;
        map["isCalibrated"] = plough.isCalibrated;
        map["minWidth"] = plough.minWidth;
        map["maxWidth"] = plough.maxWidth;
        map["deadBand"] = plough.deadBand;
        map["sectionOn"] = plough.sectionOn;
        map["lineDistance"] = plough.lineDistance;
        map["error"] = plough.error;
        map["command"] = plough.command;
    }
    return map;
}

QHash<int, QByteArray> PloughModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "ploughId";
    roles[CurrentWidthRole] = "currentWidth";
    roles[TargetWidthRole] = "targetWidth";
    roles[PloughModeRole] = "ploughMode";
    roles[PloughModeNameRole] = "ploughModeName";
    roles[PloughEnabledRole] = "ploughEnabled";
    roles[AutoWidthEnabledRole] = "autoWidthEnabled";
    roles[AutoLiftEnabledRole] = "autoLiftEnabled";
    roles[IsCalibratedRole] = "isCalibrated";
    roles[MinWidthRole] = "minWidth";
    roles[MaxWidthRole] = "maxWidth";
    roles[DeadBandRole] = "deadBand";
    roles[SectionOnRole] = "sectionOn";
    roles[LineDistanceRole] = "lineDistance";
    roles[ErrorRole] = "error";
    roles[CommandRole] = "command";
    return roles;
}

int PloughModel::getPloughId(int index) const
{
    if (index >= 0 && index < m_ploughs.count()) {
        return m_ploughs[index].id;
    }
    return -1;
}

void PloughModel::setPloughs(const QVector<PloughData> &ploughs)
{
    beginResetModel();
    m_ploughs = ploughs;
    endResetModel();
    emit countChanged();
}

void PloughModel::addPlough(const PloughData &plough)
{
    beginInsertRows(QModelIndex(), m_ploughs.count(), m_ploughs.count());
    m_ploughs.append(plough);
    endInsertRows();
    emit countChanged();
}

void PloughModel::clear()
{
    beginResetModel();
    m_ploughs.clear();
    endResetModel();
    emit countChanged();
}

void PloughModel::updateCurrentWidth(int index, int width)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    if (m_ploughs[index].currentWidth != width) {
        m_ploughs[index].currentWidth = width;
        // Обновляем ошибку
        m_ploughs[index].error = width - m_ploughs[index].targetWidth;

        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {CurrentWidthRole, ErrorRole});
        emit currentWidthChanged(index, width);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updateTargetWidth(int index, int width)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    if (m_ploughs[index].targetWidth != width) {
        m_ploughs[index].targetWidth = width;
        // Обновляем ошибку
        m_ploughs[index].error = m_ploughs[index].currentWidth - width;

        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {TargetWidthRole, ErrorRole});
        emit targetWidthChanged(index, width);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updatePloughMode(int index, int mode, const QString &modeName)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    if (m_ploughs[index].ploughMode != mode ||
        m_ploughs[index].ploughModeName != modeName) {

        m_ploughs[index].ploughMode = mode;
        m_ploughs[index].ploughModeName = modeName.isEmpty() ? getModeName(mode) : modeName;

        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {PloughModeRole, PloughModeNameRole});
        emit ploughModeChanged(index, mode, m_ploughs[index].ploughModeName);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updatePloughEnabled(int index, bool enabled)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    if (m_ploughs[index].ploughEnabled != enabled) {
        m_ploughs[index].ploughEnabled = enabled;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {PloughEnabledRole});
        emit ploughEnabledChanged(index, enabled);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updateAutoWidthEnabled(int index, bool enabled)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    if (m_ploughs[index].autoWidthEnabled != enabled) {
        m_ploughs[index].autoWidthEnabled = enabled;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {AutoWidthEnabledRole});
        emit autoWidthEnabledChanged(index, enabled);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updateAutoLiftEnabled(int index, bool enabled)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    if (m_ploughs[index].autoLiftEnabled != enabled) {
        m_ploughs[index].autoLiftEnabled = enabled;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {AutoLiftEnabledRole});
        emit autoLiftEnabledChanged(index, enabled);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updateCalibration(int index, bool calibrated, int minWidth, int maxWidth)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    bool changed = false;

    if (m_ploughs[index].isCalibrated != calibrated) {
        m_ploughs[index].isCalibrated = calibrated;
        changed = true;
    }
    if (m_ploughs[index].minWidth != minWidth) {
        m_ploughs[index].minWidth = minWidth;
        changed = true;
    }
    if (m_ploughs[index].maxWidth != maxWidth) {
        m_ploughs[index].maxWidth = maxWidth;
        changed = true;
    }

    if (changed) {
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {IsCalibratedRole, MinWidthRole, MaxWidthRole});
        emit calibrationChanged(index, calibrated, minWidth, maxWidth);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updateDeadBand(int index, int deadBand)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    if (m_ploughs[index].deadBand != deadBand) {
        m_ploughs[index].deadBand = deadBand;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {DeadBandRole});
        emit deadBandChanged(index, deadBand);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updateSectionState(int index, bool sectionOn)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    if (m_ploughs[index].sectionOn != sectionOn) {
        m_ploughs[index].sectionOn = sectionOn;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {SectionOnRole});
        emit sectionStateChanged(index, sectionOn);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updateLineDistance(int index, int distance)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    if (m_ploughs[index].lineDistance != distance) {
        m_ploughs[index].lineDistance = distance;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {LineDistanceRole});
        emit lineDistanceChanged(index, distance);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updateError(int index, int error)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    if (m_ploughs[index].error != error) {
        m_ploughs[index].error = error;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {ErrorRole});
        emit errorChanged(index, error);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updateCommand(int index, int command)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    if (m_ploughs[index].command != command) {
        m_ploughs[index].command = command;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {CommandRole});
        emit commandChanged(index, command);
        emit ploughDataUpdated(index);
    }
}

void PloughModel::updateAllData(int index, const PloughData &data)
{
    if (index < 0 || index >= m_ploughs.count()) return;

    m_ploughs[index] = data;
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex);
    emit ploughDataUpdated(index);
}

PloughModel::PloughData PloughModel::ploughAt(int index) const
{
    if (index >= 0 && index < m_ploughs.count()) {
        return m_ploughs[index];
    }
    return PloughData();
}

QString PloughModel::getModeName(int mode) const
{
    switch (mode) {
    case 0: return tr("Секция Выкл");
    case 1: return tr("Автоконфигурация Выкл");
    case 2: return tr("Автопереключение Выкл");
    case 3: return tr("Фиксированный");
    case 4: return tr("Расширение");
    case 5: return tr("Максимум");
    case 6: return tr("Сужение");
    case 7: return tr("Минимум");
    case 8: return tr("Нет линии");
    default: return tr("Неизвестно");
    }
}
