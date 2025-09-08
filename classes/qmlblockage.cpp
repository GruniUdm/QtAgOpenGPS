#include "qmlblockage.h"

qmlblockage::qmlblockage(QObject *parent)
    : QObject{parent}
{}

void qmlblockage::set(double *new_state, int size) {

    if (aog_root->property("blockageRowCount").isValid()) {
        QMutexLocker locker(&mutex);
        QVariantList rows;
        double row1 = 0.1;
        rows.clear();
        for (int i = 0; i < size; i++) {
            rows << QVariant(new_state[i]+row1);
        }
        aog_root->setProperty("blockageRowCount", rows);
    }
}
