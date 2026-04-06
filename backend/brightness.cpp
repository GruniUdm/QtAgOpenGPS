#include "backend.h"
#include "settingsmanager.h"
#include "screenbrightness.h"

void Backend::brightnessUp()
{
    int b = SettingsManager::instance()->display_brightness();
    if (b < 100) {
        b = qMin(100, b + 10);
        SettingsManager::instance()->setDisplay_brightness(b);
        ScreenBrightness::setBrightness(b);
    }
}

void Backend::brightnessDown()
{
    int b = SettingsManager::instance()->display_brightness();
    if (b > 10) {
        b = qMax(10, b - 10);
        SettingsManager::instance()->setDisplay_brightness(b);
        ScreenBrightness::setBrightness(b);
    }
}
