#include "settings.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

//Q_DECLARE_METATYPE(QVector<int>)
const int METATYPE_QVECTOR_INT = qRegisterMetaType<QVector<int>>("QVector<int>");
const int METATYPE_QVECTOR_DOUBLE = qRegisterMetaType<QVector<double>>("QVector<double>");
const int METATYPE_QVECTOR_STRING = qRegisterMetaType<QVector<QString>>("QVector<QString>");

Settings::Settings(QObject *parent)
    : QQmlPropertyMap{parent}
{
    setupKeys();

    connect (this, &QQmlPropertyMap::valueChanged,
            this, &Settings::onValueChanged);
}

void Settings::addKey(const QString settings_key,
                         const QVariant &default_value,
                         const QMetaType type)
{
    //put key into app-wide settings with its default value,
    //if it's not there already.  If it's already in QSettings, let's
    //make sure it's the correct type
    QVariant settings_value = settings.value(settings_key, default_value);

    settings_type_map.insert(settings_key, type);

    //INI files are typeless, so when settings are read in when QSettings
    //settings is constructed, some of the types end up being wrong.  In
    //a couple of cases we need to correct them back to QVariantLists.

    //this one should be a qvariant list, but ini reads it in as a QStringList
    if (type.id() == METATYPE_QVECTOR_INT && QString(settings_value.typeName()) == "QStringList" ) {
        QVariantList l;
        for(QString &i: settings_value.toStringList()) {
            l.append(QVariant(i.toInt()));
        }
        settings_value = l;
    } else if(type.id() == METATYPE_QVECTOR_DOUBLE && QString(settings_value.typeName()) == "QStringList" ) {
        QVariantList l;
        for(QString &i: settings_value.toStringList()) {
            l.append(QVariant(i.toDouble()));
        }
        settings_value = l;
    } else if(type.id() == METATYPE_QVECTOR_STRING && QString(settings_value.typeName()) == "QStringList" ) {
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
                qWarning() << "Cannot convert " << settings_key << " from INI to type " << type.name();
            }
        }
    }

    //set the possibly changed value in both settings and the hash map
    settings.setValue(settings_key, settings_value);
    QString qml_key = settings_key.split('/').join('_');
    insert(qml_key, settings_value);
    settings.sync();
}

void Settings::onValueChanged(const QString &qml_key,
                                 const QVariant &value)
{
    QString settings_key = qml_key.split('_').join('/');
    QMetaType settings_type = settings_type_map[settings_key];

    QString type_name = value.typeName();

    if (type_name == "QJSValue" && settings_type.id() == METATYPE_QVECTOR_INT) {
        QVector<int> v = toVector<int>(value);
        settings.setValue(settings_key, toVariant(v));
    } else if (type_name == "QJSValue" && settings_type.id() == METATYPE_QVECTOR_DOUBLE) {
        QVector<double> v = toVector<double>(value);
        settings.setValue(settings_key, toVariant(v));
    } else if (type_name == "QJSValue" && settings_type.id() == METATYPE_QVECTOR_STRING) {
        QVector<QString> v = toVector<QString>(value);
        settings.setValue(settings_key, toVariant(v));
    } else {
        settings.setValue(settings_key, value);
    }

    settings.sync();
}

QVariant Settings::value(const QString &key)
{
    QVariant notfound("NOTFOUND"); //sentinal
    QVariant value = settings.value(key,notfound);

    if(value == notfound) {
        qWarning() << "Settings key not found: " << key;
        return QVariant();
    }
    return settings.value(key);
}

QVector<int> Settings::valueIntVec(const QString &key)
{
    QVariant val;
    val = settings.value(key);
    return toVector<int>(val);
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    QVariant notfound("NOTFOUND"); //sentinal
    QVariant existing_value = settings.value(key,notfound);

    if (existing_value == notfound) {
        qWarning() << "Tried to write to non-pre-existing key: " << key;
        //will litter INI with "NOTFOUND" strings but should help us
        //track down bad keys
        return;
    }

    settings.setValue(key, value);

    //in qml we use underscores instead of slashes.
    QString qml_key = key.split('/').join('_');
    insert(qml_key, value);
    settings.sync();
}

void Settings::setValue(const QString &key, const QVector<int> &value_list)
{
    QVariant qv = toVariant(value_list);
    settings.setValue(key, qv);

    //in qml we use underscores instead of slashes.
    QString qml_key = key.split('/').join('_');
    insert(qml_key, qv);
}

QJsonObject Settings::toJson()
{
    //b = QVariant(QColor::fromRgbF(1,0.5,0.2));
    QVariant b;

    QStringList keys = settings.allKeys();
    QString type;
    QString json_value;
    QJsonObject blah; // = QJsonObject::fromVariantMap(keysValuesPairs);

    for (const auto &key : keys)
    {
        b = settings.value(key);

        type = b.typeName();

        if (type == "QStringList" ||
            type == "QVariantList"  ||
            type == "QJSValue")
        {
            QVector<int> list = toVector<int>(b);
            json_value="@List:";
            for(int i=0; i < list.length(); i++){
                json_value += QString("%1").arg(list[i]);
                if (i < list.length() -1)
                    json_value +=",";
            }
            blah[key] = json_value;
        } else if (type == "QPoint") {
            QByteArray raw_value;
            QDataStream ds(&raw_value,QIODevice::WriteOnly);

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

bool Settings::loadJson(QString filename)
{
    QFile loadfile(filename);
    if (!loadfile.open(QIODevice::ReadOnly))
    {
        qWarning() << "Could not load json settings file " << filename;
        return false;
    }

    QByteArray loadedjson = loadfile.readAll();
    QJsonDocument loaded(QJsonDocument::fromJson(loadedjson));
    QJsonObject j = loaded.object();
    QString new_value;
    QVariant v;

    //load all the settings from json and put them in our
    //QSettings and also this property hash map.
    for (const auto &key : j.keys())
    {
        new_value = j[key].toString();
        if (new_value.startsWith("@Variant("))
        {
            QByteArray raw_data;
            QDataStream ds(&raw_data,QIODevice::ReadOnly);

            raw_data = new_value.toLatin1().mid(9);
            ds >> v;
            setValue(key, v);
        } else if(new_value.startsWith("@List:")) {
            new_value = new_value.mid(6);
            QStringList parts = new_value.split(",");
            QVector<int> list;
            for(QString part: parts) {
                list.append(part.toInt());
            }

            setValue(key,list);
        } else {
            //if (key == "display/isMetric")
            //    qDebug() << "isMetric is a problem child.";
            v = j[key].toVariant();
            setValue(key, v);
        }
    }

    return true;
}

bool Settings::saveJson(QString filename)
{
    QFile savefile(filename);
    if (!savefile.open(QIODevice::WriteOnly))
    {
        qWarning() << "Could not save json settings file " << filename;
        return false;
    }

    savefile.write(QJsonDocument(toJson()).toJson());
    savefile.close();

    return true;

}

void Settings::sync() {
    settings.sync();
}

QVariant Settings::updateValue(const QString &key, const QVariant &input)
{
    return QQmlPropertyMap::updateValue(key, input);
}
