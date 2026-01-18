// RCModel.cpp
#include "rcmodel.h"
#include <QDebug>

RCModel::RCModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int RCModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_products.count();
}

QVariant RCModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_products.count())
        return QVariant();

    const Product &product = m_products[index.row()];

    switch (role) {
    case IdRole:
        return product.id;
    case NameRole:
        return product.name;
    case SetRateRole:
        return product.setRate;
    case SmoothRateRole:
        return product.smoothRate;
    case ActualRateRole:
        return product.actualRate;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> RCModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "productId";
    roles[NameRole] = "productName";
    roles[SetRateRole] = "productSetRate";
    roles[SmoothRateRole] = "productSmoothRate";
    roles[ActualRateRole] = "productActualRate";
    return roles;
}

void RCModel::setProducts(const QVector<Product> &products)
{
    beginResetModel();
    m_products = products;
    endResetModel();
    emit countChanged();
}

void RCModel::addProduct(const Product &product)
{
    // Проверяем, существует ли уже продукт с таким id
    if (productExists(product.id)) {
        qWarning() << "Product with id" << product.id << "already exists";
        return;
    }

    beginInsertRows(QModelIndex(), m_products.count(), m_products.count());
    m_products.append(product);
    endInsertRows();
    emit countChanged();
}

void RCModel::removeProduct(int id)
{
    int index = findProductIndex(id);
    if (index == -1) return;

    beginRemoveRows(QModelIndex(), index, index);
    m_products.remove(index);
    endRemoveRows();
    emit countChanged();
}

void RCModel::clear()
{
    beginResetModel();
    m_products.clear();
    endResetModel();
    emit countChanged();
}

void RCModel::increaseSetRate(int id, double step)
{
    int index = findProductIndex(id);
    if (index == -1) return;

    m_products[index].setRate += step;

    // Уведомляем об изменении данных
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {SetRateRole});

    emit productRateChanged(id, m_products[index].setRate);
}

void RCModel::decreaseSetRate(int id, double step)
{
    int index = findProductIndex(id);
    if (index == -1) return;

    // Проверяем, чтобы значение не стало отрицательным
    double newRate = m_products[index].setRate - step;
    if (newRate < 0) newRate = 0;

    m_products[index].setRate = newRate;

    // Уведомляем об изменении данных
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {SetRateRole});

    emit productRateChanged(id, newRate);
}

void RCModel::updateSmoothRate(int id, double newRate)
{
    int index = findProductIndex(id);
    if (index == -1) return;

    m_products[index].smoothRate = newRate;

    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {SmoothRateRole});
}

void RCModel::updateActualRate(int id, double newRate)
{
    int index = findProductIndex(id);
    if (index == -1) return;

    m_products[index].actualRate = newRate;

    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {ActualRateRole});
}

void RCModel::updateSetRate(int id, double newRate)
{
    int index = findProductIndex(id);
    if (index == -1) return;

    m_products[index].setRate = newRate;

    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {SetRateRole});

    emit productRateChanged(id, newRate);
}

void RCModel::updateName(int id, const QString &name)
{
    int index = findProductIndex(id);
    if (index == -1) return;

    m_products[index].name = name;

    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {NameRole});
}

RCModel::Product RCModel::getProduct(int id) const
{
    int index = findProductIndex(id);
    if (index == -1) return Product();

    return m_products[index];
}

bool RCModel::productExists(int id) const
{
    return findProductIndex(id) != -1;
}

int RCModel::findProductIndex(int id) const
{
    for (int i = 0; i < m_products.count(); ++i) {
        if (m_products[i].id == id) {
            return i;
        }
    }
    return -1;
}
