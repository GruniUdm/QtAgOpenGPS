// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Main class where everything is initialized
#include "formgps.h"
#include <QColor>
#include <QRgb>
#include "qmlutil.h"
#include "glm.h"
#include "cpgn.h"
#include <QLocale>
#include <QLabel>
#include <QQuickWindow>
#include "classes/settingsmanager.h"
#include <cmath>

extern QLabel *grnPixelsWindow;
extern QLabel *overlapPixelsWindow;

FormGPS::FormGPS(QWidget *parent) : QQmlApplicationEngine(parent)
{
    qDebug() << "🚀 FormGPS constructor START";

    qDebug() << "🔗 Setting up basic connections...";
    connect(this,SIGNAL(do_processSectionLookahead()), this, SLOT(processSectionLookahead()), Qt::QueuedConnection);
    connect(this,SIGNAL(do_processOverlapCount()), this, SLOT(processOverlapCount()), Qt::QueuedConnection);
    
    qDebug() << "🔧 Setting up AgIO service FIRST...";
    setupAgIOService();
    
    qDebug() << "🎯 Initializing singletons...";
    // ===== CRITIQUE : Initialiser les singletons AVANT connect_classes() =====
    // CTrack will be auto-initialized via QML singleton pattern
    qDebug() << "  ✅ CTrack singleton will be auto-created by Qt";
    
    vehicle = CVehicle::instance();
    qDebug() << "  ✅ CVehicle singleton created:" << vehicle;

    qDebug() << "🔗 Now calling connect_classes...";
    connect_classes(); //make all the inter-class connections (NOW trk is initialized!)
    
    qDebug() << "🎨 Calling setupGui...";
    setupGui();
    
    // Initialize AgIO singleton AFTER FormLoop is ready
    // Old QMLSettings removed - now using AgIOService singleton
    // Pure Qt 6.8 approach - factory function should be called automatically
    
    qDebug() << "  ✅ AgIO service initialized in main thread";
    //loadSettings();

}

FormGPS::~FormGPS()
{
    qDebug() << "🔧 FormGPS destructor START";
    
    /* clean up our dynamically-allocated
     * objects.
     */
    
    // Clean up AgIO service
    cleanupAgIOService();
    
    qDebug() << "✅ FormGPS destructor END";
}

