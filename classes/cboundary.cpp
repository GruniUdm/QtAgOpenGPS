#include "cboundary.h"
#include "classes/settingsmanager.h"

CBoundary::CBoundary(QObject *parent) : QObject(parent)
{
    turnSelected = 0;
}

void CBoundary::loadSettings() {
    isSectionControlledByHeadland = SettingsManager::instance()->value(SETTINGS_headland_isSectionControlled).value<bool>();
}
