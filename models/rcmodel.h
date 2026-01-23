#ifndef RCMODEL_H
#define RCMODEL_H

#include <QAbstractListModel>
#include <QVector>

class RCModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        SetRateRole,
        SmoothRateRole,
        ActualRateRole,
        IsActiveRole
    };
    Q_ENUM(Roles)

    struct Product {
        int id; // индекс продукта (0-3)
        QString name;
        double setRate;
        double smoothRate;
        double actualRate;
        bool isActive;
    };

    explicit RCModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Методы для доступа по индексу
    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE int getProductId(int index) const;

    // Data management
    void setProducts(const QVector<Product> &products);
    void addProduct(const Product &product);
    void clear();

    // Методы изменения данных по индексу
    Q_INVOKABLE void increaseSetRate(int index, double step = 10);
    Q_INVOKABLE void decreaseSetRate(int index, double step = 10);
    void updateSmoothRate(int index, double newRate);
    void updateActualRate(int index, double newRate);
    void updateSetRate(int index, double newRate);
    void updateName(int index, const QString &name);
    void updateIsActive(int index, bool isActive);

    // Utility
    int count() const { return m_products.count(); }
    bool isValidIndex(int index) const { return index >= 0 && index < m_products.count(); }

signals:
    void countChanged();
    void productRateChanged(int index, double newSetRate);

private:
    QVector<Product> m_products;
};

#endif // RCMODEL_H
