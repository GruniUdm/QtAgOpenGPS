#include "blockage.h"

Blockage::Blockage(QObject *parent)
    : QObject{parent}
{}

void Blockage::statistics(const double speed){
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


        blockage_avg=0;
        for(int i = 0; i < numRows; i++) blockage_avg += (blockageseccount[i]); // среднее значение семян на гектар
        blockage_avg /= numRows;
        blockage_max = 0;
        blockage_max_i=0;
        for (int i = 0; i < numRows; ++i) {
            if (blockageseccount[i] > blockage_max) {
                blockage_max = (blockageseccount[i]); // максимальное количество семян на гектар на сошнике
                blockage_max_i = i; // номер сошника на ктором максимальное значение
            }
        }
        blockage_min1 = 65535;
        blockage_min2 = 65535;
        blockage_min1_i = 0;
        blockage_min2_i = 0;

        for (int i = 0; i < numRows; ++i) {
            if (blockageseccount[i] < blockage_min1) {
                blockage_min1 = (blockageseccount[i]);
                blockage_min1_i = i;
            }
        }
        for(int i = 0; i < numRows; i++)
            if(blockageseccount[i] < blockage_min2 && blockage_min1_i != i) // минимальные значения на гектар и номер сошника
            {
                blockage_min2 =(blockageseccount[i]);
                blockage_min2_i = i;
            }
        blockage_blocked=0;
        for (int i = 0; i < numRows; i++)
            if (blockageseccount[i] < blockCountMin) blockage_blocked++; // количество забитых сошников
    }
}
