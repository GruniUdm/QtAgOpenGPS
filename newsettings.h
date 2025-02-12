#ifndef NEWSETTINGS_H
#define NEWSETTINGS_H

#include <QObject>
#include <QQmlPropertyMap>
#include <QVariant>
#include <QSettings>
#include <QColor>
#include <QPoint>
#include <QSize>
#include <QRect>

extern QVector<int> default_relay_pinConfig;
extern QVector<int> default_zones;

class NewSettings;

extern NewSettings *settings;

/* This class helps bridge between QSettings and QML/Javascript.
 * An instance of QSettings is kept inside this class and a
 * QSettings-like api is exported. This class is generally only
 * instantiated once and hopefully the instance can be registered
 * with qml using qmlSetSingletonInstance().
 *
 * The class must be initialized with all of the keys we're going
 * to use, along with their types and default values.  Types are
 * required because we have to convert some values when they are
 * set in Javascript.  addKey() will do this for a given key.
 *
 * Anytime setValue() is called QSettings gets updated and also the
 * hash map is updated, triggering binding updates in QML.  Any time
 * QML changes a property in this map, we get a signal and can then
 * update QSettings.
 */

class NewSettings : public QQmlPropertyMap
{
    Q_OBJECT

    enum SpecialCase {
        NORMAL = 0,
        VECTOR_OF_INTS = 1,
    };


private:
    QSettings settings;
    QMap<QString, QMetaType::Type> settings_type_map;
    QMap<QString, SpecialCase> special_case_map;

public:
    explicit NewSettings(QObject *parent = nullptr);


    //all keys must be predeclared by calling this method
    void setupKeys();

    void addKey(const QString qsettings_key,
                const QVariant &default_value,
                NewSettings::SpecialCase special_case = NORMAL);

    QVariant value(const QString &key);
    QVector<int> valueIntVec(const QString &key);
    void setValue(const QString &key, const QVariant &value);
    void setValue(const QString &key, const QVector<int> &value_list);

    QJsonObject toJson();
    bool saveJson(QString filename);
    bool loadJson(QString filename);

    void sync();

public slots:
    void onValueChanged (const QString &qml_key, const QVariant &value);

};

template <class T> static QVariant toVariant(const QVector<T> &list)
{
    QVariantList variantList;
    variantList.reserve(list.size());
    for (const auto& v : list)
    {
        variantList.append(v);
    }
    return variantList;
}

template <class T> static QVector<T> toVector(const QVariant &qv)
{
    QVector <T> dataList;
    foreach(QVariant v, qv.value<QVariantList>()) {
        dataList << v.value<T>();
    }
    return dataList;
}

/*
QColor parseColor(QString setcolor);
QVector3D parseColorVector(QString setcolor);
int colorSettingStringToInt(QString colorSettingString);
*/

#endif // NEWSETTINGS_H
