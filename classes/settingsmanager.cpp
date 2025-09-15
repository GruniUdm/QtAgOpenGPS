#include "settingsmanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDebug>
#include <QDataStream>

// SomcoSoftware approach: Qt manages the singleton automatically

// MASSIVE MIGRATION: METATYPE constants definitions (from settings.cpp)
const int METATYPE_QVECTOR_INT = qRegisterMetaType<QVector<int>>("QVector<int>");
const int METATYPE_QVECTOR_DOUBLE = qRegisterMetaType<QVector<double>>("QVector<double>");
const int METATYPE_QVECTOR_STRING = qRegisterMetaType<QVector<QString>>("QVector<QString>");

SettingsManager::SettingsManager(QObject* parent)
    : QQmlPropertyMap(parent)
{
    m_qsettings = new QSettings("QtAgOpenGPS", "QtAgOpenGPS", this);
    setupKeys();
    
    // Connect QML property changes to our handler
    connect(this, &QQmlPropertyMap::valueChanged,
            this, &SettingsManager::onValueChanged);
}

SettingsManager::~SettingsManager()
{
    if (m_qsettings) {
        m_qsettings->sync();
    }
}

// Meyer's singleton pattern - no explicit instance() method needed in .cpp

void SettingsManager::addKey(const QString& settings_key, const QVariant& default_value, const QMetaType type)
{
    QMutexLocker locker(&m_accessMutex);
    
    // Logic copied EXACTLY from settings.cpp:21-70
    QVariant settings_value = m_qsettings->value(settings_key, default_value);
    m_typeMap.insert(settings_key, type);

    // Type correction for INI files (copied from settings.cpp)
    if (type.id() == METATYPE_QVECTOR_INT && QString(settings_value.typeName()) == "QStringList") {
        QVariantList l;
        for(QString &i: settings_value.toStringList()) {
            l.append(QVariant(i.toInt()));
        }
        settings_value = l;
    } else if(type.id() == METATYPE_QVECTOR_DOUBLE && QString(settings_value.typeName()) == "QStringList") {
        QVariantList l;
        for(QString &i: settings_value.toStringList()) {
            l.append(QVariant(i.toDouble()));
        }
        settings_value = l;
    } else if(type.id() == METATYPE_QVECTOR_STRING && QString(settings_value.typeName()) == "QStringList") {
        QVariantList l;
        for(QString &i: settings_value.toStringList()) {
            l.append(QVariant(i));
        }
        settings_value = l;
    } else {
        if (settings_value.metaType() != type) {
            if (settings_value.canConvert(type)) {
                settings_value.convert(type);
            } else {
                qWarning() << "SettingsManager: Cannot convert" << settings_key << "from INI to type" << type.name();
            }
        }
    }

    // Set value in both QSettings and QML property map
    m_qsettings->setValue(settings_key, settings_value);
    QString qml_key = settings_key.split('/').join('_');
    insert(qml_key, settings_value);
    m_qsettings->sync();
}

QVariant SettingsManager::value(const QString& key, const QVariant& defaultValue) const
{
    QMutexLocker locker(&m_accessMutex);
    
    QVariant notfound("NOTFOUND"); // sentinel
    QVariant value = m_qsettings->value(key, notfound);

    if (value == notfound) {
        qWarning() << "SettingsManager: Settings key not found:" << key;
        return defaultValue;
    }
    return m_qsettings->value(key);
}

void SettingsManager::setValue(const QString& key, const QVariant& value)
{
    {
        QMutexLocker locker(&m_accessMutex);
        
        QVariant notfound("NOTFOUND"); // sentinel
        QVariant existing_value = m_qsettings->value(key, notfound);

        if (existing_value == notfound) {
            qWarning() << "SettingsManager: Tried to write to non-pre-existing key:" << key;
            return;
        }

        m_qsettings->setValue(key, value);

        // Update QML property map (key transformation like settings.cpp)
        QString qml_key = key.split('/').join('_');
        insert(qml_key, value);
        m_qsettings->sync();
    }
    
    // Emit signal AFTER mutex unlock
    emit settingChanged(key, value);
}

QVector<int> SettingsManager::valueIntVec(const QString& key)
{
    QMutexLocker locker(&m_accessMutex);
    QVariant val = m_qsettings->value(key);
    return toVector<int>(val);
}

void SettingsManager::setValue(const QString& key, const QVector<int>& value_list)
{
    {
        QMutexLocker locker(&m_accessMutex);
        
        QVariant qv = toVariant(value_list);
        m_qsettings->setValue(key, qv);

        // Update QML property map
        QString qml_key = key.split('/').join('_');
        insert(qml_key, qv);
    }
    
    emit settingChanged(key, toVariant(value_list));
}

void SettingsManager::sync()
{
    QMutexLocker locker(&m_accessMutex);
    m_qsettings->sync();
}

QJsonObject SettingsManager::toJson()
{
    QMutexLocker locker(&m_accessMutex);
    return toJsonUnsafe();  // Call unsafe version with mutex protection
}