//This used to be part of oglBack_paint in the C# code, but
//because openGL rendering can potentially be in another thread here, it's
//broken out here.  So the lookaheadPixels array has been populated already
//by the rendering routine.
void FormGPS::processSectionLookahead() {
    //qDebug() << "frame time before doing section lookahead " << swFrame.elapsed();
    //lock.lockForWrite();
    //qDebug() << "frame time after getting lock  " << swFrame.elapsed();

    if (SettingsManager::instance()->value(SETTINGS_display_showBack).value<bool>()) {
        grnPixelsWindow->setPixmap(QPixmap::fromImage(grnPix.mirrored()));
        overlapPixelsWindow->setPixmap(QPixmap::fromImage(overPix.mirrored()));
    }

    //determine where the tool is wrt to headland
    if (bnd.isHeadlandOn) bnd.WhereAreToolCorners(tool);

    //set the look ahead for hyd Lift in pixels per second
    CVehicle::instance()->hydLiftLookAheadDistanceLeft = tool.farLeftSpeed * CVehicle::instance()->hydLiftLookAheadTime * 10;
    CVehicle::instance()->hydLiftLookAheadDistanceRight = tool.farRightSpeed * CVehicle::instance()->hydLiftLookAheadTime * 10;

    if (CVehicle::instance()->hydLiftLookAheadDistanceLeft > 200) CVehicle::instance()->hydLiftLookAheadDistanceLeft = 200;
    if (CVehicle::instance()->hydLiftLookAheadDistanceRight > 200) CVehicle::instance()->hydLiftLookAheadDistanceRight = 200;

    tool.lookAheadDistanceOnPixelsLeft = tool.farLeftSpeed * tool.lookAheadOnSetting * 10;
    tool.lookAheadDistanceOnPixelsRight = tool.farRightSpeed * tool.lookAheadOnSetting * 10;

    if (tool.lookAheadDistanceOnPixelsLeft > 200) tool.lookAheadDistanceOnPixelsLeft = 200;
    if (tool.lookAheadDistanceOnPixelsRight > 200) tool.lookAheadDistanceOnPixelsRight = 200;

    tool.lookAheadDistanceOffPixelsLeft = tool.farLeftSpeed * tool.lookAheadOffSetting * 10;
    tool.lookAheadDistanceOffPixelsRight = tool.farRightSpeed * tool.lookAheadOffSetting * 10;

    if (tool.lookAheadDistanceOffPixelsLeft > 160) tool.lookAheadDistanceOffPixelsLeft = 160;
    if (tool.lookAheadDistanceOffPixelsRight > 160) tool.lookAheadDistanceOffPixelsRight = 160;

    //determine if section is in boundary and headland using the section left/right positions
    bool isLeftIn = true, isRightIn = true;

    if (bnd.bndList.count() > 0)
    {
        for (int j = 0; j < tool.numOfSections; j++)
        {
            //only one first left point, the rest are all rights moved over to left
            isLeftIn = j == 0 ? bnd.IsPointInsideFenceArea(tool.section[j].leftPoint) : isRightIn;
            isRightIn = bnd.IsPointInsideFenceArea(tool.section[j].rightPoint);

            if (tool.isSectionOffWhenOut)
            {
                //merge the two sides into in or out
                if (isLeftIn || isRightIn) tool.section[j].isInBoundary = true;
                else tool.section[j].isInBoundary = false;
            }
            else
            {
                //merge the two sides into in or out
                if (!isLeftIn || !isRightIn) tool.section[j].isInBoundary = false;
                else tool.section[j].isInBoundary = true;
            }
        }
    }

    //determine farthest ahead lookahead - is the height of the readpixel line
    double rpHeight = 0;
    double rpOnHeight = 0;
    double rpToolHeight = 0;

    //pick the larger side
    if (CVehicle::instance()->hydLiftLookAheadDistanceLeft > CVehicle::instance()->hydLiftLookAheadDistanceRight) rpToolHeight = CVehicle::instance()->hydLiftLookAheadDistanceLeft;
    else rpToolHeight = CVehicle::instance()->hydLiftLookAheadDistanceRight;

    if (tool.lookAheadDistanceOnPixelsLeft > tool.lookAheadDistanceOnPixelsRight) rpOnHeight = tool.lookAheadDistanceOnPixelsLeft;
    else rpOnHeight = tool.lookAheadDistanceOnPixelsRight;

    isHeadlandClose = false;

    //clamp the height after looking way ahead, this is for switching off super section only
    rpOnHeight = fabs(rpOnHeight);
    rpToolHeight = fabs(rpToolHeight);

    //10 % min is required for overlap, otherwise it never would be on.
    int pixLimit = (int)((double)(tool.section[0].rpSectionWidth * rpOnHeight) / (double)(5.0));
    //bnd.isSectionControlledByHeadland = true;
    if ((rpOnHeight < rpToolHeight && bnd.isHeadlandOn && bnd.isSectionControlledByHeadland)) rpHeight = rpToolHeight + 2;
    else rpHeight = rpOnHeight + 2;
    //qDebug() << bnd.isSectionControlledByHeadland << "headland sections";

    if (rpHeight > 290) rpHeight = 290;
    if (rpHeight < 8) rpHeight = 8;

    //read the whole block of pixels up to max lookahead, one read only
    //pixels are already read in another thread.

    //determine if headland is in read pixel buffer left middle and right.
    int start = 0, end = 0, tagged = 0, totalPixel = 0;

    //slope of the look ahead line
    double mOn = 0, mOff = 0;

    //tram and hydraulics
    if (tram.displayMode > 0 && tool.width > CVehicle::instance()->trackWidth)
    {
        tram.controlByte = 0;
        //1 pixels in is there a tram line?
        if (tram.isOuter)
        {
            if (grnPixels[(int)(tram.halfWheelTrack * 10)].green == 245) tram.controlByte += 2;
            if (grnPixels[tool.rpWidth - (int)(tram.halfWheelTrack * 10)].green == 245) tram.controlByte += 1;
        }
        else
        {
            if (grnPixels[tool.rpWidth / 2 - (int)(tram.halfWheelTrack * 10)].green == 245) tram.controlByte += 2;
            if (grnPixels[tool.rpWidth / 2 + (int)(tram.halfWheelTrack * 10)].green == 245) tram.controlByte += 1;
        }
    }
    else tram.controlByte = 0;

    //determine if in or out of headland, do hydraulics if on
    if (bnd.isHeadlandOn)
    {
        //calculate the slope
        double m = (CVehicle::instance()->hydLiftLookAheadDistanceRight - CVehicle::instance()->hydLiftLookAheadDistanceLeft) / tool.rpWidth;
        int height = 1;

        for (int pos = 0; pos < tool.rpWidth; pos++)
        {
            height = (int)(CVehicle::instance()->hydLiftLookAheadDistanceLeft + (m * pos)) - 1;
            for (int a = pos; a < height * tool.rpWidth; a += tool.rpWidth)
            {
                if (grnPixels[a].green == 250)
                {
                    isHeadlandClose = true;
                    goto GetOutTool;
                }
            }
        }

    GetOutTool: //goto

        //is the tool completely in the headland or not
        bnd.isToolInHeadland = bnd.isToolOuterPointsInHeadland && !isHeadlandClose;

        //set hydraulics based on tool in headland or not
        bnd.SetHydPosition(autoBtnState, p_239, *CVehicle::instance());

        //set hydraulics based on tool in headland or not
        bnd.SetHydPosition(autoBtnState, p_239, *CVehicle::instance());

    }

    ///////////////////////////////////////////   Section control        ssssssssssssssssssssss

    int endHeight = 1, startHeight = 1;

    if (bnd.isHeadlandOn && bnd.isSectionControlledByHeadland) bnd.WhereAreToolLookOnPoints(*CVehicle::instance(), tool);

    for (int j = 0; j < tool.numOfSections; j++)
    {
        //Off or too slow or going backwards
        if (tool.sectionButtonState.get(j) == btnStates::Off || CVehicle::instance()->avgSpeed < CVehicle::instance()->slowSpeedCutoff || tool.section[j].speedPixels < 0)
        {
            tool.section[j].sectionOnRequest = false;
            tool.section[j].sectionOffRequest = true;

            // Manual on, force the section On
            if (tool.sectionButtonState.get(j) == btnStates::On)
            {
                tool.section[j].sectionOnRequest = true;
                tool.section[j].sectionOffRequest = false;
                continue;
            }
            continue;
        }

        // Manual on, force the section On
        if (tool.sectionButtonState.get(j) == btnStates::On)
        {
            tool.section[j].sectionOnRequest = true;
            tool.section[j].sectionOffRequest = false;
            continue;
        }


        //AutoSection - If any nowhere applied, send OnRequest, if its all green send an offRequest
        tool.section[j].isSectionRequiredOn = false;

        //calculate the slopes of the lines
        mOn = (tool.lookAheadDistanceOnPixelsRight - tool.lookAheadDistanceOnPixelsLeft) / tool.rpWidth;
        mOff = (tool.lookAheadDistanceOffPixelsRight - tool.lookAheadDistanceOffPixelsLeft) / tool.rpWidth;

        start = tool.section[j].rpSectionPosition - tool.section[0].rpSectionPosition;
        end = tool.section[j].rpSectionWidth - 1 + start;

        if (end >= tool.rpWidth)
            end = tool.rpWidth - 1;

        totalPixel = 1;
        tagged = 0;

        for (int pos = start; pos <= end; pos++)
        {
            startHeight = (int)(tool.lookAheadDistanceOffPixelsLeft + (mOff * pos)) * tool.rpWidth + pos;
            endHeight = (int)(tool.lookAheadDistanceOnPixelsLeft + (mOn * pos)) * tool.rpWidth + pos;

            for (int a = startHeight; a <= endHeight; a += tool.rpWidth)
            {
                totalPixel++;
                if (grnPixels[a].green == 0) tagged++;
            }
        }

        //determine if meeting minimum coverage
        tool.section[j].isSectionRequiredOn = ((tagged * 100) / totalPixel > (100 - tool.minCoverage));

        //logic if in or out of boundaries or headland
        if (bnd.bndList.count() > 0)
        {
            //if out of boundary, turn it off
            if (!tool.section[j].isInBoundary)
            {
                tool.section[j].isSectionRequiredOn = false;
                tool.section[j].sectionOffRequest = true;
                tool.section[j].sectionOnRequest = false;
                tool.section[j].sectionOffTimer = 0;
                tool.section[j].sectionOnTimer = 0;
                continue;
            }
            else
            {
                //is headland coming up
                if (bnd.isHeadlandOn && bnd.isSectionControlledByHeadland)
                {
                    bool isHeadlandInLookOn = false;

                    //is headline in off to on area
                    mOn = (tool.lookAheadDistanceOnPixelsRight - tool.lookAheadDistanceOnPixelsLeft) / tool.rpWidth;
                    mOff = (tool.lookAheadDistanceOffPixelsRight - tool.lookAheadDistanceOffPixelsLeft) / tool.rpWidth;

                    start = tool.section[j].rpSectionPosition - tool.section[0].rpSectionPosition;

                    end = tool.section[j].rpSectionWidth - 1 + start;

                    if (end >= tool.rpWidth)
                        end = tool.rpWidth - 1;

                    tagged = 0;

                    for (int pos = start; pos <= end; pos++)
                    {
                        startHeight = (int)(tool.lookAheadDistanceOffPixelsLeft + (mOff * pos)) * tool.rpWidth + pos;
                        endHeight = (int)(tool.lookAheadDistanceOnPixelsLeft + (mOn * pos)) * tool.rpWidth + pos;

                        for (int a = startHeight; a <= endHeight; a += tool.rpWidth)
                        {
                            if (a < 0)
                                mOn = 0;
                            if (grnPixels[a].green == 250)
                            {
                                isHeadlandInLookOn = true;
                                goto GetOutHdOn;
                            }
                        }
                    }
                GetOutHdOn:

                    //determine if look ahead points are completely in headland
                    if (tool.section[j].isSectionRequiredOn && tool.section[j].isLookOnInHeadland && !isHeadlandInLookOn)
                    {
                        tool.section[j].isSectionRequiredOn = false;
                        tool.section[j].sectionOffRequest = true;
                        tool.section[j].sectionOnRequest = false;
                    }

                    if (tool.section[j].isSectionRequiredOn && !tool.section[j].isLookOnInHeadland && isHeadlandInLookOn)
                    {
                        tool.section[j].isSectionRequiredOn = true;
                        tool.section[j].sectionOffRequest = false;
                        tool.section[j].sectionOnRequest = true;
                    }
                }
            }
        }


        //global request to turn on section
        tool.section[j].sectionOnRequest = tool.section[j].isSectionRequiredOn;
        tool.section[j].sectionOffRequest = !tool.section[j].sectionOnRequest;

    }  // end of go thru all sections "for"

    //Set all the on and off times based from on off section requests
    for (int j = 0; j < tool.numOfSections; j++)
    {
        //SECTION timers

        if (tool.section[j].sectionOnRequest)
            tool.section[j].isSectionOn = true;

        //turn off delay
        if (tool.turnOffDelay > 0)
        {
            if (!tool.section[j].sectionOffRequest) tool.section[j].sectionOffTimer = (int)(gpsHz / 2.0 * tool.turnOffDelay);

            if (tool.section[j].sectionOffTimer > 0) tool.section[j].sectionOffTimer--;

            if (tool.section[j].sectionOffRequest && tool.section[j].sectionOffTimer == 0)
            {
                if (tool.section[j].isSectionOn) tool.section[j].isSectionOn = false;
            }
        }
        else
        {
            if (tool.section[j].sectionOffRequest)
                tool.section[j].isSectionOn = false;
        }

        //Mapping timers
        if (tool.section[j].sectionOnRequest && !tool.section[j].isMappingOn && tool.section[j].mappingOnTimer == 0)
        {
            tool.section[j].mappingOnTimer = (int)(tool.lookAheadOnSetting * (gpsHz / 2) - 1);
        }
        else if (tool.section[j].sectionOnRequest && tool.section[j].isMappingOn && tool.section[j].mappingOffTimer > 1)
        {
            tool.section[j].mappingOffTimer = 0;
            tool.section[j].mappingOnTimer = (int)(tool.lookAheadOnSetting * (gpsHz / 2) - 1);
        }

        if (tool.lookAheadOffSetting > 0)
        {
            if (tool.section[j].sectionOffRequest && tool.section[j].isMappingOn && tool.section[j].mappingOffTimer == 0)
            {
                tool.section[j].mappingOffTimer = (int)(tool.lookAheadOffSetting * (gpsHz / 2) + 4);
            }
        }
        else if (tool.turnOffDelay > 0)
        {
            if (tool.section[j].sectionOffRequest && tool.section[j].isMappingOn && tool.section[j].mappingOffTimer == 0)
                tool.section[j].mappingOffTimer = (int)(tool.turnOffDelay * gpsHz / 2);
        }
        else
        {
            tool.section[j].mappingOffTimer = 0;
        }

        //MAPPING - Not the making of triangle patches - only status - on or off
        if (tool.section[j].sectionOnRequest)
        {
            tool.section[j].mappingOffTimer = 0;
            if (tool.section[j].mappingOnTimer > 1)
                tool.section[j].mappingOnTimer--;
            else
            {
                tool.section[j].isMappingOn = true;
            }
        }

        if (tool.section[j].sectionOffRequest)
        {
            tool.section[j].mappingOnTimer = 0;
            if (tool.section[j].mappingOffTimer > 1)
                tool.section[j].mappingOffTimer--;
            else
            {
                tool.section[j].isMappingOn = false;
            }
        }
    }

    //Checks the workswitch or steerSwitch if required
    if (ahrs.isAutoSteerAuto || mc.isRemoteWorkSystemOn)
        mc.CheckWorkAndSteerSwitch(ahrs,isBtnAutoSteerOn);

    // check if any sections have changed status
    number = 0;

    for (int j = 0; j < tool.numOfSections; j++)
    {
        if (tool.section[j].isMappingOn)
        {
            number |= 1ul << j;
        }
    }

    //there has been a status change of section on/off
    if (number != lastNumber)
    {
        int sectionOnOffZones = 0, patchingZones = 0;

        //everything off
        if (number == 0)
        {
            for (int j = 0; j < triStrip.count(); j++)
            {
                if (triStrip[j].isDrawing)
                    triStrip[j].TurnMappingOff(tool, fd);
            }
        }
        else if (!tool.isMultiColoredSections)
        {
            //set the start and end positions from section points
            for (int j = 0; j < tool.numOfSections; j++)
            {
                //skip till first mapping section
                if (!tool.section[j].isMappingOn) continue;

                //do we need more patches created
                if (triStrip.count() < sectionOnOffZones + 1)
                    triStrip.append(CPatches());

                //set this strip start edge to edge of this section
                triStrip[sectionOnOffZones].newStartSectionNum = j;

                while ((j + 1) < tool.numOfSections && tool.section[j + 1].isMappingOn)
                {
                    j++;
                }

                //set the edge of this section to be end edge of strp
                triStrip[sectionOnOffZones].newEndSectionNum = j;
                sectionOnOffZones++;
            }

            //count current patch strips being made
            for (int j = 0; j < triStrip.count(); j++)
            {
                if (triStrip[j].isDrawing) patchingZones++;
            }

            //tests for creating new strips or continuing
            bool isOk = (patchingZones == sectionOnOffZones && sectionOnOffZones < 3);

            if (isOk)
            {
                for (int j = 0; j < sectionOnOffZones; j++)
                {
                    if (triStrip[j].newStartSectionNum > triStrip[j].currentEndSectionNum
                        || triStrip[j].newEndSectionNum < triStrip[j].currentStartSectionNum)
                        isOk = false;
                }
            }

            if (isOk)
            {
                for (int j = 0; j < sectionOnOffZones; j++)
                {
                    if (triStrip[j].newStartSectionNum != triStrip[j].currentStartSectionNum
                        || triStrip[j].newEndSectionNum != triStrip[j].currentEndSectionNum)
                    {
                        //if (tool.isSectionsNotZones)
                        {
                            triStrip[j].AddMappingPoint(tool,fd, 0);
                        }

                        triStrip[j].currentStartSectionNum = triStrip[j].newStartSectionNum;
                        triStrip[j].currentEndSectionNum = triStrip[j].newEndSectionNum;
                        triStrip[j].AddMappingPoint(tool,fd, 0);
                    }
                }
            }
            else
            {
                //too complicated, just make new strips
                for (int j = 0; j < triStrip.count(); j++)
                {
                    if (triStrip[j].isDrawing)
                        triStrip[j].TurnMappingOff(tool, fd);
                }

                for (int j = 0; j < sectionOnOffZones; j++)
                {
                    triStrip[j].currentStartSectionNum = triStrip[j].newStartSectionNum;
                    triStrip[j].currentEndSectionNum = triStrip[j].newEndSectionNum;
                    triStrip[j].TurnMappingOn(tool, 0);
                }
            }
        }
        else if (tool.isMultiColoredSections) //could be else only but this is more clear
        {
            //set the start and end positions from section points
            for (int j = 0; j < tool.numOfSections; j++)
            {
                //do we need more patches created
                if (triStrip.count() < sectionOnOffZones + 1)
                    triStrip.append(CPatches());

                //set this strip start edge to edge of this section
                triStrip[sectionOnOffZones].newStartSectionNum = j;

                //set the edge of this section to be end edge of strp
                triStrip[sectionOnOffZones].newEndSectionNum = j;
                sectionOnOffZones++;

                if (!tool.section[j].isMappingOn)
                {
                    if (triStrip[j].isDrawing)
                        triStrip[j].TurnMappingOff(tool, fd);
                }
                else
                {
                    triStrip[j].currentStartSectionNum = triStrip[j].newStartSectionNum;
                    triStrip[j].currentEndSectionNum = triStrip[j].newEndSectionNum;
                    triStrip[j].TurnMappingOn(tool,j);
                }
            }
        }


        lastNumber = number;
    }

    //send the byte out to section machines
    BuildMachineByte();

    //if a minute has elapsed save the field in case of crash and to be able to resume
    if (minuteCounter > 30 && (uint)sentenceCounter < 20)
    {
        tmrWatchdog->stop();

        //don't save if no gps
        if (isJobStarted)
        {
            //auto save the field patches, contours accumulated so far
            FileSaveSections();
            FileSaveContour();

            //NMEA log file
            //TODO: if (isLogElevation) FileSaveElevation();
            //ExportFieldAs_KML();
        }

        //if its the next day, calc sunrise sunset for next day
        minuteCounter = 0;

        //set saving flag off
        isSavingFile = false;

        //go see if data ready for draw and position updates
        tmrWatchdog->start();

        //calc overlap
        //oglZoom.Refresh();

    }

    //stop the timer and calc how long it took to do calcs and draw
    frameTimeRough = swFrame.elapsed();
    //qDebug() << "frame time after finishing section lookahead " << frameTimeRough ;

    if (frameTimeRough > 50) frameTimeRough = 50;
    frameTime = frameTime * 0.90 + frameTimeRough * 0.1;

    QObject *aog = qmlItem(mainWindow, "aog");
    if (aog) {
        aog->setProperty("frameTime", frameTime);
    }

    //TODO 5 hz sections
    //if (bbCounter++ > 0)
    //    bbCounter = 0;

    //draw the section control window off screen buffer
    //if (bbCounter == 0)
    //{
    if (isJobStarted)
    {
        p_239.pgn[p_239.geoStop] = mc.isOutOfBounds ? 1 : 0;

        // SendPgnToLoop(p_239.pgn);  // ❌ REMOVED - Phase 4.6: AgIOService Workers handle PGN
        
        // SendPgnToLoop(p_229.pgn);  // ❌ REMOVED - Phase 4.6: Use AgIOService.sendPgn() instead
        if (m_agioService) {
            m_agioService->sendPgn(p_239.pgn);
            m_agioService->sendPgn(p_229.pgn);
        }
    }


    //lock.unlock();

    //this is the end of the "frame". Now we wait for next NMEA sentence with a valid fix.
}

