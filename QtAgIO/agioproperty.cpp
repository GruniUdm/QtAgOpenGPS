#include "agiosettings.h"
#include "agioproperty.h"

//AgIOSettings settings;

AgIOProperty test_property("test/test", false);
//QtAOG-specific setting properties
AgIOProperty property_environment_last_name("environment/name",QString(""));

AgIOProperty::AgIOProperty(const QString key, const QVariant &defaultvalue): key(key), default_value(defaultvalue)
{
    AgIOProperty::add_default_map(key, default_value);
}

AgIOProperty::AgIOProperty(const QString key, const QVector<int> &defaultvalue): key(key), default_list(defaultvalue)
{
    AgIOProperty::add_default_map(key,toVariant(defaultvalue));
}

void AgIOProperty::add_default_map(const QString &key, const QVariant &default_value) {
    default_map[key] = default_value;
}

void AgIOProperty::init_defaults()
{
    for (QString key: AgIOProperty::default_map.keys()) {
        agiosettings->value(key, default_map[key]);
    }
}

AgIOProperty::operator int() { return agiosettings->value(key,default_value).toInt(); }
AgIOProperty::operator char() { return (char)(agiosettings->value(key,default_value).toInt()); }
AgIOProperty::operator double() { return agiosettings->value(key,default_value).toDouble(); }
AgIOProperty::operator QString() { return agiosettings->value(key,default_value).toString(); }
AgIOProperty::operator bool() { return agiosettings->value(key,default_value).toBool(); }
AgIOProperty::operator float() { return agiosettings->value(key,default_value).toFloat(); }
AgIOProperty::operator QColor() { return agiosettings->value(key,default_value).value<QColor>(); }
//special case for QVector<int>.  In AgIOSettings, this type is marshalled as
//a QVariantList so that the ini file has human-readable text in it.
AgIOProperty::operator QVector<int>() { return agiosettings->value(key,default_list); }

QVariant AgIOProperty::value(void) { return agiosettings->value(key,default_value); }

AgIOProperty &AgIOProperty::operator=(QVariant newvalue) { agiosettings->setValue(key, newvalue); return *this;}
AgIOProperty &AgIOProperty::operator=(QVector<int> newvalue) { agiosettings->setValue(key, newvalue); return *this;}
