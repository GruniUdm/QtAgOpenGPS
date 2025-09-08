#include "cboundary.h"
#include "settings.h"

CBoundary::CBoundary(QObject *parent) : QObject(parent)
{
    turnSelected = 0;
}

void CBoundary::loadSettings() {
    isSectionControlledByHeadland = settings->value(SETTINGS_headland_isSectionControlled).value<bool>();
}