void FormGPS::processOverlapCount()
{
    if (isJobStarted)
    {
        int once = 0;
        int twice = 0;
        int more = 0;
        int level = 0;
        double total = 0;
        double total2 = 0;

        //50, 96, 112
        for (int i = 0; i < 400 * 400; i++)
        {

            if (overPixels[i].red > 105)
            {
                more++;
                level = overPixels[i].red;
            }
            else if (overPixels[i].red > 85)
            {
                twice++;
                level = overPixels[i].red;
            }
            else if (overPixels[i].red > 50)
            {
                once++;
            }
        }
        total = once + twice + more;
        total2 = total + twice + more + more;

        if (total2 > 0)
        {
            fd.actualAreaCovered = (total / total2 * (double)fd.workedAreaTotal);
            fd.overlapPercent = ((1 - total / total2) * 100);
        }
        else
        {
            fd.actualAreaCovered = 0;
            fd.overlapPercent = 0;
        }
    }
}

void FormGPS::tmrWatchdog_timeout()
{
    //TODO: replace all this with individual timers for cleaner

    if (! SettingsManager::instance()->value(SETTINGS_menu_isSimulatorOn).value<bool>() && timerSim.isActive()) {
        qDebug() << "Shutting down simulator.";
        timerSim.stop();
    } else if ( SettingsManager::instance()->value(SETTINGS_menu_isSimulatorOn).value<bool>() && ! timerSim.isActive() ) {
        qDebug() << "Starting up simulator.";
        pn.latitude = sim.latitude;
        pn.longitude = sim.longitude;
        pn.headingTrue = 0;

        timerSim.start(100); //fire simulator every 100 ms.
        gpsHz = 10;
    }


    // This is done in QML
//    if ((uint)sentenceCounter++ > 20)
//    {
//        //TODO: ShowNoGPSWarning();
//        return;
//    }
    sentenceCounter += 1;


    if (tenSecondCounter++ >= 40)
    {
        tenSecondCounter = 0;
        tenSeconds++;
    }
    if (threeSecondCounter++ >= 12)
    {
        threeSecondCounter = 0;
        threeSeconds++;
    }
    if (oneSecondCounter++ >= 4)
    {
        oneSecondCounter = 0;
        oneSecond++;
    }
    if (oneHalfSecondCounter++ >= 2)
    {
        oneHalfSecondCounter = 0;
        oneHalfSecond++;
    }
    if (oneFifthSecondCounter++ >= 0)
    {
        oneFifthSecondCounter = 0;
        oneFifthSecond++;
    }

    ////////////////////////////////////////////// 10 second ///////////////////////////////////////////////////////
    //every 10 second update status
    if (tenSeconds != 0)
    {
        //reset the counter
        tenSeconds = 0;
    }
    /////////////////////////////////////////////////////////   333333333333333  ////////////////////////////////////////
    //every 3 second update status
    if (displayUpdateThreeSecondCounter != threeSeconds)
    {
        //reset the counter
        displayUpdateThreeSecondCounter = threeSeconds;

        //check to make sure the grid is big enough
        //worldGrid.checkZoomWorldGrid(pn.fix.northing, pn.fix.easting);

        //hide the NAv panel in 6  secs
        /* TODO:
        if (panelNavigation.Visible)
        {
            if (navPanelCounter-- < 2) panelNavigation.Visible = false;
        }

        if (panelNavigation.Visible)
            lblHz.Text = gpsHz.ToString("N1") + " ~ " + (frameTime.ToString("N1")) + " " + FixQuality;

        lblFix.Text = FixQuality + pn.age.ToString("N1");

        lblTime.Text = DateTime.Now.ToString("T");
        */

        //save nmea log file
        //TODO: if (isLogNMEA) FileSaveNMEA();

        //update button lines numbers
        //TODO: UpdateGuidanceLineButtonNumbers();

    }//end every 3 seconds

    //every second update all status ///////////////////////////   1 1 1 1 1 1 ////////////////////////////
    if (displayUpdateOneSecondCounter != oneSecond)
    {
        //reset the counter
        displayUpdateOneSecondCounter = oneSecond;

        //counter used for saving field in background
        minuteCounter++;
        tenMinuteCounter++;
    }

    //every half of a second update all status  ////////////////    0.5  0.5   0.5    0.5    /////////////////
    if (displayUpdateHalfSecondCounter != oneHalfSecond)
    {
        //reset the counter
        displayUpdateHalfSecondCounter = oneHalfSecond;

        isFlashOnOff = !isFlashOnOff;

        //the ratemap trigger
        worldGrid.isRateTrigger = true;

        //Make sure it is off when it should
        if ((!ct.isContourBtnOn && CTrack::instance()->idx == -1 && isBtnAutoSteerOn)
            ) onStopAutoSteer();

    } //end every 1/2 second

    //every fourth second update  ///////////////////////////   Fourth  ////////////////////////////
    {
        //reset the counter
        oneHalfSecondCounter++;
        oneSecondCounter++;
        yt.makeUTurnCounter++;

        secondsSinceStart = stopwatch.elapsed() / 1000.0;
    }
}

