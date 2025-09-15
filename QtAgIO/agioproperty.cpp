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
    // Force le rechargement depuis l'INI utilisateur
    if (agiosettings) {
        qDebug() << "🔧 AgIOProperty loading from INI:" << agiosettings->fileName();
        
        // Vérifier quelques valeurs clés pour diagnostic
        QString subnet = QString("%1.%2.%3")
            .arg(agiosettings->value("comm/udpIP1", 192).toInt())
            .arg(agiosettings->value("comm/udpIP2", 168).toInt()) 
            .arg(agiosettings->value("comm/udpIP3", 1).toInt());
        int udpPort = agiosettings->value("comm/udpListenPort", 9999).toInt();
        
        qDebug() << "📡 INI Configuration:";
        qDebug() << "   Subnet:" << subnet << "- UDP Port:" << udpPort;
        qDebug() << "   Loopback Listen:" << agiosettings->value("comm/loopListenPort", 15551).toInt();
        qDebug() << "   Loopback Send:" << agiosettings->value("comm/loopSendPort", 15550).toInt();
        
        qDebug() << "📡 NTRIP Configuration:";
        qDebug() << "   Enabled:" << agiosettings->value("comm/ntripIsOn", false).toBool();
        qDebug() << "   URL:" << agiosettings->value("comm/ntripURL", "test.test").toString();
        qDebug() << "   Mount:" << agiosettings->value("comm/ntripMount", "test").toString();
        qDebug() << "   Port:" << agiosettings->value("comm/ntripCasterPort", 2101).toInt();
        qDebug() << "   User:" << agiosettings->value("comm/ntripUserName", "test").toString();
        
        agiosettings->sync(); // Force la synchronisation
    } else {
        qDebug() << "❌ AgIOProperty: agiosettings is null, using hardcoded defaults";
    }
}

AgIOProperty::operator int() { 
    if (!agiosettings) return default_value.toInt();
    return agiosettings->value(key,default_value).toInt(); 
}
AgIOProperty::operator char() { 
    if (!agiosettings) return (char)(default_value.toInt());
    return (char)(agiosettings->value(key,default_value).toInt()); 
}
AgIOProperty::operator double() { 
    if (!agiosettings) return default_value.toDouble();
    return agiosettings->value(key,default_value).toDouble(); 
}
AgIOProperty::operator QString() { 
    if (!agiosettings) return default_value.toString();
    return agiosettings->value(key,default_value).toString(); 
}
AgIOProperty::operator bool() { 
    if (!agiosettings) return default_value.toBool();
    return agiosettings->value(key,default_value).toBool(); 
}
AgIOProperty::operator float() { 
    if (!agiosettings) return default_value.toFloat();
    return agiosettings->value(key,default_value).toFloat(); 
}
AgIOProperty::operator QColor() { return agiosettings->value(key,default_value).value<QColor>(); }
//special case for QVector<int>.  In AgIOSettings, this type is marshalled as
//a QVariantList so that the ini file has human-readable text in it.
AgIOProperty::operator QVector<int>() { return agiosettings->value(key,default_list); }

QVariant AgIOProperty::value(void) { return agiosettings->value(key,default_value); }

AgIOProperty &AgIOProperty::operator=(QVariant newvalue) { agiosettings->setValue(key, newvalue); return *this;}
AgIOProperty &AgIOProperty::operator=(QVector<int> newvalue) { agiosettings->setValue(key, newvalue); return *this;}
