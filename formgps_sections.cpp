// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Mainly concerned with module communication on section related PGN data
#include "formgps.h"
#include "qmlutil.h"
#include "common.h"
#include "cpgn.h"
#include "classes/settingsmanager.h"
#include "cmodulecomm.h"

/* SectionSetPosition(), SectionCalcWidths(), and SectionCalcMulti() are all in CTool */

void FormGPS::BuildMachineByte()
{
    CPGN_FE &p_254 = CModuleComm::instance()->p_254;
    CPGN_EF &p_239 = CModuleComm::instance()->p_239;
    CPGN_E5 &p_229 = CModuleComm::instance()->p_229;

    if (tool.isSectionsNotZones)
    {
        p_254.pgn[CPGN_FE::sc1to8] = 0;
        p_254.pgn[CPGN_FE::sc9to16] = 0;

        int number = 0;
        for (int j = 0; j < 8; j++)
        {
            if (tool.section[j].isSectionOn)
                number |= 1 << j;
        }
        p_254.pgn[CPGN_FE::sc1to8] = (char)number;
        number = 0;

        for (int j = 8; j < 16; j++)
        {
            if (tool.section[j].isSectionOn)
                number |= 1 << (j-8);
        }
        p_254.pgn[CPGN_FE::sc9to16] = (char)number;

        //machine pgn
        p_239.pgn[CPGN_EF::sc9to16] = p_254.pgn[CPGN_FE::sc9to16];
        p_239.pgn[CPGN_EF::sc1to8] = p_254.pgn[CPGN_FE::sc1to8];
        p_229.pgn[CPGN_E5::sc1to8] = p_254.pgn[CPGN_FE::sc1to8];
        p_229.pgn[CPGN_E5::sc9to16] = p_254.pgn[CPGN_FE::sc9to16];
        p_229.pgn[CPGN_E5::toolLSpeed] = (char)(tool.farLeftSpeed * 10);
        p_229.pgn[CPGN_E5::toolRSpeed] = (char)(tool.farRightSpeed * 10);
    }
    else
    {
        //zero all the bytes - set only if on
        for (int i = 5; i < 13; i++)
        {
            p_229.pgn[i] = 0;
        }

        int number = 0;
        for (int k = 0; k < 8; k++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (tool.section[j + k * 8].isSectionOn)
                    number |= 1 << j;
            }
            p_229.pgn[5 + k] = (char)number;
            number = 0;
        }

        //tool speed to calc ramp
        p_229.pgn[CPGN_E5::toolLSpeed] = (char)(tool.farLeftSpeed * 10);
        p_229.pgn[CPGN_E5::toolRSpeed] = (char)(tool.farRightSpeed * 10);

        p_239.pgn[CPGN_EF::sc1to8] = p_229.pgn[CPGN_E5::sc1to8];
        p_239.pgn[CPGN_EF::sc9to16] = p_229.pgn[CPGN_E5::sc9to16];

        p_254.pgn[CPGN_FE::sc1to8] = p_229.pgn[CPGN_E5::sc1to8];
        p_254.pgn[CPGN_FE::sc9to16] = p_229.pgn[CPGN_E5::sc9to16];

    }

    p_239.pgn[CPGN_EF::speed] = (char)(CVehicle::instance()->avgSpeed * 10);
    p_239.pgn[CPGN_EF::tram] = (char)tram.controlByte;

    emit CModuleComm::instance()->p_239_changed();
    emit CModuleComm::instance()->p_254_changed();
}

