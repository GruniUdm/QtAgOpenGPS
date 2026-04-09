#include "screenbrightness.h"

#include <QProcess>
#include <QFile>
#include <QRegularExpression>
#include <QtGlobal>
#include <QLoggingCategory>

#if defined(Q_OS_WINDOWS)
#include <windows.h>
#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")
#endif

#if defined(Q_OS_ANDROID) || defined(__ANDROID__)
#include <QtCore/qnativeinterface.h>
#endif


bool ScreenBrightness::setBrightness(int percent)
{
    percent = qBound(5, percent, 100);

#if defined(Q_OS_ANDROID) || defined(__ANDROID__)
    return setBrightnessAndroid(percent);
#elif defined(Q_OS_LINUX)
    return setBrightnessLinux(percent);
#elif defined(Q_OS_WINDOWS)
    return setBrightnessWindows(percent);
#else
    Q_UNUSED(percent);
    return false;
#endif
}

int ScreenBrightness::getBrightness()
{
#if defined(Q_OS_ANDROID) || defined(__ANDROID__)
    return getBrightnessAndroid();
#elif defined(Q_OS_LINUX)
    return getBrightnessLinux();
#elif defined(Q_OS_WINDOWS)
    return getBrightnessWindows();
#else
    return -1;
#endif
}

#if defined(Q_OS_WINDOWS)

bool ScreenBrightness::setBrightnessWindows(int percent)
{
    QProcess process;
    QString script = QString(
                         "powershell -Command \"(Get-WmiObject -Namespace root\\WMI -Class "
                         "WmiMonitorBrightnessMethods).WmiSetBrightness(1,%1)\"")
                         .arg(percent);
    process.setProgram("cmd");
    process.setArguments({"/c", script});
    process.start();
    bool finished = process.waitForFinished(3000);
    if (!finished) {
        process.kill();
        process.waitForFinished();
    }
    return finished && process.exitCode() == 0;
}

int ScreenBrightness::getBrightnessWindows()
{
    QProcess process;
    process.setProgram("cmd");
    process.setArguments({"/c", "powershell -Command \"(Get-WmiObject -Namespace root\\WMI -Class WmiMonitorBrightness).CurrentBrightness\""});
    process.start();
    bool finished = process.waitForFinished(3000);
    if (!finished) {
        process.kill();
        process.waitForFinished();
    }

    if (finished && process.exitCode() == 0) {
        QString output = process.readAllStandardOutput().trimmed();
        bool ok = false;
        int brightness = output.toInt(&ok);
        if (ok) {
            return brightness * 100 / 255;
        }
    }
    return 40;
}

#elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID) && !defined(__ANDROID__)

bool ScreenBrightness::setBrightnessLinux(int percent)
{
    QString brightnessFile;
    QStringList candidates = {
        "/sys/class/backlight/amdgpu_bl0/brightness",
        "/sys/class/backlight/amdgpu_bl1/brightness",
        "/sys/class/backlight/intel_backlight/brightness",
        "/sys/class/backlight/radeon_bl0/brightness",
        "/sys/class/backlight/acpi_video0/brightness",
    };

    for (const QString &candidate : candidates) {
        QFile testFile(candidate);
        if (testFile.exists() && testFile.open(QIODevice::WriteOnly)) {
            testFile.close();
            brightnessFile = candidate;
            break;
        }
    }

    if (brightnessFile.isEmpty()) {
        QProcess process;
        process.setProgram("xrandr");
        process.setArguments({"--verbose"});
        process.start();
        process.waitForFinished(2000);

        if (process.exitCode() == 0) {
            QString output = process.readAllStandardOutput();
            QRegularExpression re("(\\S+)\\s+connected[^:]*:.*?\\s+Brightness:\\s+([0-9.]+)",
                               QRegularExpression::DotMatchesEverythingOption);
            QRegularExpressionMatch match = re.match(output);
            if (match.hasMatch()) {
                QString display = match.captured(1);
                float newBrightness = percent / 100.0;
                QProcess processXrandr;
                processXrandr.setProgram("xrandr");
                processXrandr.setArguments({"--output", display, "--brightness",
                                           QString::number(newBrightness, 'f', 2)});
                processXrandr.start();
                processXrandr.waitForFinished(2000);
                return processXrandr.exitCode() == 0;
            }
        }

        QProcess processDdc;
        processDdc.setProgram("ddcutil");
        processDdc.setArguments({"setvcp", "10", QString::number(percent)});
        processDdc.start();
        processDdc.waitForFinished(2000);
        return processDdc.exitCode() == 0;
    }

    QFile file(brightnessFile);
    if (file.open(QIODevice::WriteOnly)) {
        QString maxFile = brightnessFile;
        maxFile.replace("brightness", "max_brightness");
        QFile maxF(maxFile);
        int maxBrightness = 255;
        if (maxF.open(QIODevice::ReadOnly)) {
            maxBrightness = QString::fromUtf8(maxF.readAll()).trimmed().toInt();
            if (maxBrightness <= 0) maxBrightness = 255;
            maxF.close();
        }

        int value = percent * maxBrightness / 100;
        QTextStream stream(&file);
        stream << value << "\n";
        file.close();
        return true;
    } else {

    }

    return false;
}