QString FormGPS::speedKPH() {
    double spd = CVehicle::instance()->avgSpeed;

    //convert to kph
    spd *= 0.1;

    return locale.toString(spd,'f',1);
}

QString FormGPS::speedMPH() {
    double spd = CVehicle::instance()->avgSpeed;

    //convert to mph
    spd *= 0.0621371;

    return locale.toString(spd,'f',1);
}

void FormGPS::SwapDirection() {
    if (!yt.isYouTurnTriggered)
    {
        yt.isYouTurnRight = ! yt.isYouTurnRight;
        yt.ResetCreatedYouTurn();
    }
    else if (yt.isYouTurnBtnOn)
    {
        yt.isYouTurnBtnOn = false;
    }
}


void FormGPS::JobClose()
{
    lock.lockForWrite();
    recPath.resumeState = 0;
    recPath.currentPositonIndex = 0;

    sbGrid.clear();

    //reset field offsets
    if (!isKeepOffsetsOn)
    {
        pn.fixOffset.easting = 0;
        pn.fixOffset.northing = 0;
    }

    //turn off headland
    bnd.isHeadlandOn = false; //this turns off the button

    recPath.recList.clear();
    recPath.StopDrivingRecordedPath();

    //make sure hydraulic lift is off
    p_239.pgn[p_239.hydLift] = 0;
    CVehicle::instance()->isHydLiftOn = false; //this turns off the button also

    //oglZoom.SendToBack();

    //clean all the lines
    bnd.bndList.clear();
    //TODO: bnd.shpList.clear();


    isJobStarted = false;

    //fix ManualOffOnAuto buttons
    manualBtnState = btnStates::Off;

    //fix auto button
    autoBtnState = btnStates::Off;

    /*
    btnZone1.BackColor = Color.Silver;
    btnZone2.BackColor = Color.Silver;
    btnZone3.BackColor = Color.Silver;
    btnZone4.BackColor = Color.Silver;
    btnZone5.BackColor = Color.Silver;
    btnZone6.BackColor = Color.Silver;
    btnZone7.BackColor = Color.Silver;
    btnZone8.BackColor = Color.Silver;

    btnZone1.Enabled = false;
    btnZone2.Enabled = false;
    btnZone3.Enabled = false;
    btnZone4.Enabled = false;
    btnZone5.Enabled = false;
    btnZone6.Enabled = false;
    btnZone7.Enabled = false;
    btnZone8.Enabled = false;

    btnSection1Man.Enabled = false;
    btnSection2Man.Enabled = false;
    btnSection3Man.Enabled = false;
    btnSection4Man.Enabled = false;
    btnSection5Man.Enabled = false;
    btnSection6Man.Enabled = false;
    btnSection7Man.Enabled = false;
    btnSection8Man.Enabled = false;
    btnSection9Man.Enabled = false;
    btnSection10Man.Enabled = false;
    btnSection11Man.Enabled = false;
    btnSection12Man.Enabled = false;
    btnSection13Man.Enabled = false;
    btnSection14Man.Enabled = false;
    btnSection15Man.Enabled = false;
    btnSection16Man.Enabled = false;

    btnSection1Man.BackColor = Color.Silver;
    btnSection2Man.BackColor = Color.Silver;
    btnSection3Man.BackColor = Color.Silver;
    btnSection4Man.BackColor = Color.Silver;
    btnSection5Man.BackColor = Color.Silver;
    btnSection6Man.BackColor = Color.Silver;
    btnSection7Man.BackColor = Color.Silver;
    btnSection8Man.BackColor = Color.Silver;
    btnSection9Man.BackColor = Color.Silver;
    btnSection10Man.BackColor = Color.Silver;
    btnSection11Man.BackColor = Color.Silver;
    btnSection12Man.BackColor = Color.Silver;
    btnSection13Man.BackColor = Color.Silver;
    btnSection14Man.BackColor = Color.Silver;
    btnSection15Man.BackColor = Color.Silver;
    btnSection16Man.BackColor = Color.Silver;
    */

    //clear the section lists
    for (int j = 0; j < triStrip.count(); j++)
    {
        //clean out the lists
        triStrip[j].patchList.clear();
        triStrip[j].triangleList.clear();
    }

    triStrip.clear();
    triStrip.append(CPatches());

    //clear the flags
    flagPts.clear();

    //ABLine
    tram.tramList.clear();

    CTrack::instance()->ResetCurveLine();

    //tracks
    CTrack::instance()->gArr.clear();
    CTrack::instance()->idx = -1;

    //clean up tram
    tram.displayMode = 0;
    tram.generateMode = 0;
    tram.tramBndInnerArr.clear();
    tram.tramBndOuterArr.clear();

    //clear out contour and Lists
    ct.ResetContour();
    ct.isContourBtnOn = false; //turns off button in gui
    ct.isContourOn = false;

    //btnABDraw.Enabled = false;
    //btnCycleLines.Image = Properties.Resources.ABLineCycle;
    //btnCycleLines.Enabled = false;
    //btnCycleLinesBk.Image = Properties.Resources.ABLineCycleBk;
    //btnCycleLinesBk.Enabled = false;

    //AutoSteer
    //btnAutoSteer.Enabled = false;
    isBtnAutoSteerOn = false;

    //auto YouTurn shutdown
    yt.isYouTurnBtnOn = false;

    yt.ResetYouTurn();

    //reset acre and distance counters
    fd.workedAreaTotal = 0;

    //reset GUI areas
    fd.UpdateFieldBoundaryGUIAreas(bnd.bndList);

    displayFieldName = tr("None");

    recPath.recList.clear();
    recPath.shortestDubinsList.clear();
    recPath.shuttleDubinsList.clear();

    //FixPanelsAndMenus();
    camera.SetZoom();
    worldGrid.isGeoMap = false;
    worldGrid.isRateMap = false;

    //release Bing texture
    lock.unlock();
}