void FormGPS::DoRemoteSwitches()
{
    //MTZ8302 Feb 2020

    CModuleComm &mc = *CModuleComm::instance();

    // Check if AgIOService is ON - if OFF, skip all hardware switch processing
    SettingsManager* settings = SettingsManager::instance();
    if (!settings->feature_isAgIOOn()) {
        // AgIOService is OFF - manual QML controls have priority
        return;
    }

    bool sectionsChanged = false; // Track if any section state changed
    if (isJobStarted())
    {
        //MainSW was used
        if (mc.ss[mc.swMain] != mc.ssP[mc.swMain])
        {
            //Main SW pressed
            if ((mc.ss[mc.swMain] & 1) == 1)
            {
                this->setAutoBtnState(btnStates::Off);
            } // if Main SW ON

            //if Main SW in Arduino is pressed OFF
            if ((mc.ss[mc.swMain] & 2) == 2)
            {
                this->setAutoBtnState(btnStates::Auto);
            } // if Main SW OFF

            mc.ssP[mc.swMain] = mc.ss[mc.swMain];
        }  //Main or Rate SW


        if (tool.isSectionsNotZones)
        {
            if (mc.ss[mc.swOnGr0] != 0)
            {
                // ON Signal from Arduino
                if ((mc.ss[mc.swOnGr0] & 128) == 128 && tool.numOfSections > 7)
                {
                    tool.sectionButtonState[7] = btnStates::On;
                    tool.section[7].sectionBtnState = btnStates::On;
                    sectionsChanged = true;
                    //TODO: not sure why we have redundant states like that
                }
                if ((mc.ss[mc.swOnGr0] & 64) == 64 && tool.numOfSections > 6)
                {
                    tool.sectionButtonState[6] = btnStates::On;
                    tool.section[6].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr0] & 32) == 32 && tool.numOfSections > 5)
                {
                    tool.sectionButtonState[5] = btnStates::On;
                    tool.section[5].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr0] & 16) == 16 && tool.numOfSections > 4)
                {
                    tool.sectionButtonState[4] = btnStates::On;
                    tool.section[4].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr0] & 8) == 8 && tool.numOfSections > 3)
                {
                    tool.sectionButtonState[3] = btnStates::On;
                    tool.section[3].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr0] & 4) == 4 && tool.numOfSections > 2)
                {
                    tool.sectionButtonState[2] = btnStates::On;
                    tool.section[2].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr0] & 2) == 2 && tool.numOfSections > 1)
                {
                    tool.sectionButtonState[1] = btnStates::On;
                    tool.section[1].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr0] & 1) == 1)
                {
                    tool.sectionButtonState[0] = btnStates::On;
                    tool.section[0].sectionBtnState = btnStates::On;
                    sectionsChanged = true;
                }
                mc.ssP[mc.swOnGr0] = mc.ss[mc.swOnGr0];
            } //if swONLo != 0
            else { if (mc.ssP[mc.swOnGr0] != 0) { mc.ssP[mc.swOnGr0] = 0; } }

            if (mc.ss[mc.swOnGr1] != 0)
            {
                // sections ON signal from Arduino
                if ((mc.ss[mc.swOnGr1] & 128) == 128 && tool.numOfSections > 15)
                {
                    tool.sectionButtonState[15] = btnStates::On;
                    tool.section[15].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr1] & 64) == 64 && tool.numOfSections > 14)
                {
                    tool.sectionButtonState[14] = btnStates::On;
                    tool.section[14].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr1] & 32) == 32 && tool.numOfSections > 13)
                {
                    tool.sectionButtonState[13] = btnStates::On;
                    tool.section[13].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr1] & 16) == 16 && tool.numOfSections > 12)
                {
                    tool.sectionButtonState[12] = btnStates::On;
                    tool.section[12].sectionBtnState = btnStates::On;
                }

                if ((mc.ss[mc.swOnGr1] & 8) == 8 && tool.numOfSections > 11)
                {
                    tool.sectionButtonState[11] = btnStates::On;
                    tool.section[11].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr1] & 4) == 4 && tool.numOfSections > 10)
                {
                    tool.sectionButtonState[10] = btnStates::On;
                    tool.section[10].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr1] & 2) == 2 && tool.numOfSections > 9)
                {
                    tool.sectionButtonState[9] = btnStates::On;
                    tool.section[9].sectionBtnState = btnStates::On;
                }
                if ((mc.ss[mc.swOnGr1] & 1) == 1 && tool.numOfSections > 8)
                {
                    tool.sectionButtonState[8] = btnStates::On;
                    tool.section[8].sectionBtnState = btnStates::On;
                }
                mc.ssP[mc.swOnGr1] = mc.ss[mc.swOnGr1];
            } //if swONHi != 0
            else { if (mc.ssP[mc.swOnGr1] != 0) { mc.ssP[mc.swOnGr1] = 0; } }

            // Switches have changed
            if (mc.ss[mc.swOffGr0] != mc.ssP[mc.swOffGr0])
            {
                //if Main = Auto then change section to Auto if Off signal from Arduino stopped
                if (this->autoBtnState() == btnStates::Auto)
                {

                    for(int s=0; s< 8; s++) {
                        if ((mc.ssP[mc.swOffGr0] & (1 << s)) && !(mc.ss[mc.swOffGr0] & (1 << s)) && (tool.sectionButtonState[s] == btnStates::Off))
                        {
                            tool.sectionButtonState[s] = btnStates::Auto;
                            tool.section[s].sectionBtnState = btnStates::Auto;
                            sectionsChanged = true;
                        }
                    }
                }
                mc.ssP[mc.swOffGr0] = mc.ss[mc.swOffGr0];
            }

            if (mc.ss[mc.swOffGr1] != mc.ssP[mc.swOffGr1])
            {
                //if Main = Auto then change section to Auto if Off signal from Arduino stopped
                if (this->autoBtnState() == btnStates::Auto)
                {
                    for(int s=8; s< 16; s++) {
                        if ((mc.ssP[mc.swOffGr1] & (1 << s)) && !(mc.ss[mc.swOffGr1] & (1 << s)) && (tool.sectionButtonState[s+8] == btnStates::Off))
                        {
                            tool.sectionButtonState[s+8] = btnStates::Auto;
                            tool.section[s+8].sectionBtnState = btnStates::Auto;
                            sectionsChanged = true;
                        }
                    }
                }
                mc.ssP[mc.swOffGr1] = mc.ss[mc.swOffGr1];
            }

            // OFF Signal from Arduino
            if (mc.ss[mc.swOffGr0] != 0)
            {
                //if section SW in Arduino is switched to OFF; check always, if switch is locked to off GUI should not change
                for(int s=0; s< 8; s++) {
                    if ((mc.ss[mc.swOffGr0] & (1 << s)) && tool.sectionButtonState[s] != btnStates::Off)
                    {
                        // Hardware switch override
                        tool.sectionButtonState[s] = btnStates::Off;
                        tool.section[s].sectionBtnState = btnStates::Off;
                        sectionsChanged = true;
                    }
                }
            } // if swOFFLo !=0
            if (mc.ss[mc.swOffGr1] != 0)
            {
                //if section SW in Arduino is switched to OFF; check always, if switch is locked to off GUI should not change
                for (int s=0; s<8; s++) {
                    if ((mc.ss[mc.swOffGr0] & (1 << s)) && tool.sectionButtonState[s+8] != btnStates::Off)
                    {
                        tool.sectionButtonState[s+8] = btnStates::Off;
                        tool.section[s+8].sectionBtnState = btnStates::Off;
                        sectionsChanged = true;
                    }
                }
            } // if swOFFHi !=0
        }//if serial or udp port open
        else
        {
            //DoZones
            int Bit;
            // zones to on
            if (mc.ss[mc.swOnGr0] != 0)
            {
                for (int i = 0; i < 8; i++)
                {
                    Bit = 1 << i;
                    if ((tool.zoneRanges[i + 1] > 0) && ((mc.ss[mc.swOnGr0] & Bit) == Bit))
                    {
                        tool.section[tool.zoneRanges[i + 1] - 1].sectionBtnState = btnStates::On;
                        tool.sectionButtonState[tool.zoneRanges[i + 1] - 1] = btnStates::On;
                        sectionsChanged = true;
                    }
                }

                mc.ssP[mc.swOnGr0] = mc.ss[mc.swOnGr0];
            }
            else { if (mc.ssP[mc.swOnGr0] != 0) { mc.ssP[mc.swOnGr0] = 0; } }

            // zones to auto
            if (mc.ss[mc.swOffGr0] != mc.ssP[mc.swOffGr0])
            {
                if (this->autoBtnState() == btnStates::Auto)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        Bit = 1 << i;
                        if ((tool.zoneRanges[i + 1] > 0) && ((mc.ssP[mc.swOffGr0] & Bit) == Bit)
                            && ((mc.ss[mc.swOffGr0] & Bit) != Bit) && (tool.section[tool.zoneRanges[i + 1] - 1].sectionBtnState == btnStates::Off))
                        {
                            tool.section[tool.zoneRanges[i + 1] - 1].sectionBtnState = btnStates::Auto;
                            tool.sectionButtonState[tool.zoneRanges[i + 1] - 1] = btnStates::Auto;
                            sectionsChanged = true;
                        }
                    }
                }
                mc.ssP[mc.swOffGr0] = mc.ss[mc.swOffGr0];
            }

            // zones to off
            if (mc.ss[mc.swOffGr0] != 0)
            {
                for (int i = 0; i < 8; i++)
                {
                    Bit = 1 << i;
                    if ((tool.zoneRanges[i + 1] > 0) && ((mc.ss[mc.swOffGr0] & Bit) == Bit) && (tool.section[tool.zoneRanges[i + 1] - 1].sectionBtnState != btnStates::Off))
                    {
                        tool.section[tool.zoneRanges[i + 1] - 1].sectionBtnState = btnStates::Off;
                        tool.sectionButtonState[tool.zoneRanges[i + 1] - 1] = btnStates::Off;
                        sectionsChanged = true;
                    }
                }
            }
        }
    }

    // Qt BINDABLE: Property binding handles automatic QML synchronization
    // No manual sync needed - BINDABLE properties update automatically
}
