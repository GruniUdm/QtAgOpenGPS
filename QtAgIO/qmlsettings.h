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

    //Qt 6.8 modern singleton pattern - in AgIO module
    QML_NAMED_ELEMENT(AgIOSettings)
    QML_SINGLETON

public:
    QMap<QString, QString> qml_to_settings_map; //map of qml property keys to agiosettings path keys
    QMap<QString, QString> settings_to_qml_map; //map of qml property keys to agiosettings path keys
    QMap<QString, QString> settings_type_map; //map of qml property keys to agiosettings path keys

    explicit QMLSettings(QObject *parent = nullptr);

    void addKey(QString qml_key, QString settings_key, QString type_name);
    void setupKeys();

    void loadSettings();

    //Qt 6.8 factory function for QML_SINGLETON
    static QMLSettings *create(QQmlEngine *engine, QJSEngine *scriptEngine);
    
    //FormLoop integration - same pattern as FormGPS
    static void setFormLoopInstance(QMLSettings* instance);
    static QMLSettings *instance();
    void updateSetting(const QString &settings_key);

private:
    static QMLSettings* s_formloop_instance;
    
public slots:
    void onValueChanged(const QString &key, const QVariant &value);
};

#endif // QMLSETTINGS_H
