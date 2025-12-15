#include "qmlblockage.h"

qmlblockage::qmlblockage(QObject *parent)
    : QObject{parent}
{}

void qmlblockage::statistics(const double speed){
    // Phase 6.0.20: FormGPS context available via 'this' - no qmlItem() needed
    int k = 0;
    int numRows1 = SettingsManager::instance()->seed_blockRow1();
    int numRows2 = SettingsManager::instance()->seed_blockRow2();
    int numRows3 = SettingsManager::instance()->seed_blockRow3();
    int numRows4 = SettingsManager::instance()->seed_blockRow4();
    int numRows = SettingsManager::instance()->seed_numRows();
    int blockCountMin = SettingsManager::instance()->seed_blockCountMin();
    double toolWidth = SettingsManager::instance()->vehicle_toolWidth();
    double rowwidth = toolWidth / numRows;
    //int vtgSpeed = 5;
    if (speed != 0 && rowwidth != 0) {
        for (int i = 0; i < numRows1 && i < (sizeof(blockageSecCount1) / sizeof(blockageSecCount1[0])); i++)
            blockageseccount[k++] = floor(blockageSecCount1[i] * 7.2 / rowwidth / speed);
        for (int i = 0; i < numRows2 && i < (sizeof(blockageSecCount2) / sizeof(blockageSecCount2[0])); i++)
            blockageseccount[k++] = floor(blockageSecCount2[i] * 7.2 / rowwidth / speed);
        for (int i = 0; i < numRows3 && i < (sizeof(blockageSecCount3) / sizeof(blockageSecCount3[0])); i++)
            blockageseccount[k++] = floor(blockageSecCount3[i] * 7.2 / rowwidth / speed);
        for (int i = 0; i < numRows4 && i < (sizeof(blockageSecCount4) / sizeof(blockageSecCount4[0])); i++)
            blockageseccount[k++] = floor(blockageSecCount4[i] * 7.2 / rowwidth / speed);
    //}
    //if(QDateTime::currentMSecsSinceEpoch() - blockage_lastUpdate >= 3000){

    //set(blockageseccount, (sizeof(blockageseccount)/sizeof(blockageseccount[0])));

    //blockage_lastUpdate = QDateTime::currentMSecsSinceEpoch();

    blockage_avg = std::accumulate(std::begin(blockageseccount), std::end(blockageseccount), 0);
    blockage_avg = blockage_avg / numRows;

    blockage_max_i = 0;
    blockage_max_i = 0;
    for (int i = 0; i < numRows && i < (sizeof(blockageseccount) / sizeof(blockageseccount[0])); ++i) {
        if (blockageseccount[i] > blockage_max_i) {
            blockage_max = (blockageseccount[i]);
            blockage_max_i = i+1;
        }
    }
    blockage_min1 = 65535;
    blockage_min2 = 65535;
    blockage_min1_i = 0;
    blockage_min2_i = 0;

    for (int i = 0; i < numRows && i < (sizeof(blockageseccount) / sizeof(blockageseccount[0])); ++i) {
        if (blockageseccount[i] < blockage_min1) {
            blockage_min1 = (blockageseccount[i]);
            blockage_min1_i = i+1;
        }
    }
    for (int i = 0; i < numRows && i < (sizeof(blockageseccount) / sizeof(blockageseccount[0])); i++)
        if (blockageseccount[i] < blockage_min2 && blockage_min1 != i) {
            blockage_min2 = (blockageseccount[i]);
            blockage_min2_i = i+1;
        }
    blockage_blocked = 0;
    for (int i = 0; i < numRows && i < (sizeof(blockageseccount) / sizeof(blockageseccount[0])); i++)
        if (blockageseccount[i] < blockCountMin)
            blockage_blocked = blockage_blocked+1;
    }
}