void FormGPS::JobNew()
{
    startCounter = 0;

    //btnSectionMasterManual.Enabled = true;
    manualBtnState = btnStates::Off;
    //btnSectionMasterManual.Image = Properties.Resources.ManualOff;

    //btnSectionMasterAuto.Enabled = true;
    autoBtnState = btnStates::Off;
    //btnSectionMasterAuto.Image = Properties.Resources.SectionMasterOff;

    CTrack::instance()->ABLine.abHeading = 0.00;

    camera.SetZoom();
    fileSaveCounter = 25;
    CTrack::instance()->isAutoTrack = false;
    isJobStarted = true;
}

void FormGPS::FileSaveEverythingBeforeClosingField(bool saveVehicle)
{
    qDebug() << "shutting down, saving field items.";

    if (! isJobStarted) return;

    qDebug() << "Test3";
    lock.lockForWrite();
    //turn off contour line if on
    if (ct.isContourOn) ct.StopContourLine(contourSaveList);

    //turn off all the sections
    for (int j = 0; j < tool.numOfSections; j++)
    {
        tool.section[j].sectionOnOffCycle = false;
        tool.section[j].sectionOffRequest = false;
    }

    //turn off patching
    for (int j = 0; j < triStrip.count(); j++)
    {
        if (triStrip[j].isDrawing) triStrip[j].TurnMappingOff(tool, fd);
    }
    lock.unlock();
    qDebug() << "Test4";

    //FileSaveHeadland();
    qDebug() << "Starting FileSaveBoundary()";
    FileSaveBoundary();
    qDebug() << "Starting FileSaveSections()";
    FileSaveSections();
    qDebug() << "Starting FileSaveContour()";
    FileSaveContour();
    qDebug() << "Starting FileSaveTracks()";
    FileSaveTracks();
    qDebug() << "Starting FileSaveFlags()";
    FileSaveFlags();
    qDebug() << "Starting ExportFieldAs_KML()";
    ExportFieldAs_KML();
    qDebug() << "All file operations completed";
    //ExportFieldAs_ISOXMLv3()
    //ExportFieldAs_ISOXMLv4()

    // Save vehicle settings AFTER all field operations complete (conditional)
    qDebug() << "Before vehicle_saveas check, saveVehicle=" << saveVehicle;
    if(saveVehicle && SettingsManager::instance()->value(SETTINGS_vehicle_vehicleName).value<QString>() != "Default Vehicle") {
        QString vehicleName = SettingsManager::instance()->value(SETTINGS_vehicle_vehicleName).value<QString>();
        qDebug() << "Scheduling async vehicle_saveas():" << vehicleName;

        // ASYNC SOLUTION: Defer vehicle_saveas to avoid mutex deadlock during field close
        QTimer::singleShot(100, this, [this, vehicleName]() {
            qDebug() << "Executing async vehicle_saveas():" << vehicleName;
            vehicle_saveas(vehicleName);
            qDebug() << "Async vehicle_saveas() completed";
        });
    } else {
        qDebug() << "Skipping vehicle_saveas (saveVehicle=" << saveVehicle << ")";
    }

    qDebug() << "Before field cleanup";
    //property_setF_CurrentDir = tr("None");
    //currentFieldDirectory = (QString)property_setF_CurrentDir;
    displayFieldName = tr("None");

    qDebug() << "Before JobClose()";
    JobClose();
    qDebug() << "JobClose() completed";
    //Text = "AgOpenGPS";
    qDebug() << "Test5";
}

