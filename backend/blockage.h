#ifndef BLOCKAGE_H
#define BLOCKAGE_H

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include "cnmea.h"
#include "settingsmanager.h"

class Blockage : public QObject
{
    Q_OBJECT
    QMutex mutex;
protected:
    //QVariantList rows;
public:

    CNMEA pn;
    int blockageSecCount1[16] = {0};
    int blockageSecCount2[16] = {0};
    int blockageSecCount3[16] = {0};
    int blockageSecCount4[16] = {0};
    int blockageseccount[64] = {0};
    int blockage_avg=0;
    int blockage_min1=0;
    int blockage_min2=0;
    int blockage_max=0;
    int blockage_min=0;
    int blockage_min1_i=0;
    int blockage_min2_i=0;
    int blockage_max_i=0;
    int blockage_blocked=0;

    explicit Blockage(QObject *parent = 0);
    static inline QObject *aog_root = NULL;
    static inline void set_aog_root(QObject *aog_root_) { aog_root = aog_root_;}

private:

public slots:
void statistics(const double speed);
};
#endif // BLOCKAGE_H
