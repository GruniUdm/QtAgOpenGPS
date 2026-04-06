#include "brightnesscontroller.h"
#include "screenbrightness.h"
#include "settingsmanager.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(brightness_log, "brightness.controller")

void BrightnessController::brightnessUp()
{
    int b = SettingsManager::instance()->display_brightness();
    if (b < 100) {
        b = qMin(100, b + 10);
        SettingsManager::instance()->setDisplay_brightness(b);
        ScreenBrightness::setBrightness(b);
        emit brightnessChanged(b);
    }
}

void BrightnessController::brightnessDown()
{
    int b = SettingsManager::instance()->display_brightness();
    if (b > 10) {
        b = qMax(10, b - 10);
        SettingsManager::instance()->setDisplay_brightness(b);
        ScreenBrightness::setBrightness(b);
        emit brightnessChanged(b);
    }
}
