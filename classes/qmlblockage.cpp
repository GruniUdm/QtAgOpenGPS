#include "qmlblockage.h"

qmlblockage::qmlblockage(QObject *parent)
    : QObject{parent}
{}

void qmlblockage::set(const QVector<QVector<double>> &new_state) {
    if (aog_root && aog_root->property("blockageRowCount").isValid()) {
        QMutexLocker locker(&mutex);
        QVariantList rows;
        double row1 = 0.1;
        rows.clear();

        // Объединяем все вложенные векторы в один плоский список
        for (const auto &innerVec : new_state) {
            for (double value : innerVec) {
                rows << QVariant(value + row1);
            }
        }

        // CRITICAL: Force QML update in GUI thread to prevent threading violation
        QMetaObject::invokeMethod(aog_root, "setProperty", Qt::QueuedConnection,
                                  Q_ARG(QString, "blockageRowCount"), Q_ARG(QVariant, QVariant(rows)));
    }
}