QJsonObject SettingsManager::toJsonUnsafe()
{
    // Logic copied EXACTLY from settings.cpp:145-189 (without mutex - for internal use)
    QVariant b;
    QStringList keys = m_qsettings->allKeys();
    QString type;
    QString json_value;
    QJsonObject blah;

    for (const auto &key : keys) {
        b = m_qsettings->value(key);
        type = b.typeName();

        if (type == "QStringList" || type == "QVariantList" || type == "QJSValue") {
            QVector<int> list = toVector<int>(b);
            json_value = "@List:";
            for(int i=0; i < list.length(); i++){
                json_value += QString("%1").arg(list[i]);
                if (i < list.length() -1)
                    json_value += ",";
            }
            blah[key] = json_value;
        } else if (type == "QPoint") {
            QByteArray raw_value;
            QDataStream ds(&raw_value, QIODevice::WriteOnly);
            ds << b;
            json_value = QLatin1String("@Variant(");
            json_value += QString::fromLatin1(raw_value.constData(), raw_value.size());
            json_value += ")";
            blah[key] = json_value;
        } else {
            blah[key] = QJsonValue::fromVariant(b);
        }
    }

    return blah;
}

bool SettingsManager::loadJson(QString filename)
{
    // Read JSON file OUTSIDE mutex scope (conforming to THREADING_PLAN pattern)
    
    QFile loadfile(filename);
    if (!loadfile.open(QIODevice::ReadOnly)) {
        qWarning() << "SettingsManager: Could not load json settings file" << filename;
        return false;
    }

    QByteArray loadedjson = loadfile.readAll();
    QJsonDocument loaded(QJsonDocument::fromJson(loadedjson));
    QJsonObject j = loaded.object();
    QString new_value;
    QVariant v;

    // Process settings updates with limited mutex scope (conforming to THREADING_PLAN pattern)
    {
        QMutexLocker locker(&m_accessMutex);  // Limited scope for settings updates only

        for (const auto &key : j.keys()) {
            new_value = j[key].toString();
            if (new_value.startsWith("@Variant(")) {
                QByteArray raw_data;
                QDataStream ds(&raw_data, QIODevice::ReadOnly);
                raw_data = new_value.toLatin1().mid(9);
                ds >> v;
                // Use internal setValue to avoid recursive locking
                m_qsettings->setValue(key, v);
                QString qml_key = key.split('/').join('_');
                insert(qml_key, v);
            } else if(new_value.startsWith("@List:")) {
                new_value = new_value.mid(6);
                QStringList parts = new_value.split(",");
                QVector<int> list;
                for(QString part: parts) {
                    list.append(part.toInt());
                }
                QVariant qv = toVariant(list);
                m_qsettings->setValue(key, qv);
                QString qml_key = key.split('/').join('_');
                insert(qml_key, qv);
            } else {
                v = j[key].toVariant();
                m_qsettings->setValue(key, v);
                QString qml_key = key.split('/').join('_');
                insert(qml_key, v);
            }
        }
    }  // Mutex unlocked here

    return true;
}

bool SettingsManager::saveJson(QString filename)
{
    qDebug() << "saveJson: Starting for" << filename;

    // Generate JSON with thread-safe scope (conforming to THREADING_PLAN pattern)
    QJsonObject jsonData;
    {
        qDebug() << "saveJson: Acquiring mutex for toJson()";
        QMutexLocker locker(&m_accessMutex);  // Limited scope like setValue()
        qDebug() << "saveJson: Mutex acquired, calling toJson()";
        jsonData = toJsonUnsafe();  // Call unsafe version with mutex protection
        qDebug() << "saveJson: toJson() completed, releasing mutex";
    }  // Mutex unlocked here before file I/O
    qDebug() << "saveJson: Mutex released, starting file operations";

    // File I/O operations OUTSIDE mutex scope (no thread conflict)
    QFile savefile(filename);
    qDebug() << "saveJson: Attempting to open file" << filename;
    if (!savefile.open(QIODevice::WriteOnly)) {
        qWarning() << "SettingsManager: Could not save json settings file" << filename;
        return false;
    }
    qDebug() << "saveJson: File opened successfully, writing JSON";

    savefile.write(QJsonDocument(jsonData).toJson());
    qDebug() << "saveJson: JSON written, closing file";
    savefile.close();
    qDebug() << "saveJson: File closed, operation completed successfully";
    return true;
}

QVariant SettingsManager::updateValue(const QString& key, const QVariant& input)
{
    return QQmlPropertyMap::updateValue(key, input);
}

void SettingsManager::onValueChanged(const QString& qml_key, const QVariant& value)
{
    // Logic copied from settings.cpp:72-94 with thread safety
    QString settings_key = qml_key.split('_').join('/');
    QMetaType settings_type = m_typeMap[settings_key];

    QString type_name = value.typeName();

    {
        QMutexLocker locker(&m_accessMutex);
        
        if (type_name == "QJSValue" && settings_type.id() == METATYPE_QVECTOR_INT) {
            QVector<int> v = toVector<int>(value);
            m_qsettings->setValue(settings_key, toVariant(v));
        } else if (type_name == "QJSValue" && settings_type.id() == METATYPE_QVECTOR_DOUBLE) {
            QVector<double> v = toVector<double>(value);
            m_qsettings->setValue(settings_key, toVariant(v));
        } else if (type_name == "QJSValue" && settings_type.id() == METATYPE_QVECTOR_STRING) {
            QVector<QString> v = toVector<QString>(value);
            m_qsettings->setValue(settings_key, toVariant(v));
        } else {
            m_qsettings->setValue(settings_key, value);
        }

        m_qsettings->sync();
    }
}

// setupKeys() is implemented in settingsmanager_addkeys.cpp