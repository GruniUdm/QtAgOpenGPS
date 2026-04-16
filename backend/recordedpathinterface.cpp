#include "recordedpathinterface.h"
#include "recordedpath.h"
#include "recordedpathproperties.h"
#include "cvehicle.h"
#include "cyouturn.h"
#include "siminterface.h"
#include "backend.h"
#include "glm.h"
#include "settingsmanager.h"

#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>
#include <QLocale>
#include <QStringView>
#include <QDir>
#include <QStandardPaths>

RecordedPathInterface *RecordedPathInterface::s_instance = nullptr;
QMutex RecordedPathInterface::s_mutex;
bool RecordedPathInterface::s_cpp_created = false;

RecordedPathInterface::RecordedPathInterface(QObject *parent)
    : QObject{parent}
{
}

RecordedPathInterface *RecordedPathInterface::instance() {
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new RecordedPathInterface();
        s_cpp_created = true;
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         s_instance, []() {
                         delete s_instance; s_instance = nullptr;
                         });
    }
    return s_instance;
}

RecordedPathInterface *RecordedPathInterface::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if (!s_instance) {
        s_instance = new RecordedPathInterface();
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}

void RecordedPathInterface::recPathLoad()
{
    RecordedPath::instance()->updateInterface();
    // Set menu open state
    RecordedPath::instance()->m_recordedPathProperties->set_menuOpen(true);
}

void RecordedPathInterface::recPathClear()
{
    // Stop driving if active when menu is closed (matching expected behavior)
    if (RecordedPath::instance()->isDrivingRecordedPath()) {
        RecordedPath::instance()->StopDrivingRecordedPath();
        set_isDriving(false);
    }

    RecordedPath::instance()->recList.clear();
    RecordedPath::instance()->updateInterface();
    // Set menu closed state
    RecordedPath::instance()->m_recordedPathProperties->set_menuOpen(false);
}

void RecordedPathInterface::recPathFollowStop()
{
    if (RecordedPath::instance()->isDrivingRecordedPath())
    {
        RecordedPath::instance()->StopDrivingRecordedPath();
        set_isDriving(false);
    }
    else
    {
        CYouTurn *yt = qobject_cast<CYouTurn*>(Backend::instance()->yt());
        bool started = RecordedPath::instance()->StartDrivingRecordedPath(*CVehicle::instance(), *yt);
        set_isDriving(started);
    }
}

void RecordedPathInterface::recPathRecordStop()
{
    if (RecordedPath::instance()->isRecordOn)
    {
        RecordedPath::instance()->isRecordOn = false;
        emit recordStateChanged(false);
        emit showPathNewDialog();
    }
    else if (Backend::instance()->isJobStarted())
    {
        RecordedPath::instance()->recList.clear();
        RecordedPath::instance()->isRecordOn = true;
        emit recordStateChanged(true);
    }
}

void RecordedPathInterface::recPathResumeStyle()
{
    RecordedPath::instance()->resumeState++;
    if (RecordedPath::instance()->resumeState > 2)
        RecordedPath::instance()->resumeState = 0;

    set_resumeState(RecordedPath::instance()->resumeState);

    // Show timed message matching C# original
    switch (RecordedPath::instance()->resumeState) {
    case 1:
        Backend::instance()->timedMessage(1500, "Resume Style", "Last Stopped Position");
        break;
    case 2:
        Backend::instance()->timedMessage(1500, "Resume Style", "Closest Point");
        break;
    default:
        Backend::instance()->timedMessage(1500, "Resume Style", "Start At Beginning");
        break;
    }
}

void RecordedPathInterface::recPathSwapAB()
{
    int cnt = RecordedPath::instance()->recList.size();
    QVector<CRecPathPt> _recList;
    _recList.reserve(cnt);

    for (int i = cnt - 1; i > -1; i--)
    {
        CRecPathPt pt = RecordedPath::instance()->recList[i];
        pt.heading += M_PI;
        if (pt.heading < -glm::twoPI) pt.heading += glm::twoPI;
        _recList.append(pt);
    }
    RecordedPath::instance()->recList.clear();
    RecordedPath::instance()->recList = _recList;
    RecordedPath::instance()->updateInterface();
}

void RecordedPathInterface::recPathPick()
{
    RecordedPath::instance()->resumeState = 0;
    RecordedPath::instance()->currentPositonIndex = 0;
    emit RecordedPath::instance()->open("");
}

