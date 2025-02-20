#include "cpgn.h"
#include "newsettings.h"

CPGN_FC::CPGN_FC(): pgn(QByteArray( "\x80\x81\x7f\xfc\x08\x00\x00\x00\x00\x00\x00\x00\x00\xCC", 14))
{
    loadSettings();
}

void CPGN_FC::loadSettings()
{
    pgn[gainProportional] = settings->value(SETTINGS_as_Kp).value<int>();
    pgn[highPWM] = settings->value(SETTINGS_as_highSteerPWM).value<int>();
    pgn[lowPWM] = settings->value(SETTINGS_as_lowSteerPWM).value<int>();
    pgn[minPWM] = settings->value(SETTINGS_as_minSteerPWM).value<int>();
    pgn[countsPerDegree] = settings->value(SETTINGS_as_countsPerDegree).value<int>();
    pgn[wasOffsetHi] = (char)(settings->value(SETTINGS_as_wasOffset).value<int>() >> 8);;
    pgn[wasOffsetLo] = (char)settings->value(SETTINGS_as_wasOffset).value<int>();
    pgn[ackerman] = settings->value(SETTINGS_as_ackerman).value<int>();
}

CPGN_EE::CPGN_EE() : pgn(QByteArray("\x80\x81\x7f\xee\x08\x00\x00\x00\x00\x00\x00\x00\x00\xCC", 14))
{
    loadSettings();
}

void CPGN_EE::loadSettings()
{
    pgn[raiseTime] = settings->value(SETTINGS_ardMac_hydRaiseTime).value<int>();
    pgn[lowerTime] = settings->value(SETTINGS_ardMac_hydLowerTime).value<int>();
    pgn[enableHyd] = settings->value(SETTINGS_ardMac_isHydEnabled).value<int>();
    pgn[set0] = settings->value(SETTINGS_ardMac_setting0).value<int>();

    pgn[user1] = settings->value(SETTINGS_ardMac_user1).value<int>();
    pgn[user2] = settings->value(SETTINGS_ardMac_user2).value<int>();
    pgn[user3] = settings->value(SETTINGS_ardMac_user3).value<int>();
    pgn[user4] = settings->value(SETTINGS_ardMac_user4).value<int>();
}

void CPGN_EE::MakeCRC()
{
    int crc = 0;
    for (int i = 2; i < pgn.length() - 1; i++)
    {
        crc += pgn[i];
    }
    pgn[pgn.length() - 1] = crc;
}

CPGN_EC::CPGN_EC() : pgn(QByteArray("\x80\x81\x7f\xec\x18\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xCC", 30))
{
    loadSettings();
}

void CPGN_EC::loadSettings()
{
    QVector<int> words = toVector<int>(settings->value(SETTINGS_relay_pinConfig));

    pgn[pin0] = words[0];
    pgn[pin1] = words[1];
    pgn[pin2] = words[2];
    pgn[pin3] = words[3];
    pgn[pin4] = words[4];
    pgn[pin5] = words[5];
    pgn[pin6] = words[6];
    pgn[pin7] = words[7];
    pgn[pin8] = words[8];
    pgn[pin9] = words[9];

    pgn[pin10] = words[10];
    pgn[pin11] = words[11];
    pgn[pin12] = words[12];
    pgn[pin13] = words[13];
    pgn[pin14] = words[14];
    pgn[pin15] = words[15];
    pgn[pin16] = words[16];
    pgn[pin17] = words[17];
    pgn[pin18] = words[18];
    pgn[pin19] = words[19];

    pgn[pin20] = words[20];
    pgn[pin21] = words[21];
    pgn[pin22] = words[22];
    pgn[pin23] = words[23];
}

void CPGN_EC::MakeCRC()
{
    int crc = 0;
    for (int i = 2; i < pgn.length() - 1; i++)
    {
        crc += pgn[i];
    }
    pgn[pgn.length() - 1] = crc;
}

PGN32502::PGN32502() : pgn(QByteArray( "\xF6\x7E\x00\x00\x00\x00\x00\x00\x00\x00\x00\xCC", 12))
{
    loadSettings();
}

void PGN32502::loadSettings()

{   QVector<int> rate_settings = toVector<int>(settings->value(SETTINGS_rate_Product0));
    pgn[ID] = rate_settings[0];
    pgn[KP] = rate_settings[3];
    pgn[KI] = rate_settings[4];
    pgn[KD] = rate_settings[5];
    pgn[MinPWM] = rate_settings[6];
    pgn[MaxPWM] = rate_settings[7];
    pgn[PIDScale] = rate_settings[8];
    pgn[MeterCal] = rate_settings[9];
    pgn[Command] = rate_settings[10];
}

void PGN32502::MakeCRC()
{
    int crc = 0;
    for (int i = 2; i < pgn.length() - 1; i++)
    {
        crc += pgn[i];
    }
    pgn[pgn.length() - 1] = crc;
}

PGN32500::PGN32500() : pgn(QByteArray( "\xF7\xF4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xCC", 14))
{
}

PGN32400::PGN32400() : pgn(QByteArray( "\xF7\x7E\x7E\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xCC", 16))
{
}

void PGN32500::MakeCRC()
{
    int crc = 0;
    for (int i = 2; i < pgn.length() - 1; i++)
    {
        crc += pgn[i];
    }
    pgn[pgn.length() - 1] = crc;
}
