#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QQmlPropertyMap>
#include <QVariant>
#include <QSettings>
#include <QMutex>
#include <QMutexLocker>
#include <QColor>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QVector>
#include <QJsonObject>
#include <QtQml/qqmlregistration.h>
#include <QtQml/QQmlEngine>
#include <QtQml/QJSEngine>

#include "settings_defines.h"

class SettingsManager : public QQmlPropertyMap
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT

public:
    ~SettingsManager();

    // C++ singleton access (strict singleton pattern - same as CTrack/CVehicle)
    static SettingsManager* instance() {
        static SettingsManager* s_instance = new SettingsManager(nullptr);
        return s_instance;
    }

    // API compatible Settings (EXACTEMENT) - QML accessible methods
    Q_INVOKABLE QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
    Q_INVOKABLE QVector<int> valueIntVec(const QString& key);
    Q_INVOKABLE void setValue(const QString& key, const QVector<int>& value_list);
    Q_INVOKABLE void sync();

    // JSON compatibility (Settings features)
    Q_INVOKABLE QJsonObject toJson();
    Q_INVOKABLE bool saveJson(QString filename);
    Q_INVOKABLE bool loadJson(QString filename);

private:
    // Private constructor for strict singleton pattern
    explicit SettingsManager(QObject* parent = nullptr);

    mutable QMutex m_accessMutex;
    QSettings* m_qsettings;
    QMap<QString, QMetaType> m_typeMap;

    // Internal methods (C++ only)
    void setupKeys();
    void addKey(const QString& qsettings_key, const QVariant& default_value, const QMetaType type);
    QJsonObject toJsonUnsafe();  // toJson() without mutex (for internal use when mutex already held)

protected:
    QVariant updateValue(const QString& key, const QVariant& input) override;

public slots:
    void onValueChanged(const QString& qml_key, const QVariant& value);

signals:
    void settingChanged(const QString& key, const QVariant& value);
};

// MASSIVE MIGRATION: toVariant/toVector templates and METATYPE constants
extern const int METATYPE_QVECTOR_INT;
extern const int METATYPE_QVECTOR_DOUBLE;
extern const int METATYPE_QVECTOR_STRING;

#ifndef TO_VARIANT_TO_VECTOR_DEFINED
#define TO_VARIANT_TO_VECTOR_DEFINED
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
#endif

#endif // SETTINGSMANAGER_H