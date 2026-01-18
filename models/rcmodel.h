// RCModel.h
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
        ActualRateRole
    };
    Q_ENUM(Roles)

    struct Product {
        int id;
        QString name;
        double setRate;     // Установленная норма
        double smoothRate;  // Фактическая усредненная норма
        double actualRate;  // Фактическая норма без фильтров
    };

    explicit RCModel(QObject *parent = nullptr);

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Data management
    void setProducts(const QVector<Product> &products);
    void addProduct(const Product &product);
    void removeProduct(int id);
    void clear();

    // Methods to change rates
    Q_INVOKABLE void increaseSetRate(int id, double step = 10);
    Q_INVOKABLE void decreaseSetRate(int id, double step = 10);

    // Update methods
    void updateSmoothRate(int id, double newRate);
    void updateActualRate(int id, double newRate);
    void updateSetRate(int id, double newRate);
    void updateName(int id, const QString &name);

    // Utility
    int count() const { return m_products.count(); }
    Product getProduct(int id) const;
    bool productExists(int id) const;

signals:
    void countChanged();
    void productRateChanged(int id, double newSetRate);

private:
    QVector<Product> m_products;

    // Helper method to find product index by id
    int findProductIndex(int id) const;
};

#endif // RCMODEL_H
