// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Settings wrapper class tie in
#ifndef AOGSETTINGS_H
#define AOGSETTINGS_H

#include <QSettings>
#include <QVariant>
#include <QList>
#include <QVector3D>
#include <QColor>

//Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QVector<int>)

extern QVariant unset;

class QJsonObject;

class AOGSettings : public QSettings
{
    Q_OBJECT
public:
    AOGSettings(QObject *parent = 0);

    QVariant value(const QString &key, const QVariant &defaultvalue = unset);
    QVector<int> value(const QString &key, const QVector<int> &defaultvalue = QVector<int> {});
    void setValue(const QString &key, const QVector<int> &value_list);
    void setValue(const QString &key, const QVariant &value);

    //special method for QMLSettings to call, which won't call back to
    //QMLSettings which would be a loop
    void setValue_noqml(const QString &key, const QVariant &value);

    QJsonObject toJson();
    bool saveJson(QString filename);
    bool loadJson(QString filename);

signals:
    void updateFromSettings();
};

template <class T> static QVariant _toVariant(const QVector<T> &list)
{
    QVariantList variantList;
    variantList.reserve(list.size());
    for (const auto& v : list)
    {
        variantList.append(v);
    }
    return variantList;
}

template <class T> static QVector<T> _toVector(const QVariant &qv)
{
    QVector <T> dataList;
    foreach(QVariant v, qv.value<QVariantList>()) {
        dataList << v.value<T>();
    }
    return dataList;
}

QColor parseColor(QString setcolor);
QVector3D parseColorVector(QString setcolor);
int colorSettingStringToInt(QString colorSettingString);

#endif // AOGSETTINGS_H