int ScreenBrightness::getBrightnessLinux()
{
    QString brightnessFile;
    QStringList candidates = {
        "/sys/class/backlight/amdgpu_bl0/brightness",
        "/sys/class/backlight/amdgpu_bl1/brightness",
        "/sys/class/backlight/intel_backlight/brightness",
        "/sys/class/backlight/radeon_bl0/brightness",
        "/sys/class/backlight/acpi_video0/brightness",
    };

    for (const QString &candidate : candidates) {
        if (QFile::exists(candidate)) {
            brightnessFile = candidate;
            break;
        }
    }

    if (!brightnessFile.isEmpty()) {
        QFile file(brightnessFile);
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream stream(&file);
            int value = stream.readAll().trimmed().toInt();
            file.close();

            QString maxFile = brightnessFile;
            maxFile.replace("brightness", "max_brightness");
            QFile maxF(maxFile);
            int maxBrightness = 255;
            if (maxF.open(QIODevice::ReadOnly)) {
                maxBrightness = QString::fromUtf8(maxF.readAll()).trimmed().toInt();
                if (maxBrightness <= 0) maxBrightness = 255;
                maxF.close();
            }

            return value * 100 / maxBrightness;
        }
    }

    QProcess process;
    process.setProgram("xrandr");
    process.setArguments({"--verbose"});
    process.start();
    process.waitForFinished(2000);

    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput();
        QRegularExpression re("(\\S+)\\s+connected[^:]*:.*?\\s+Brightness:\\s+([0-9.]+)",
                           QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatch match = re.match(output);
        if (match.hasMatch()) {
            return qRound(match.captured(2).toFloat() * 100);
        }
    }

    return 40;
}

#elif defined(Q_OS_ANDROID) || defined(__ANDROID__)

bool ScreenBrightness::setBrightnessAndroid(int percent)
{
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    if (!activity.isValid()) {
        return false;
    }

    QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
    if (!window.isValid()) {
        return false;
    }

    QJniObject layoutParams = window.callObjectMethod(
        "getAttributes", "()Landroid/view/WindowManager$LayoutParams;");

    float brightnessValue = percent / 100.0f;
    layoutParams.setField<float>("screenBrightness", brightnessValue);

    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([window, layoutParams]() {
        window.callMethod<void>("setAttributes",
                                "(Landroid/view/WindowManager$LayoutParams;)V",
                                layoutParams.object());
    });

    return true;
}

int ScreenBrightness::getBrightnessAndroid()
{
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    if (!activity.isValid())
        return 40;

    QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
    if (!window.isValid())
        return 40;

    QJniObject layoutParams = window.callObjectMethod(
        "getAttributes", "()Landroid/view/WindowManager$LayoutParams;");

    float brightness = layoutParams.getField<float>("screenBrightness");

    if (brightness <= 0) {
        QJniObject contentResolver = activity.callObjectMethod(
            "getContentResolver", "()Landroid/content/ContentResolver;");
        QJniObject settings = QJniObject::callStaticObjectMethod(
            "android/provider/Settings$System",
            "getInt",
            "(Landroid/content/ContentResolver;Ljava/lang/String;)I",
            contentResolver.object(),
            QJniObject::fromString("screen_brightness").object());
        jint val = settings.callMethod<jint>("intValue", "()I");
        brightness = val * 100 / 255.0f;
    }

    return static_cast<int>(brightness);
}

#endif