void RecordedPathInterface::pathOpen(const QString &pathName)
{
    // Get current field directory from SettingsManager
    QString currentFieldDir = SettingsManager::instance()->f_currentDir();

    if (currentFieldDir.isEmpty()) {
        return;
    }

    // Build full file paths (matching original C# behavior)
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString appName = QCoreApplication::applicationName();
    QString fieldDir = documentsPath + "/" + appName + "/Fields/" + currentFieldDir;
    QString sourceFile = fieldDir + "/" + pathName;
    QString recPathFile = fieldDir + "/RecPath.txt";

    // Copy selected .rec file to RecPath.txt (like original C#)
    if (QFile::exists(sourceFile)) {
        // Remove existing RecPath.txt first (QFile::copy won't overwrite)
        if (QFile::exists(recPathFile)) {
            QFile::remove(recPathFile);
        }
        if (!QFile::copy(sourceFile, recPathFile)) {
            qWarning() << "Failed to copy to RecPath.txt";
        }
    }

    // Call FileLoadRecPath to load the path into memory
    QQmlEngine *engine = qmlEngine(this);
    QObject *formGPS = nullptr;
    if (engine) {
        QVariant aogVariant = engine->rootContext()->property("formGPS");
        formGPS = aogVariant.value<QObject*>();
    }
    if (formGPS) {
        QMetaObject::invokeMethod(formGPS, "FileLoadRecPath", Qt::DirectConnection, Q_ARG(QString, pathName));
    } else {
        // Load directly - same as FileLoadRecPath but inline
        QString directoryName = documentsPath + "/" + appName + "/Fields/" + currentFieldDir;
        QString filepath = directoryName + "/" + pathName;

        QFile recFile(filepath);
        if (!recFile.open(QIODevice::ReadOnly)) {
            return;
        }

        QTextStream reader(&recFile);
        reader.setLocale(QLocale::C);

        QString line = reader.readLine();
        line = reader.readLine();
        int numPoints = line.toInt();

        RecordedPath::instance()->recList.clear();
        RecordedPath::instance()->recList.reserve(numPoints);

        while (!reader.atEnd()) {
            for (int v = 0; v < numPoints; v++) {
                line = reader.readLine();
                int comma1 = line.indexOf(',');
                int comma2 = line.indexOf(',', comma1 + 1);
                int comma3 = line.indexOf(',', comma2 + 1);
                int comma4 = line.indexOf(',', comma3 + 1);

                CRecPathPt point(
                    QStringView(line).left(comma1).toDouble(),
                    QStringView(line).mid(comma1 + 1, comma2 - comma1 - 1).toDouble(),
                    QStringView(line).mid(comma2 + 1, comma3 - comma2 - 1).toDouble(),
                    QStringView(line).mid(comma3 + 1, comma4 - comma3 - 1).toDouble(),
                    (QStringView(line).mid(comma4 + 1) == u"True"));

                RecordedPath::instance()->recList.append(point);
            }
        }

        recFile.close();

        // Update QSG
        RecordedPath::instance()->updateInterface();
    }

    // Reset resume state when loading new path (matching C# original)
    RecordedPath::instance()->resumeState = 0;
    set_resumeState(0);

    // Set the current path name (like original stores in RecPath.txt)
    RecordedPath::instance()->set_currentPathName(pathName);

    // Set menu open AFTER updateInterface (which resets menuOpen)
    // This makes the selected path visible in QSG
    RecordedPath::instance()->m_recordedPathProperties->set_menuOpen(true);

    emit pathListChanged();
}

void RecordedPathInterface::pathDelete(const QString &pathName)
{
    // Get fields directory
    QString fieldsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                        + "/" + QCoreApplication::applicationName() + "/Fields";

    // Ensure we have .rec extension (like pathOpen does)
    QString fileName = pathName;
    if (!fileName.endsWith(".rec", Qt::CaseInsensitive)) {
        fileName += ".rec";
    }

    // Get current field directory
    QString currentFieldDir = SettingsManager::instance()->f_currentDir();

    if (currentFieldDir.isEmpty()) {
        return;
    }

    // Look only in current field directory (matching scanPathFiles behavior)
    QString filePath = fieldsDir + "/" + currentFieldDir + "/" + fileName;
    QFileInfo fi(filePath);

    if (fi.exists()) {
        if (QFile::remove(filePath)) {
            emit pathListChanged();
            return;
        }
    }

    // Also check RecPath.txt in current field
    QString recPathFile = fieldsDir + "/" + currentFieldDir + "/RecPath.txt";
    QFileInfo recFi(recPathFile);
    if (recFi.exists()) {
        if (QFile::remove(recPathFile)) {
            emit pathListChanged();
            return;
        }
    }

    emit pathListChanged();
}

QStringList RecordedPathInterface::scanPathFiles()
{
    QStringList pathFiles;

    // Get current field directory from SettingsManager (like original)
    QString currentFieldDir = SettingsManager::instance()->f_currentDir();

    if (currentFieldDir.isEmpty()) {
        return pathFiles;
    }

    // Get fields directory
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString appName = QCoreApplication::applicationName();
    QString fieldDirPath = documentsPath + "/" + appName + "/Fields/" + currentFieldDir;

    QDir fieldDir(fieldDirPath);
    if (!fieldDir.exists()) {
        return pathFiles;
    }

    // Scan only .rec files (like original)
    QFileInfoList files = fieldDir.entryInfoList(QStringList() << "*.rec", QDir::Files);

    for (const QFileInfo &fi : files) {
        // Get filename without extension like original
        QString baseName = fi.completeBaseName();
        pathFiles.append(baseName);
    }

    emit pathListChanged();
    return pathFiles;
}