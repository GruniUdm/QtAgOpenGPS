#ifndef SCREENBRIGHTNESS_H
#define SCREENBRIGHTNESS_H

#include <QtGlobal>

#if defined(Q_OS_ANDROID) || defined(__ANDROID__)
#include <QJniObject>
#endif

class ScreenBrightness
{
public:
    static bool setBrightness(int percent);
    static int getBrightness();

private:
#if defined(Q_OS_WINDOWS)
    static bool setBrightnessWindows(int percent);
    static int getBrightnessWindows();
#elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID) && !defined(__ANDROID__)
    static bool setBrightnessLinux(int percent);
    static int getBrightnessLinux();
#elif defined(Q_OS_ANDROID) || defined(__ANDROID__)
    static bool setBrightnessAndroid(int percent);
    static int getBrightnessAndroid();
#endif
};

#endif // SCREENBRIGHTNESS_H