// AgIO Service Setup Methods
void FormGPS::setupAgIOService()
{
    qDebug() << "🔧 Setting up AgIO service (main thread)...";
    
    // AgIOService will be created automatically by QML factory function
    // This ensures a single instance shared between C++ and QML
    qDebug() << "📍 AgIOService will be created by QML factory on first access";
    
    // Connect FormLoop GPS data to AgIOService AFTER QML initialization
    // This will be done in setupAgIOConnection() called after QML loading
    qDebug() << "✅ AgIOService setup deferred to QML factory pattern";
}

void FormGPS::connectToAgIOFactoryInstance()
{
    qDebug() << "🔗 Connecting to AgIOService factory instance...";
    
    // Get the factory-created singleton instance
    m_agioService = AgIOService::instance();
    
    if (m_agioService) {
        qDebug() << "✅ Connected to AgIOService singleton instance";
        
        // Now connect the Phase 4.2 pipeline: AgIOService → pn → vehicle → OpenGL
        connectFormLoopToAgIOService();
        
        qDebug() << "🔗 Phase 4.2: AgIOService → pn → vehicle pipeline established";
    } else {
        qDebug() << "❌ ERROR: AgIOService singleton not found";
    }
}

void FormGPS::testAgIOConfiguration()
{
    qDebug() << "\n=================================";
    qDebug() << "📋 AgIO Configuration Test";
    qDebug() << "=================================";
    
    QSettings settings("QtAgOpenGPS", "QtAgOpenGPS");
    qDebug() << "📁 Settings file:" << settings.fileName();
    
    // Display NTRIP settings
    qDebug() << "\n🌐 NTRIP Configuration:";
    qDebug() << "  URL:" << settings.value("comm/ntripURL", "").toString();
    qDebug() << "  Mount:" << settings.value("comm/ntripMount", "").toString();
    qDebug() << "  Port:" << settings.value("comm/ntripCasterPort", 2101).toInt();
    qDebug() << "  Enabled:" << settings.value("comm/ntripIsOn", false).toBool();
    qDebug() << "  User:" << (settings.value("comm/ntripUserName", "").toString().isEmpty() ? "none" : "configured");
    
    // Display UDP settings
    qDebug() << "\n📡 UDP Configuration:";
    int ip1 = settings.value("comm/udpIP1", 192).toInt();
    int ip2 = settings.value("comm/udpIP2", 168).toInt();
    int ip3 = settings.value("comm/udpIP3", 5).toInt();
    qDebug() << "  Subnet:" << QString("%1.%2.%3.xxx").arg(ip1).arg(ip2).arg(ip3);
    qDebug() << "  Broadcast:" << QString("%1.%2.%3.255").arg(ip1).arg(ip2).arg(ip3);
    qDebug() << "  Listen Port:" << settings.value("comm/udpListenPort", 9999).toInt();
    qDebug() << "  Send Port:" << settings.value("comm/udpSendPort", 8888).toInt();
    
    qDebug() << "\n🔍 Expected Data Sources:";
    qDebug() << "  1. GPS via UDP on port 9999 (NMEA sentences)";
    qDebug() << "  2. GPS via Serial port (if configured)";
    qDebug() << "  3. NTRIP corrections from" << settings.value("comm/ntripURL", "").toString();
    qDebug() << "  4. AgOpenGPS modules on" << QString("%1.%2.%3.255").arg(ip1).arg(ip2).arg(ip3);
    qDebug() << "=================================\n";
}

