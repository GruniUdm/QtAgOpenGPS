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
    qWarning() << "recPathFollowStop called, isDriving:" << isDriving() << "actualDriving:" << RecordedPath::instance()->isDrivingRecordedPath();
    if (RecordedPath::instance()->isDrivingRecordedPath())
    {
        qWarning() << "  Stopping driving";
        RecordedPath::instance()->StopDrivingRecordedPath();
        set_isDriving(false);
    }
    else
    {
        qWarning() << "  Starting driving, recList count:" << RecordedPath::instance()->recList.count();
        CYouTurn *yt = qobject_cast<CYouTurn*>(Backend::instance()->yt());
        bool started = RecordedPath::instance()->StartDrivingRecordedPath(*CVehicle::instance(), *yt);
        qWarning() << "  Started result:" << started;
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
    qDebug() << "pathOpen called with:" << pathName;
    
    // Get current field directory from SettingsManager
    QString currentFieldDir = SettingsManager::instance()->f_currentDir();
    
    if (currentFieldDir.isEmpty()) {
        qWarning() << "pathOpen: no current field";
        return;
    }
    
    // Build full file paths (matching original C# behavior)
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString appName = QCoreApplication::applicationName();
    QString fieldDir = documentsPath + "/" + appName + "/Fields/" + currentFieldDir;
    QString sourceFile = fieldDir + "/" + pathName;
    QString recPathFile = fieldDir + "/RecPath.txt";
    
    qDebug() << "pathOpen: source:" << sourceFile;
    qDebug() << "pathOpen: dest:" << recPathFile;
    
    // Copy selected .rec file to RecPath.txt (like original C#)
    if (QFile::exists(sourceFile)) {
        // Remove existing RecPath.txt first (QFile::copy won't overwrite)
        if (QFile::exists(recPathFile)) {
            QFile::remove(recPathFile);
        }
        if (QFile::copy(sourceFile, recPathFile)) {
            qDebug() << "Copied path to RecPath.txt";
        } else {
            qWarning() << "Failed to copy to RecPath.txt";
        }
    }
    
    // Call FileLoadRecPath to load the path into memory
    qWarning() << "pathOpen: calling FileLoadRecPath with:" << pathName;
    QQmlEngine *engine = qmlEngine(this);
    QObject *formGPS = nullptr;
    if (engine) {
        QVariant aogVariant = engine->rootContext()->property("formGPS");
        formGPS = aogVariant.value<QObject*>();
    }
    qWarning() << "pathOpen: formGPS object:" << formGPS;
    if (formGPS) {
        bool ok = QMetaObject::invokeMethod(formGPS, "FileLoadRecPath", Qt::DirectConnection, Q_ARG(QString, pathName));
        qWarning() << "pathOpen: invokeResult:" << ok;
    } else {
        qWarning() << "pathOpen: formGPS is null! Loading directly in interface";
        // Load directly - same as FileLoadRecPath but inline
        QString currentFieldDir = SettingsManager::instance()->f_currentDir();
        if (currentFieldDir.isEmpty()) {
            qWarning() << "pathOpen: no current field";
            return;
        }
        
        QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        QString appName = QCoreApplication::applicationName();
        QString directoryName = documentsPath + "/" + appName + "/Fields/" + currentFieldDir;
        QString filepath = directoryName + "/" + pathName;
        
        qWarning() << "pathOpen: loading from:" << filepath;
        
        QFile recFile(filepath);
        if (!recFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Couldn't open" << filepath;
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
        qWarning() << "pathOpen: loaded" << numPoints << "points";

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
    qWarning() << "pathOpen: setting menuOpen = true";
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
    
    qDebug() << "pathDelete: looking for" << fileName;
    
    // Get current field directory
    QString currentFieldDir = SettingsManager::instance()->f_currentDir();
    
    if (currentFieldDir.isEmpty()) {
        qWarning() << "pathDelete: no current field";
        return;
    }
    
    // Look only in current field directory (matching scanPathFiles behavior)
    QString filePath = fieldsDir + "/" + currentFieldDir + "/" + fileName;
    QFileInfo fi(filePath);
    
    if (fi.exists()) {
        if (QFile::remove(filePath)) {
            qDebug() << "Deleted path:" << filePath;
            emit pathListChanged();
            return;
        } else {
            qWarning() << "Failed to delete:" << filePath;
        }
    }
    
    // Also check RecPath.txt in current field
    QString recPathFile = fieldsDir + "/" + currentFieldDir + "/RecPath.txt";
    QFileInfo recFi(recPathFile);
    if (recFi.exists()) {
        if (QFile::remove(recPathFile)) {
            qDebug() << "Deleted RecPath.txt:" << recPathFile;
            emit pathListChanged();
            return;
        }
    }
    
    qWarning() << "pathDelete: file not found:" << filePath;
    emit pathListChanged();
}

QStringList RecordedPathInterface::scanPathFiles()
{
    QStringList pathFiles;
    
    // Get current field directory from SettingsManager (like original)
    QString currentFieldDir = SettingsManager::instance()->f_currentDir();
    
    if (currentFieldDir.isEmpty()) {
        qWarning() << "scanPathFiles: no current field";
        return pathFiles;
    }
    
    // Get fields directory
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString appName = QCoreApplication::applicationName();
    QString fieldDirPath = documentsPath + "/" + appName + "/Fields/" + currentFieldDir;
    
    qWarning() << "scanPathFiles: fieldDirPath:" << fieldDirPath;
    
    QDir fieldDir(fieldDirPath);
    if (!fieldDir.exists()) {
        qWarning() << "scanPathFiles: field directory doesn't exist:" << fieldDirPath;
        return pathFiles;
    }
    
    // Scan only .rec files (like original)
    QFileInfoList files = fieldDir.entryInfoList(QStringList() << "*.rec", QDir::Files);
    qWarning() << "scanPathFiles: .rec files found:" << files.count();
    
    for (const QFileInfo &fi : files) {
        // Get filename without extension like original
        QString baseName = fi.completeBaseName();
        pathFiles.append(baseName);
    }
    
    qWarning() << "scanPathFiles: Found path files:" << pathFiles;
    emit pathListChanged();
    return pathFiles;
}