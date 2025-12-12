#ifndef QMLBLOCKAGE_H
#define QMLBLOCKAGE_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include "cmodulecomm.h"

class qmlblockage : public QObject
{
    Q_OBJECT
    QMutex mutex;
protected:
    //QVariantList rows;
public:
    explicit qmlblockage(QObject *parent = nullptr);

    static inline QObject *aog_root = NULL;
    static inline void set_aog_root(QObject *aog_root_) { aog_root = aog_root_;}
    void set(const QVector<QVector<double>> &new_state);

public slots:
    //void set (double *new_state, int size);
};
#endif // QMLBLOCKAGE_H