void FormGPS::connectFormLoopToAgIOService()
{
    qDebug() << "🔗 Phase 4.2: Connecting AgIOService → pn → vehicle ...";
    
    if (!m_agioService) {
        qDebug() << "❌ Cannot connect: AgIOService is null";
        return;
    }
    
    // PHASE 4.2: Direct connection AgIOService → pn → vehicle
    // This replaces FormLoop progressively as per architecture document
    
    // Connect AgIOService GPS data to pn (CNMEA) for processing
    // SELECTIVE BLOCKING: Only block GPS/position data in simulation mode
    // Allow other communications (steer commands, module status, etc.)
    connect(m_agioService, &AgIOService::gpsDataChanged, this, [this]() {
        if (!m_agioService) return;
        
        // SELECTIVE FIX: Block only GPS/position data when simulator is active
        // Still allow module communications (steer, work, auto buttons, etc.)
        if (timerSim.isActive()) {
            return; // Let simulator handle GPS position exclusively
        }
        
        // Get GPS data from AgIOService (ZERO latency - direct member access)
        double lat = m_agioService->latitude();
        double lon = m_agioService->longitude();
        double heading = m_agioService->heading();
        double speed = m_agioService->speed();
        
        // COMPLETE: Get all missing GPS properties from AgIOService (Qt 6 explicit getters)
        double altitude = m_agioService->altitude();
        double hdop = m_agioService->hdop();
        int satellites = m_agioService->satellites();
        double gpsHz_val = m_agioService->gpsHz();
        double nowHz_val = m_agioService->nowHz();
        
        // Update pn with COMPLETE GPS data (replaces FormLoop data path)
        pn.latitude = lat;
        pn.longitude = lon;
        pn.altitude = altitude;
        pn.hdop = hdop;
        pn.satellitesTracked = satellites;
        
        // Convert WGS84 to local coordinates
        pn.ConvertWGS84ToLocal(lat, lon, pn.fix.northing, pn.fix.easting);
        
        // Update heading, speed and Hz values
        pn.headingTrue = heading;
        pn.speed = speed;
        
        // Update local Hz variables for QML display
        gpsHz = gpsHz_val;
        nowHz = nowHz_val;
        
        // Reset sentence counter (like original FormLoop) - enables GPS display
        sentenceCounter = 0;
        
        // Update AOGInterface properties for QML display (like FormLoop UpdateUIVars)
        QObject *aog = qmlItem(mainWindow, "aog");
        if (aog) {
            aog->setProperty("latitude", lat);
            aog->setProperty("longitude", lon);
            aog->setProperty("heading", heading);
            aog->setProperty("speedKph", speed);
            aog->setProperty("altitude", altitude);
            aog->setProperty("sats", satellites);
            aog->setProperty("hdop", hdop);
            aog->setProperty("gpsHz", gpsHz_val);
            aog->setProperty("nowHz", nowHz_val);
        }
        
        // Trigger position update (this updates vehicle positions)
        UpdateFixPosition();
    }, Qt::QueuedConnection);  // CRITICAL: Force GUI thread execution to prevent threading violation
    
    // Connect AgIOService IMU data to ahrs structure for OpenGL display
    connect(m_agioService, &AgIOService::imuDataChanged, this, [this]() {
        if (!m_agioService) return;
        
        // Update ahrs structure with AgIOService IMU data (replaces FormLoop IMU path)
        // Qt 6 best practice: Use explicit getters instead of property() for better performance
        ahrs.imuRoll = m_agioService->imuRoll() / 10.0;    // Convert from integer x10 to degrees
        ahrs.imuPitch = m_agioService->imuPitch() / 10.0;  // Convert from integer x10 to degrees
        // Get GPS heading for pn.headingTrue (FormGPS will handle the fusion)
        double gpsHeading = m_agioService->heading();  // GPS fix2fix heading in degrees
        
        // Only update IMU heading if IMU is actually connected
        // If no IMU, keep ahrs.imuHeading = 99999 to display "#INV"
        if (m_agioService->imuConnected()) {
            // Real IMU heading from actual IMU sensor (not implemented yet)
            // TODO: Get real IMU heading when available
            ahrs.imuHeading = 99999; // Show #INV until real IMU data available
        } else {
            // No IMU connected - keep default value to show #INV
            ahrs.imuHeading = 99999;
        }
        
        // Update AOGInterface IMU properties for QML display
        QObject *aog = qmlItem(mainWindow, "aog");
        if (aog) {
            aog->setProperty("imuRoll", ahrs.imuRoll);
            aog->setProperty("imuPitch", ahrs.imuPitch);
            aog->setProperty("imuHeading", ahrs.imuHeading);
        }
        
        qDebug() << "🧭 GPS heading available:" << gpsHeading << "° (IMU heading not available from firmware)";
        qDebug() << "🧭 IMU updated: Roll" << ahrs.imuRoll << "° Pitch" << ahrs.imuPitch << "° Heading" << ahrs.imuHeading << "°";
    }, Qt::QueuedConnection);  // CRITICAL: Force GUI thread execution to prevent threading violation
    
    // CRITICAL: Initialize IMU with valid values to replace 88888 invalid marker
    // This fixes the #INV display issue immediately
    if (m_agioService) {
        ahrs.imuRoll = m_agioService->imuRoll() / 10.0;
        ahrs.imuPitch = m_agioService->imuPitch() / 10.0;
        
        double gpsHeading = m_agioService->heading();
        // Initialize IMU with GPS heading for display
        ahrs.imuHeading = gpsHeading; // GPS heading displayed as IMU
        qDebug() << "🔧 GPS heading available at init:" << gpsHeading << "°";
        
        // QML interface will be updated when UpdateFixPosition() is called
        
        qDebug() << "🔧 IMU initialized: Roll" << ahrs.imuRoll << "° Pitch" << ahrs.imuPitch << "° Heading" << ahrs.imuHeading << "°";
    }
    
    qDebug() << "✅ AgIOService → pn → vehicle pipeline established";
    qDebug() << "  Data flow: AgIOService (direct member) → pn → vehicle → OpenGL";
    qDebug() << "  This progressively replaces FormLoop as per Phase 4 architecture";
}

void FormGPS::cleanupAgIOService()
{
    qDebug() << "🔧 Cleaning up AgIO service...";
    
    if (m_agioService) {
        m_agioService->shutdown();
        // Note: Don't delete m_agioService as it's managed by QML singleton system
        m_agioService = nullptr;
        qDebug() << "✅ AgIO service cleaned up";
    }
}
