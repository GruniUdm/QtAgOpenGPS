#ifndef QMLSETTINGS_H
#define QMLSETTINGS_H

#include <QObject>
#include <QQmlEngine>
#include <QtQml/qqml.h>
#include <QVariantList>
#include <QSettings>
#include <QQmlPropertyMap>


class QMLSettings : public QQmlPropertyMap
{
    Q_OBJECT

    //this below is for future usage as c++ module plugin
    //adding the QML_ELEMENT macro to the class makes it available to QML
    //QML_ELEMENT
    //adding the QML_SINGLETON macro to the class makes qml_settings a singleton
    //QML_SINGLETON

public:
    QMap<QString, QString> qml_to_settings_map; //map of qml property keys to agiosettings path keys
    QMap<QString, QString> settings_to_qml_map; //map of qml property keys to agiosettings path keys
    QMap<QString, QString> settings_type_map; //map of qml property keys to agiosettings path keys

    explicit QMLSettings(QObject *parent = nullptr);

    void addKey(QString qml_key, QString settings_key, QString type_name);
    void setupKeys();

    void loadSettings();

    //add QMLSettings as element of formloop instance
    static QMLSettings *instance(QQmlEngine *engine = nullptr, QJSEngine *scriptEngine = nullptr);
    void updateSetting(const QString &settings_key);
public slots:
    void onValueChanged(const QString &key, const QVariant &value);
};

#endif // QMLSETTINGS_H
