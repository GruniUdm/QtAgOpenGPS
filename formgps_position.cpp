// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// This runs every time we get a new GPS fix, or sim position
#include "formgps.h"
#include "cnmea.h"
#include "cmodulecomm.h"
#include "ccontour.h"
#include "cvehicle.h"
#include "csection.h"
#include "cboundary.h"
#include "ctrack.h"
#include <QQuickView>
#include <QOpenGLContext>
#include <QPair>
#include <QElapsedTimer>
#include "glm.h"
#include <string>
#include "aogrenderer.h"
#include "cpgn.h"
#include "qmlutil.h"
#include "glutils.h"

//called for every new GPS or simulator position
void FormGPS::UpdateFixPosition()
{
    QLocale locale;

    //swFrame.Stop();
    //Measure the frequency of the GPS updates
    //timeSliceOfLastFix = (double)(swFrame.elapsed()) / 1000;
    lock.lockForWrite(); //stop GL from updating while we calculate a new position

    //get Hz from AgIOService NMEA data if available, otherwise calculate from timeslice
    if (m_agioService && m_agioService->nowHz() > 0) {
        // Use real NMEA Hz values from AgIOService (preferred)
        nowHz = m_agioService->nowHz();
        gpsHz = m_agioService->gpsHz();
    } else {
        // Fallback: calculate from timer (original method)
        //nowHz = 1 / timeSliceOfLastFix;
        nowHz = 1000.0 / swFrame.elapsed(); //convert ms into hz
        if (nowHz > 20) nowHz = 20;
        if (nowHz < 3) nowHz = 3;

        //simple comp filter
        gpsHz = 0.98 * gpsHz + 0.02 * nowHz;
    }

    //Initialization counter
    startCounter++;

    if (!isGPSPositionInitialized)
    {
        InitializeFirstFewGPSPositions();
        lock.unlock();
        return;
    }

    //qDebug() << "Easting " <<  pn.fix.easting << "Northing" <<  pn.fix.northing << "Time " << swFrame.elapsed() << nowHz;

    swFrame.restart();

    pn.speed = pn.vtgSpeed;
    CVehicle::instance()->AverageTheSpeed(pn.speed);

    /*
    //GPS is valid, let's bootstrap the demo field if needed
    if(bootstrap_field)
    {
        fileCreateField();
        fileSaveABLines();
        bootstrap_field = false;
    }
    */

    //#region Heading
    //calculate current heading only when moving, otherwise use last
    if (headingFromSource == "Fix")
    {
        //#region Start

        distanceCurrentStepFixDisplay = glm::Distance(prevDistFix, pn.fix);
        if ((double)(fd.distanceUser += distanceCurrentStepFixDisplay) > 999) fd.distanceUser = 0;
        distanceCurrentStepFixDisplay *= 100;

        prevDistFix = pn.fix;

        if (fabs(CVehicle::instance()->avgSpeed) < 1.5 && !isFirstHeadingSet)
            goto byPass;

        if (!isFirstHeadingSet) //set in steer settings, Stanley
        {
            prevFix.easting = stepFixPts[0].easting; prevFix.northing = stepFixPts[0].northing;

            if (stepFixPts[2].isSet == 0)
            {
                //this is the first position no roll or offset correction
                if (stepFixPts[0].isSet == 0)
                {
                    stepFixPts[0].easting = pn.fix.easting;
                    stepFixPts[0].northing = pn.fix.northing;
                    stepFixPts[0].isSet = 1;
                    lock.unlock();
                    return;
                }

                //and the second
                if (stepFixPts[1].isSet == 0)
                {
                    for (int i = totalFixSteps - 1; i > 0; i--) stepFixPts[i] = stepFixPts[i - 1];
                    stepFixPts[0].easting = pn.fix.easting;
                    stepFixPts[0].northing = pn.fix.northing;
                    stepFixPts[0].isSet = 1;
                    lock.unlock();
                    return;
                }

                //the critcal moment for checking initial direction/heading.
                for (int i = totalFixSteps - 1; i > 0; i--) stepFixPts[i] = stepFixPts[i - 1];
                stepFixPts[0].easting = pn.fix.easting;
                stepFixPts[0].northing = pn.fix.northing;
                stepFixPts[0].isSet = 1;

                gpsHeading = atan2(pn.fix.easting - stepFixPts[2].easting,
                                   pn.fix.northing - stepFixPts[2].northing);

                if (gpsHeading < 0) gpsHeading += glm::twoPI;
                else if (gpsHeading > glm::twoPI) gpsHeading -= glm::twoPI;

                CVehicle::instance()->fixHeading = gpsHeading;

                //set the imu to gps heading offset
                if (ahrs.imuHeading != 99999)
                {
                    double imuHeading = (glm::toRadians(ahrs.imuHeading));
                    imuGPS_Offset = 0;

                    //Difference between the IMU heading and the GPS heading
                    double gyroDelta = (imuHeading + imuGPS_Offset) - gpsHeading;

                    if (gyroDelta < 0) gyroDelta += glm::twoPI;
                    else if (gyroDelta > glm::twoPI) gyroDelta -= glm::twoPI;

                    //calculate delta based on circular data problem 0 to 360 to 0, clamp to +- 2 Pi
                    if (gyroDelta >= -glm::PIBy2 && gyroDelta <= glm::PIBy2) gyroDelta *= -1.0;
                    else
                    {
                        if (gyroDelta > glm::PIBy2) { gyroDelta = glm::twoPI - gyroDelta; }
                        else { gyroDelta = (glm::twoPI + gyroDelta) * -1.0; }
                    }
                    if (gyroDelta > glm::twoPI) gyroDelta -= glm::twoPI;
                    else if (gyroDelta < -glm::twoPI) gyroDelta += glm::twoPI;

                    //moe the offset to line up imu with gps
                    imuGPS_Offset = (gyroDelta);
                    //rounding a floating point number doesn't make sense.
                    //imuGPS_Offset = Math.Round(imuGPS_Offset, 6);

                    if (imuGPS_Offset >= glm::twoPI) imuGPS_Offset -= glm::twoPI;
                    else if (imuGPS_Offset <= 0) imuGPS_Offset += glm::twoPI;

                    //determine the Corrected heading based on gyro and GPS
                    imuCorrected = imuHeading + imuGPS_Offset;
                    if (imuCorrected > glm::twoPI) imuCorrected -= glm::twoPI;
                    else if (imuCorrected < 0) imuCorrected += glm::twoPI;

                    CVehicle::instance()->fixHeading = imuCorrected;
                }

                //set the camera
                camera.camHeading = glm::toDegrees(gpsHeading);

                //now we have a heading, fix the first 3
                if (CVehicle::instance()->antennaOffset != 0)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        stepFixPts[i].easting = (cos(-gpsHeading) * CVehicle::instance()->antennaOffset) + stepFixPts[i].easting;
                        stepFixPts[i].northing = (sin(-gpsHeading) * CVehicle::instance()->antennaOffset) + stepFixPts[i].northing;
                    }
                }

                if (ahrs.imuRoll != 88888)
                {
                    //change for roll to the right is positive times -1
                    rollCorrectionDistance = tan(glm::toRadians((ahrs.imuRoll))) * -CVehicle::instance()->antennaHeight;

                    // roll to left is positive  **** important!!
                    // not any more - April 30, 2019 - roll to right is positive Now! Still Important
                    for (int i = 0; i < 3; i++)
                    {
                        stepFixPts[i].easting = (cos(-gpsHeading) * rollCorrectionDistance) + stepFixPts[i].easting;
                        stepFixPts[i].northing = (sin(-gpsHeading) * rollCorrectionDistance) + stepFixPts[i].northing;
                    }
                }

                //get the distance from first to 2nd point, update fix with new offset/roll
                stepFixPts[0].distance = glm::Distance(stepFixPts[1], stepFixPts[0]);
                pn.fix.easting = stepFixPts[0].easting;
                pn.fix.northing = stepFixPts[0].northing;

                isFirstHeadingSet = true;
                TimedMessageBox(2000, "Direction Reset", "Forward is Set");

                lastGPS = pn.fix;

                lock.unlock();
                return;
            }
        }
        //#endregion

        //#region Offset Roll
        if (CVehicle::instance()->antennaOffset != 0)
        {
            pn.fix.easting = (cos(-gpsHeading) * CVehicle::instance()->antennaOffset) + pn.fix.easting;
            pn.fix.northing = (sin(-gpsHeading) * CVehicle::instance()->antennaOffset) + pn.fix.northing;
        }

        uncorrectedEastingGraph = pn.fix.easting;

        //originalEasting = pn.fix.easting;
        if (ahrs.imuRoll != 88888)
        {
            //change for roll to the right is positive times -1
            rollCorrectionDistance = sin(glm::toRadians((ahrs.imuRoll))) * -CVehicle::instance()->antennaHeight;
            correctionDistanceGraph = rollCorrectionDistance;

            pn.fix.easting = (cos(-gpsHeading) * rollCorrectionDistance) + pn.fix.easting;
            pn.fix.northing = (sin(-gpsHeading) * rollCorrectionDistance) + pn.fix.northing;
        }

        //#endregion

        //#region Fix Heading

        double minFixHeadingDistSquared;
        double newGPSHeading;
        double imuHeading;
        double camDelta;
        double gyroDelta;

        //imu on board
        if (ahrs.imuHeading != 99999)
        {
            //check for out-of bounds fusion weights in case config
            //file was edited and changed inappropriately.
            //TODO move this sort of thing to FormGPS::load_settings
            if (ahrs.fusionWeight > 0.4) ahrs.fusionWeight = 0.4;
            if (ahrs.fusionWeight < 0.2) ahrs.fusionWeight = 0.2;

            //how far since last fix
            distanceCurrentStepFix = glm::Distance(stepFixPts[0], pn.fix);

            if (distanceCurrentStepFix < gpsMinimumStepDistance)
            {
                goto byPass;
            }

            //userDistance can be reset

            minFixHeadingDistSquared = minHeadingStepDist * minHeadingStepDist;
            fixToFixHeadingDistance = 0;

            for (int i = 0; i < totalFixSteps; i++)
            {
                fixToFixHeadingDistance = glm::DistanceSquared(stepFixPts[i], pn.fix);
                currentStepFix = i;

                if (fixToFixHeadingDistance > minFixHeadingDistSquared)
                {
                    break;
                }
            }

            if (fixToFixHeadingDistance < (minFixHeadingDistSquared * 0.5))
                goto byPass;

            newGPSHeading = atan2(pn.fix.easting - stepFixPts[currentStepFix].easting,
                                  pn.fix.northing - stepFixPts[currentStepFix].northing);
            if (newGPSHeading < 0) newGPSHeading += glm::twoPI;

            if (ahrs.isReverseOn)
            {
                ////what is angle between the last valid heading before stopping and one just now
                delta = fabs(M_PI - fabs(fabs(newGPSHeading - imuCorrected) - M_PI));

                //ie change in direction
                if (delta > 1.57) //
                {
                    CVehicle::instance()->setIsReverse(true);
                    newGPSHeading += M_PI;
                    if (newGPSHeading < 0) newGPSHeading += glm::twoPI;
                    else if (newGPSHeading >= glm::twoPI) newGPSHeading -= glm::twoPI;
                    isReverseWithIMU = true;
                }
                else
                {
                    CVehicle::instance()->setIsReverse(false);
                    isReverseWithIMU = false;
                }
            }
            else
            {
                CVehicle::instance()->setIsReverse(false);
            }

            if (CVehicle::instance()->isReverse)
                newGPSHeading -= glm::toRadians(CVehicle::instance()->antennaPivot / 1
                                                * mc.actualSteerAngleDegrees * ahrs.reverseComp);
            else
                newGPSHeading -= glm::toRadians(CVehicle::instance()->antennaPivot / 1
                                                * mc.actualSteerAngleDegrees * ahrs.forwardComp);

            if (newGPSHeading < 0) newGPSHeading += glm::twoPI;
            else if (newGPSHeading >= glm::twoPI) newGPSHeading -= glm::twoPI;

            gpsHeading = newGPSHeading;

            //#region IMU Fusion

            // IMU Fusion with heading correction, add the correction
            //current gyro angle in radians
            imuHeading = (glm::toRadians(ahrs.imuHeading));

            //Difference between the IMU heading and the GPS heading
            gyroDelta = 0;

            //if (!isReverseWithIMU)
            gyroDelta = (imuHeading + imuGPS_Offset) - gpsHeading;
            //else
            //{
            //    gyroDelta = 0;
            //}

            if (gyroDelta < 0) gyroDelta += glm::twoPI;
            else if (gyroDelta > glm::twoPI) gyroDelta -= glm::twoPI;

            //calculate delta based on circular data problem 0 to 360 to 0, clamp to +- 2 Pi
            if (gyroDelta >= -glm::PIBy2 && gyroDelta <= glm::PIBy2) gyroDelta *= -1.0;
            else
            {
                if (gyroDelta > glm::PIBy2) { gyroDelta = glm::twoPI - gyroDelta; }
                else { gyroDelta = (glm::twoPI + gyroDelta) * -1.0; }
            }
            if (gyroDelta > glm::twoPI) gyroDelta -= glm::twoPI;
            else if (gyroDelta < -glm::twoPI) gyroDelta += glm::twoPI;

            //moe the offset to line up imu with gps
            if(!isReverseWithIMU)
                imuGPS_Offset += (gyroDelta * (ahrs.fusionWeight));
            else
                imuGPS_Offset += (gyroDelta * (0.02));

            if (imuGPS_Offset > glm::twoPI) imuGPS_Offset -= glm::twoPI;
            else if (imuGPS_Offset < 0) imuGPS_Offset += glm::twoPI;

            //determine the Corrected heading based on gyro and GPS
            imuCorrected = imuHeading + imuGPS_Offset;
            if (imuCorrected > glm::twoPI) imuCorrected -= glm::twoPI;
            else if (imuCorrected < 0) imuCorrected += glm::twoPI;

            //use imu as heading when going slow
            CVehicle::instance()->fixHeading = imuCorrected;

            //#endregion
        }
        else
        {
            //how far since last fix
            distanceCurrentStepFix = glm::Distance(stepFixPts[0], pn.fix);

            if (distanceCurrentStepFix < (gpsMinimumStepDistance))
                goto byPass;

            minFixHeadingDistSquared = minHeadingStepDist * minHeadingStepDist;
            fixToFixHeadingDistance = 0;

            for (int i = 0; i < totalFixSteps; i++)
            {
                fixToFixHeadingDistance = glm::DistanceSquared(stepFixPts[i], pn.fix);
                currentStepFix = i;

                if (fixToFixHeadingDistance > minFixHeadingDistSquared)
                {
                    break;
                }
            }

            if (fixToFixHeadingDistance < minFixHeadingDistSquared * 0.5)
                goto byPass;

            newGPSHeading = atan2(pn.fix.easting - stepFixPts[currentStepFix].easting,
                                  pn.fix.northing - stepFixPts[currentStepFix].northing);
            if (newGPSHeading < 0) newGPSHeading += glm::twoPI;

            if (ahrs.isReverseOn)
            {

                ////what is angle between the last valid heading before stopping and one just now
                delta = fabs(M_PI - fabs(fabs(newGPSHeading - gpsHeading) - M_PI));

                filteredDelta = delta * 0.2 + filteredDelta * 0.8;

                //filtered delta different then delta
                if (fabs(filteredDelta - delta) > 0.5)
                {
                    CVehicle::instance()->setIsChangingDirection(true);
                }
                else
                {
                    CVehicle::instance()->setIsChangingDirection(false);
                }

                //we can't be sure if changing direction so do nothing
                if (CVehicle::instance()->isChangingDirection)
                    goto byPass;

                //ie change in direction
                if (filteredDelta > 1.57) //
                {
                    CVehicle::instance()->setIsReverse(true);
                    newGPSHeading += M_PI;
                    if (newGPSHeading < 0) newGPSHeading += glm::twoPI;
                    else if (newGPSHeading >= glm::twoPI) newGPSHeading -= glm::twoPI;
                }
                else
                    CVehicle::instance()->setIsReverse(false);

                if (CVehicle::instance()->isReverse)
                    newGPSHeading -= glm::toRadians(CVehicle::instance()->antennaPivot / 1
                                                    * mc.actualSteerAngleDegrees * ahrs.reverseComp);
                else
                    newGPSHeading -= glm::toRadians(CVehicle::instance()->antennaPivot / 1
                                                    * mc.actualSteerAngleDegrees * ahrs.forwardComp);

                if (newGPSHeading < 0) newGPSHeading += glm::twoPI;
                else if (newGPSHeading >= glm::twoPI) newGPSHeading -= glm::twoPI;
            }

            else
            {
                CVehicle::instance()->setIsReverse(false);
            }

            //set the headings
            CVehicle::instance()->fixHeading = gpsHeading = newGPSHeading;
        }

        //save current fix and set as valid
        for (int i = totalFixSteps - 1; i > 0; i--) stepFixPts[i] = stepFixPts[i - 1];
        stepFixPts[0].easting = pn.fix.easting;
        stepFixPts[0].northing = pn.fix.northing;
        stepFixPts[0].isSet = 1;

        //#endregion

        //#region Camera

        camDelta = CVehicle::instance()->fixHeading - smoothCamHeading;

        if (camDelta < 0) camDelta += glm::twoPI;
        else if (camDelta > glm::twoPI) camDelta -= glm::twoPI;

        //calculate delta based on circular data problem 0 to 360 to 0, clamp to +- 2 Pi
        if (camDelta >= -glm::PIBy2 && camDelta <= glm::PIBy2) camDelta *= -1.0;
        else
        {
            if (camDelta > glm::PIBy2) { camDelta = glm::twoPI - camDelta; }
            else { camDelta = (glm::twoPI + camDelta) * -1.0; }
        }
        if (camDelta > glm::twoPI) camDelta -= glm::twoPI;
        else if (camDelta < -glm::twoPI) camDelta += glm::twoPI;

        smoothCamHeading -= camDelta * camera.camSmoothFactor;

        if (smoothCamHeading > glm::twoPI) smoothCamHeading -= glm::twoPI;
        else if (smoothCamHeading < -glm::twoPI) smoothCamHeading += glm::twoPI;

        camera.camHeading = glm::toDegrees(smoothCamHeading);

        //#endregion


        //Calculate a million other things
    byPass:
        if (ahrs.imuHeading != 99999)
        {
            imuCorrected = (glm::toRadians(ahrs.imuHeading)) + imuGPS_Offset;
            if (imuCorrected > glm::twoPI) imuCorrected -= glm::twoPI;
            else if (imuCorrected < 0) imuCorrected += glm::twoPI;

            //use imu as heading when going slow
            CVehicle::instance()->fixHeading = imuCorrected;
        }

        camDelta = CVehicle::instance()->fixHeading - smoothCamHeading;

        if (camDelta < 0) camDelta += glm::twoPI;
        else if (camDelta > glm::twoPI) camDelta -= glm::twoPI;

        //calculate delta based on circular data problem 0 to 360 to 0, clamp to +- 2 Pi
        if (camDelta >= -glm::PIBy2 && camDelta <= glm::PIBy2) camDelta *= -1.0;
        else
        {
            if (camDelta > glm::PIBy2) { camDelta = glm::twoPI - camDelta; }
            else { camDelta = (glm::twoPI + camDelta) * -1.0; }
        }
        if (camDelta > glm::twoPI) camDelta -= glm::twoPI;
        else if (camDelta < -glm::twoPI) camDelta += glm::twoPI;

        smoothCamHeading -= camDelta * camera.camSmoothFactor;

        if (smoothCamHeading > glm::twoPI) smoothCamHeading -= glm::twoPI;
        else if (smoothCamHeading < -glm::twoPI) smoothCamHeading += glm::twoPI;

        camera.camHeading = glm::toDegrees(smoothCamHeading);

        TheRest();
    } else if (headingFromSource == "VTG")
    {
        isFirstHeadingSet = true;
        if (CVehicle::instance()->avgSpeed > 1)
        {
            //use NMEA headings for camera and tractor graphic
            CVehicle::instance()->fixHeading = glm::toRadians(pn.headingTrue);
            camera.camHeading = pn.headingTrue;
            gpsHeading = CVehicle::instance()->fixHeading;
        }

        //grab the most current fix to last fix distance
        distanceCurrentStepFix = glm::Distance(pn.fix, prevFix);

        //#region Antenna Offset

        if (CVehicle::instance()->antennaOffset != 0)
        {
            pn.fix.easting = (cos(-CVehicle::instance()->fixHeading) * CVehicle::instance()->antennaOffset) + pn.fix.easting;
            pn.fix.northing = (sin(-CVehicle::instance()->fixHeading) * CVehicle::instance()->antennaOffset) + pn.fix.northing;
        }
        //#endregion

        uncorrectedEastingGraph = pn.fix.easting;

        //an IMU with heading correction, add the correction
        if (ahrs.imuHeading != 99999)
        {
            //current gyro angle in radians
            double correctionHeading = (glm::toRadians(ahrs.imuHeading));

            //Difference between the IMU heading and the GPS heading
            double gyroDelta = (correctionHeading + imuGPS_Offset) - gpsHeading;
            if (gyroDelta < 0) gyroDelta += glm::twoPI;

            //calculate delta based on circular data problem 0 to 360 to 0, clamp to +- 2 Pi
            if (gyroDelta >= -glm::PIBy2 && gyroDelta <= glm::PIBy2) gyroDelta *= -1.0;
            else
            {
                if (gyroDelta > glm::PIBy2) { gyroDelta = glm::twoPI - gyroDelta; }
                else { gyroDelta = (glm::twoPI + gyroDelta) * -1.0; }
            }
            if (gyroDelta > glm::twoPI) gyroDelta -= glm::twoPI;
            if (gyroDelta < -glm::twoPI) gyroDelta += glm::twoPI;

            //if the gyro and last corrected fix is < 10 degrees, super low pass for gps
            if (fabs(gyroDelta) < 0.18)
            {
                //a bit of delta and add to correction to current gyro
                imuGPS_Offset += (gyroDelta * (0.1));
                if (imuGPS_Offset > glm::twoPI) imuGPS_Offset -= glm::twoPI;
                if (imuGPS_Offset < -glm::twoPI) imuGPS_Offset += glm::twoPI;
            }
            else
            {
                //a bit of delta and add to correction to current gyro
                imuGPS_Offset += (gyroDelta * (0.2));
                if (imuGPS_Offset > glm::twoPI) imuGPS_Offset -= glm::twoPI;
                if (imuGPS_Offset < -glm::twoPI) imuGPS_Offset += glm::twoPI;
            }

            //determine the Corrected heading based on gyro and GPS
            imuCorrected = correctionHeading + imuGPS_Offset;
            if (imuCorrected > glm::twoPI) imuCorrected -= glm::twoPI;
            if (imuCorrected < 0) imuCorrected += glm::twoPI;

            CVehicle::instance()->fixHeading = imuCorrected;

            camera.camHeading = CVehicle::instance()->fixHeading;
            if (camera.camHeading > glm::twoPI) camera.camHeading -= glm::twoPI;
            camera.camHeading = glm::toDegrees(camera.camHeading);
        }


        //#region Roll

        if (ahrs.imuRoll != 88888)
        {
            //change for roll to the right is positive times -1
            rollCorrectionDistance = sin(glm::toRadians((ahrs.imuRoll))) * -CVehicle::instance()->antennaHeight;
            correctionDistanceGraph = rollCorrectionDistance;

            // roll to left is positive  **** important!!
            // not any more - April 30, 2019 - roll to right is positive Now! Still Important
            pn.fix.easting = (cos(-CVehicle::instance()->fixHeading) * rollCorrectionDistance) + pn.fix.easting;
            pn.fix.northing = (sin(-CVehicle::instance()->fixHeading) * rollCorrectionDistance) + pn.fix.northing;
        }

        //#endregion Roll

        TheRest();

        //most recent fixes are now the prev ones
        prevFix.easting = pn.fix.easting; prevFix.northing = pn.fix.northing;

    } else if (headingFromSource == "Dual")
    {
        isFirstHeadingSet = true;
        //use Dual Antenna heading for camera and tractor graphic
        CVehicle::instance()->fixHeading = glm::toRadians(pn.headingTrueDual);
        gpsHeading = CVehicle::instance()->fixHeading;

        uncorrectedEastingGraph = pn.fix.easting;

        if (CVehicle::instance()->antennaOffset != 0)
        {
            pn.fix.easting = (cos(-CVehicle::instance()->fixHeading) * CVehicle::instance()->antennaOffset) + pn.fix.easting;
            pn.fix.northing = (sin(-CVehicle::instance()->fixHeading) * CVehicle::instance()->antennaOffset) + pn.fix.northing;
        }

        if (ahrs.imuRoll != 88888 && CVehicle::instance()->antennaHeight != 0)
        {

            //change for roll to the right is positive times -1
            rollCorrectionDistance = sin(glm::toRadians((ahrs.imuRoll))) * -CVehicle::instance()->antennaHeight;
            correctionDistanceGraph = rollCorrectionDistance;

            pn.fix.easting = (cos(-gpsHeading) * rollCorrectionDistance) + pn.fix.easting;
            pn.fix.northing = (sin(-gpsHeading) * rollCorrectionDistance) + pn.fix.northing;
        }

        //grab the most current fix and save the distance from the last fix
        distanceCurrentStepFix = glm::Distance(pn.fix, prevDistFix);

        //userDistance can be reset
        if ((double)(fd.distanceUser += distanceCurrentStepFix) > 999) fd.distanceUser = 0;

        distanceCurrentStepFixDisplay = distanceCurrentStepFix * 100;
        prevDistFix = pn.fix;

        if (glm::DistanceSquared(lastReverseFix, pn.fix) > 0.20)
        {
            //most recent heading
            double newHeading = atan2(pn.fix.easting - lastReverseFix.easting,
                                      pn.fix.northing - lastReverseFix.northing);

            if (newHeading < 0) newHeading += glm::twoPI;


            //what is angle between the last reverse heading and current dual heading
            double delta = fabs(M_PI - fabs(fabs(newHeading - CVehicle::instance()->fixHeading) - M_PI));

            //are we going backwards
            CVehicle::instance()->isReverse = delta > 2 ? true : false;

            //save for next meter check
            lastReverseFix = pn.fix;
        }

        double camDelta = CVehicle::instance()->fixHeading - smoothCamHeading;

        if (camDelta < 0) camDelta += glm::twoPI;
        else if (camDelta > glm::twoPI) camDelta -= glm::twoPI;

        //calculate delta based on circular data problem 0 to 360 to 0, clamp to +- 2 Pi
        if (camDelta >= -glm::PIBy2 && camDelta <= glm::PIBy2) camDelta *= -1.0;
        else
        {
            if (camDelta > glm::PIBy2) { camDelta = glm::twoPI - camDelta; }
            else { camDelta = (glm::twoPI + camDelta) * -1.0; }
        }
        if (camDelta > glm::twoPI) camDelta -= glm::twoPI;
        else if (camDelta < -glm::twoPI) camDelta += glm::twoPI;

        smoothCamHeading -= camDelta * camera.camSmoothFactor;

        if (smoothCamHeading > glm::twoPI) smoothCamHeading -= glm::twoPI;
        else if (smoothCamHeading < -glm::twoPI) smoothCamHeading += glm::twoPI;

        camera.camHeading = glm::toDegrees(smoothCamHeading);

        TheRest();
    }
    //else {
    //}

    if (CVehicle::instance()->fixHeading >= glm::twoPI)
        CVehicle::instance()->fixHeading-= glm::twoPI;

    //#endregion

    //#region Corrected Position for GPS_OUT
    //NOTE: Michael, I'm not sure about this entire region

    double rollCorrectedLat;
    double rollCorrectedLon;
    pn.ConvertLocalToWGS84(pn.fix.northing, pn.fix.easting, rollCorrectedLat, rollCorrectedLon);

    QByteArray pgnRollCorrectedLatLon(22, 0);

    pgnRollCorrectedLatLon[0] = 0x80;
    pgnRollCorrectedLatLon[1] = 0x81;
    pgnRollCorrectedLatLon[2] = 0x7F;
    pgnRollCorrectedLatLon[3] = 0x64;
    pgnRollCorrectedLatLon[4] = 16;

    std::memcpy(pgnRollCorrectedLatLon.data() + 5, &rollCorrectedLon, 8);
    std::memcpy(pgnRollCorrectedLatLon.data() + 13, &rollCorrectedLat, 8);

    // SendPgnToLoop(pgnRollCorrectedLatLon); // ❌ REMOVED - Phase 4.6: AgIOService Workers handle PGN
    // GPS position data now flows through: AgIOService → FormGPS → pn/vehicle → OpenGL

    //#endregion

    //#region AutoSteer

    //preset the values
    CVehicle::instance()->guidanceLineDistanceOff = 32000;

    if (ct.isContourBtnOn)
    {
        ct.DistanceFromContourLine(isBtnAutoSteerOn, *CVehicle::instance(), yt, ahrs, pn, CVehicle::instance()->pivotAxlePos, CVehicle::instance()->steerAxlePos);
    }
    else
    {
        //auto track routine
        if (CTrack::instance()->isAutoTrack && !isBtnAutoSteerOn)
        {
            //CTrack::instance()->autoTrack3SecTimer = 0;

            CTrack::instance()->SwitchToClosestRefTrack(CVehicle::instance()->steerAxlePos, *CVehicle::instance());
        }

        CTrack::instance()->BuildCurrentLine(CVehicle::instance()->pivotAxlePos,secondsSinceStart,isBtnAutoSteerOn,yt,*CVehicle::instance(),bnd,ahrs,gyd,pn);
    }

    // autosteer at full speed of updates

    //if the whole path driving driving process is green
    if (recPath.isDrivingRecordedPath) recPath.UpdatePosition(*CVehicle::instance(), yt, isBtnAutoSteerOn);

    // If Drive button off - normal autosteer
    if (!CVehicle::instance()->isInFreeDriveMode)
    {
        //fill up0 the appropriate arrays with new values
        p_254.pgn[p_254.speedHi] = (char)((int)(fabs(CVehicle::instance()->avgSpeed) * 10.0) >> 8);
        p_254.pgn[p_254.speedLo] = (char)((int)(fabs(CVehicle::instance()->avgSpeed) * 10.0));
        //mc.machineControlData[mc.cnSpeed] = mc.autoSteerData[mc.sdSpeed];

        //save distance for display
        lightbarDistance = CVehicle::instance()->guidanceLineDistanceOff;

        if (!isBtnAutoSteerOn) //32020 means auto steer is off
        {
            //NOTE: Is this supposed to be commented out?
            //CVehicle::instance()->guidanceLineDistanceOff = 32020;
            p_254.pgn[p_254.status] = 0;
        }

        else p_254.pgn[p_254.status] = 1;

        if (recPath.isDrivingRecordedPath || recPath.isFollowingDubinsToPath) p_254.pgn[p_254.status] = 1;

        //mc.autoSteerData[7] = unchecked((byte)(CVehicle::instance()->guidanceLineDistanceOff >> 8));
        //mc.autoSteerData[8] = unchecked((byte)(CVehicle::instance()->guidanceLineDistanceOff));

        //convert to cm from mm and divide by 2 - lightbar
        int distanceX2;
        //if (CVehicle::instance()->guidanceLineDistanceOff == 32020 || CVehicle::instance()->guidanceLineDistanceOff == 32000)
        if (!isBtnAutoSteerOn || CVehicle::instance()->guidanceLineDistanceOff == 32000)
            distanceX2 = 255;

        else
        {
            distanceX2 = (int)(CVehicle::instance()->guidanceLineDistanceOff * 0.05);

            if (distanceX2 < -127) distanceX2 = -127;
            else if (distanceX2 > 127) distanceX2 = 127;
            distanceX2 += 127;
        }

        p_254.pgn[p_254.lineDistance] = (char)distanceX2;

        if (!timerSim.isActive())
        {
            if (isBtnAutoSteerOn && CVehicle::instance()->avgSpeed > CVehicle::instance()->maxSteerSpeed)
            {
                onStopAutoSteer();
                if (isMetric)
                    TimedMessageBox(3000, tr("AutoSteer Disabled"), tr("Above Maximum Safe Steering Speed: ") + locale.toString(CVehicle::instance()->maxSteerSpeed, 'g', 1) + tr(" Kmh"));
                else
                    TimedMessageBox(3000, tr("AutoSteer Disabled"), tr("Above Maximum Safe Steering Speed: ") + locale.toString(CVehicle::instance()->maxSteerSpeed * 0.621371, 'g', 1) + tr(" MPH"));
            }

            if (isBtnAutoSteerOn && CVehicle::instance()->avgSpeed < CVehicle::instance()->minSteerSpeed)
            {
                minSteerSpeedTimer++;
                if (minSteerSpeedTimer > 80)
                {
                    onStopAutoSteer();
                    if (isMetric)
                        TimedMessageBox(3000, tr("AutoSteer Disabled"), tr("Below Minimum Safe Steering Speed: ") + locale.toString(CVehicle::instance()->minSteerSpeed, 'g', 1) + tr(" Kmh"));
                    else
                        TimedMessageBox(3000, tr("AutoSteer Disabled"), tr("Below Minimum Safe Steering Speed: ") + locale.toString(CVehicle::instance()->minSteerSpeed * 0.621371, 'g', 1) + tr(" MPH"));
                }
            }
            else
            {
                minSteerSpeedTimer = 0;
            }
        }

        double tanSteerAngle = tan(glm::toRadians(((double)(CVehicle::instance()->guidanceLineSteerAngle)) * 0.01));
        double tanActSteerAngle = tan(glm::toRadians(mc.actualSteerAngleDegrees));

        setAngVel = 0.277777 * CVehicle::instance()->avgSpeed * tanSteerAngle / CVehicle::instance()->wheelbase;
        actAngVel = glm::toDegrees(0.277777 * CVehicle::instance()->avgSpeed * tanActSteerAngle / CVehicle::instance()->wheelbase);


        isMaxAngularVelocity = false;
        //greater then settings rads/sec limit steer angle
        if (fabs(setAngVel) > CVehicle::instance()->maxAngularVelocity)
        {
            setAngVel = CVehicle::instance()->maxAngularVelocity;
            tanSteerAngle = 3.6 * setAngVel * CVehicle::instance()->wheelbase / CVehicle::instance()->avgSpeed;
            if (CVehicle::instance()->guidanceLineSteerAngle < 0)
                CVehicle::instance()->guidanceLineSteerAngle = (short)(glm::toDegrees(atan(tanSteerAngle)) * -100);
            else
                CVehicle::instance()->guidanceLineSteerAngle = (short)(glm::toDegrees(atan(tanSteerAngle)) * 100);
            isMaxAngularVelocity = true;
        }

        setAngVel = glm::toDegrees(setAngVel);

        p_254.pgn[p_254.steerAngleHi] = (char)(CVehicle::instance()->guidanceLineSteerAngle >> 8);
        p_254.pgn[p_254.steerAngleLo] = (char)(CVehicle::instance()->guidanceLineSteerAngle);

        if (CVehicle::instance()->isChangingDirection && ahrs.imuHeading == 99999)
            p_254.pgn[p_254.status] = 0;

        //for now if backing up, turn off autosteer
        if (!isSteerInReverse)
        {
            if (CVehicle::instance()->isReverse) p_254.pgn[p_254.status] = 0;
        }
    }

    else //Drive button is on
    {
        //fill up the auto steer array with free drive values
        p_254.pgn[p_254.speedHi] = (char)((int)(80) >> 8);
        p_254.pgn[p_254.speedLo] = (char)((int)(80));

        //turn on status to operate
        p_254.pgn[p_254.status] = 1;

        //send the steer angle
        CVehicle::instance()->guidanceLineSteerAngle = (qint16)(CVehicle::instance()->driveFreeSteerAngle * 100);

        p_254.pgn[p_254.steerAngleHi] = (char)(CVehicle::instance()->guidanceLineSteerAngle >> 8);
        p_254.pgn[p_254.steerAngleLo] = (char)(CVehicle::instance()->guidanceLineSteerAngle);


    }

    //out serial to autosteer module  //indivdual classes load the distance and heading deltas
    // SendPgnToLoop(p_254.pgn); // ❌ REMOVED - Phase 4.6: AgIOService Workers handle PGN
    if (m_agioService) {
        m_agioService->sendPgn(p_254.pgn);
    }

    // Smart WAS Calibration data collection
    if (IsCollectingData && abs(CVehicle::instance()->guidanceLineDistanceOff) < 500) // Within 50cm of guidance line
    {
        // Convert guidanceLineSteerAngle from centidegrees to degrees and collect data
        AddSteerAngleSample(CVehicle::instance()->guidanceLineSteerAngle * 0.01, abs(CVehicle::instance()->avgSpeed));
    }

    //for average cross track error
    if (CVehicle::instance()->guidanceLineDistanceOff < 29000)
    {
        crossTrackError = (int)((double)crossTrackError * 0.90 + fabs((double)CVehicle::instance()->guidanceLineDistanceOff) * 0.1);
    }
    else
    {
        crossTrackError = 0;
    }

    //#endregion

    //#region AutoSteer

    //preset the values
    /*
     * NOTE: Can this all be removed? It's not present in CS
    CVehicle::instance()->guidanceLineDistanceOff = 32000;

    if (ct.isContourBtnOn)
    {
        ct.DistanceFromContourLine(isBtnAutoSteerOn, *CVehicle::instance(), yt, ahrs, pn, CVehicle::instance()->pivotAxlePos, CVehicle::instance()->steerAxlePos);
    }
    else
    {
        if (curve.isCurveSet && curve.isBtnCurveOn)
        {
            //do the calcs for AB Curve
            curve.GetCurrentCurveLine(CVehicle::instance()->pivotAxlePos, CVehicle::instance()->steerAxlePos, secondsSinceStart, isBtnAutoSteerOn, mc.steerSwitchHigh, *CVehicle::instance(), bnd, yt, ahrs, gyd, pn);
        }

        if (ABLine.isABLineSet && ABLine.isBtnABLineOn)
        {
            ABLine.GetCurrentABLine(CVehicle::instance()->pivotAxlePos, CVehicle::instance()->steerAxlePos, secondsSinceStart, isBtnAutoSteerOn, mc.steerSwitchHigh, *CVehicle::instance(), yt, ahrs, gyd, pn);
        }
    }

    // autosteer at full speed of updates

    //if the whole path driving driving process is green
    if (recPath.isDrivingRecordedPath) recPath.UpdatePosition(*CVehicle::instance(), yt, isBtnAutoSteerOn);

    // If Drive button off - normal autosteer
    if (!CVehicle::instance()->isInFreeDriveMode)
    {
        //fill up0 the appropriate arrays with new values
        p_254.pgn[p_254.speedHi] = (char)((int)(fabs(CVehicle::instance()->avgSpeed) * 10.0) >> 8);
        p_254.pgn[p_254.speedLo] = (char)((int)(fabs(CVehicle::instance()->avgSpeed) * 10.0));
        //mc.machineControlData[mc.cnSpeed] = mc.autoSteerData[mc.sdSpeed];

        //save distance for display
        lightbarDistance = CVehicle::instance()->guidanceLineDistanceOff;

        if (!isBtnAutoSteerOn) //32020 means auto steer is off
        {
            //CVehicle::instance()->guidanceLineDistanceOff = 32020;
            p_254.pgn[p_254.status] = 0;
        }

        else p_254.pgn[p_254.status] = 1;

        if (recPath.isDrivingRecordedPath || recPath.isFollowingDubinsToPath) p_254.pgn[p_254.status] = 1;

        //mc.autoSteerData[7] = unchecked((byte)(CVehicle::instance()->guidanceLineDistanceOff >> 8));
        //mc.autoSteerData[8] = unchecked((byte)(CVehicle::instance()->guidanceLineDistanceOff));

        //convert to cm from mm and divide by 2 - lightbar
        int distanceX2;
        //if (CVehicle::instance()->guidanceLineDistanceOff == 32020 || CVehicle::instance()->guidanceLineDistanceOff == 32000)
        if (!isBtnAutoSteerOn || CVehicle::instance()->guidanceLineDistanceOff == 32000)
            distanceX2 = 255;

        else
        {
            distanceX2 = (int)(CVehicle::instance()->guidanceLineDistanceOff * 0.05);

            if (distanceX2 < -127) distanceX2 = -127;
            else if (distanceX2 > 127) distanceX2 = 127;
            distanceX2 += 127;
        }

        p_254.pgn[p_254.lineDistance] = (char)distanceX2;

        if (!timerSim.isActive())
        {
            if (isBtnAutoSteerOn && CVehicle::instance()->avgSpeed > CVehicle::instance()->maxSteerSpeed)
            {
                onStopAutoSteer();

                if (isMetric)
                    TimedMessageBox(3000, tr("AutoSteer Disabled"), tr("Above Maximum Safe Steering Speed: ") + locale.toString(CVehicle::instance()->maxSteerSpeed, 'g', 1) + tr(" Kmh"));
                else
                    TimedMessageBox(3000, tr("AutoSteer Disabled"), tr("Above Maximum Safe Steering Speed: ") + locale.toString(CVehicle::instance()->maxSteerSpeed * 0.621371, 'g', 1) + tr(" MPH"));
            }

            if (isBtnAutoSteerOn && CVehicle::instance()->avgSpeed < CVehicle::instance()->minSteerSpeed)
            {
                minSteerSpeedTimer++;
                if (minSteerSpeedTimer > 80)
                {
                    onStopAutoSteer();
                    if (isMetric)
                        TimedMessageBox(3000, tr("AutoSteer Disabled"), tr("Below Minimum Safe Steering Speed: ") + locale.toString(CVehicle::instance()->maxSteerSpeed, 'g', 1) + tr(" Kmh"));
                    else
                        TimedMessageBox(3000, tr("AutoSteer Disabled"), tr("Below Minimum Safe Steering Speed: ") + locale.toString(CVehicle::instance()->maxSteerSpeed * 0.621371, 'g', 1) + tr(" MPH"));
                }
            }
            else
            {
                minSteerSpeedTimer = 0;
            }
        }

        double tanSteerAngle = tan(glm::toRadians(((double)(CVehicle::instance()->guidanceLineSteerAngle)) * 0.01));
        double tanActSteerAngle = tan(glm::toRadians(mc.actualSteerAngleDegrees));

        setAngVel = 0.277777 * CVehicle::instance()->avgSpeed * tanSteerAngle / CVehicle::instance()->wheelbase;
        actAngVel = glm::toDegrees(0.277777 * CVehicle::instance()->avgSpeed * tanActSteerAngle / CVehicle::instance()->wheelbase);


        isMaxAngularVelocity = false;
        //greater then settings rads/sec limit steer angle
        if (fabs(setAngVel) > CVehicle::instance()->maxAngularVelocity)
        {
            setAngVel = CVehicle::instance()->maxAngularVelocity;
            tanSteerAngle = 3.6 * setAngVel * CVehicle::instance()->wheelbase / CVehicle::instance()->avgSpeed;
            if (CVehicle::instance()->guidanceLineSteerAngle < 0)
                CVehicle::instance()->guidanceLineSteerAngle = (short)(glm::toDegrees(atan(tanSteerAngle)) * -100);
            else
                CVehicle::instance()->guidanceLineSteerAngle = (short)(glm::toDegrees(atan(tanSteerAngle)) * 100);
            isMaxAngularVelocity = true;
        }

        setAngVel = glm::toDegrees(setAngVel);

        p_254.pgn[p_254.steerAngleHi] = (char)(CVehicle::instance()->guidanceLineSteerAngle >> 8);
        p_254.pgn[p_254.steerAngleLo] = (char)(CVehicle::instance()->guidanceLineSteerAngle);

        if (isChangingDirection && ahrs.imuHeading == 99999)
            p_254.pgn[p_254.status] = 0;

        //for now if backing up, turn off autosteer
        if (!isSteerInReverse)
        {
            if (CVehicle::instance()->isReverse) p_254.pgn[p_254.status] = 0;
        }
    }

    else //Drive button is on
    {
        //fill up the auto steer array with free drive values
        p_254.pgn[p_254.speedHi] = (char)((int)(80) >> 8);
        p_254.pgn[p_254.speedLo] = (char)((int)(80));

        //turn on status to operate
        p_254.pgn[p_254.status] = 1;

        //send the steer angle
        CVehicle::instance()->guidanceLineSteerAngle = (qint16)(CVehicle::instance()->driveFreeSteerAngle * 100);

        p_254.pgn[p_254.steerAngleHi] = (char)(CVehicle::instance()->guidanceLineSteerAngle >> 8);
        p_254.pgn[p_254.steerAngleLo] = (char)(CVehicle::instance()->guidanceLineSteerAngle);


    }

    //out serial to autosteer module  //indivdual classes load the distance and heading deltas
    // SendPgnToLoop(p_254.pgn); // ❌ REMOVED - Phase 4.6: AgIOService Workers handle PGN
    if (m_agioService) {
        m_agioService->sendPgn(p_254.pgn);
    }

    //for average cross track error
    if (CVehicle::instance()->guidanceLineDistanceOff < 29000)
    {
        crossTrackError = (int)((double)crossTrackError * 0.90 + fabs((double)CVehicle::instance()->guidanceLineDistanceOff) * 0.1);
    }
    else
    {
        crossTrackError = 0;
    }

    //#endregion
*/
    //#region Youturn

    //if an outer boundary is set, then apply critical stop logic
    if (bnd.bndList.count() > 0)
    {
        //check if inside all fence
        if (!yt.isYouTurnBtnOn)
        {
            mc.isOutOfBounds = !bnd.IsPointInsideFenceArea(CVehicle::instance()->pivotAxlePos);
            isOutOfBounds = mc.isOutOfBounds;
        }
        else //Youturn is on
        {
            bool isInTurnBounds = bnd.IsPointInsideTurnArea(CVehicle::instance()->pivotAxlePos) != -1;
            //Are we inside outer and outside inner all turn boundaries, no turn creation problems
            //if we are too much off track > 1.3m, kill the diagnostic creation, start again
            //if (!yt.isYouTurnTriggered)
            if (isInTurnBounds)
            {
                mc.isOutOfBounds = false;
                isOutOfBounds = false;
                //now check to make sure we are not in an inner turn boundary - drive thru is ok
                if (yt.youTurnPhase != 10)
                {
                    if (crossTrackError > 1000)
                    {
                        yt.ResetCreatedYouTurn();
                    }
                    else
                    {
                        if (CTrack::instance()->getMode() == TrackMode::AB)
                        {
                            yt.BuildABLineDubinsYouTurn(yt.isYouTurnRight,*CVehicle::instance(),bnd,
                                                        *CTrack::instance(),secondsSinceStart);
                        }
                        else
                        {
                            yt.BuildCurveDubinsYouTurn(yt.isYouTurnRight, CVehicle::instance()->pivotAxlePos,
                                                       *CVehicle::instance(),bnd,*CTrack::instance(),secondsSinceStart);
                        }
                    }

                    if (yt.uTurnStyle == 0 && yt.youTurnPhase == 10)
                    {
                        yt.SmoothYouTurn(6);
                    }
                    if (yt.isTurnCreationTooClose && !yt.turnTooCloseTrigger)
                    {
                        yt.turnTooCloseTrigger = true;
                        //if (sounds.isTurnSoundOn) sounds.sndUTurnTooClose.Play(); Implemented in QML
                    }
                }
                else if (yt.ytList.count() > 5)//wait to trigger the actual turn since its made and waiting
                {
                    //distance from current pivot to first point of youturn pattern
                    distancePivotToTurnLine = glm::Distance(yt.ytList[5], CVehicle::instance()->pivotAxlePos);

                    //if ((distancePivotToTurnLine <= 20.0) && (distancePivotToTurnLine >= 18.0) && !yt.isYouTurnTriggered)

                    /* moved to QML
                    if (!sounds.isBoundAlarming)
                    {
                        if (sounds.isTurnSoundOn) sounds.sndBoundaryAlarm.Play();
                        sounds.isBoundAlarming = true;
                    }*/
                    //yt.YouTurnTriggerCTrack::instance(), *CVehicle::instance());
                    //if we are close enough to pattern, trigger.
                    if ((distancePivotToTurnLine <= 1.0) && (distancePivotToTurnLine >= 0) && !yt.isYouTurnTriggered)
                    {
                        yt.YouTurnTrigger(*CTrack::instance(), *CVehicle::instance());
                        //moved to QML
                        //sounds.isBoundAlarming = false;
                    }

                    //if (isBtnAutoSteerOn && CVehicle::instance()->guidanceLineDistanceOff > 300 && !yt.isYouTurnTriggered)
                    //{
                    //    yt.ResetCreatedYouTurn();
                    //}
                }
            }
            else
            {
                if (!yt.isYouTurnTriggered)
                {
                    yt.ResetCreatedYouTurn();
                    mc.isOutOfBounds = !bnd.IsPointInsideFenceArea(CVehicle::instance()->pivotAxlePos);
                    isOutOfBounds = mc.isOutOfBounds;
                }

            }

            //}
            //// here is stop logic for out of bounds - in an inner or out the outer turn border.
            //else
            //{
            //    //mc.isOutOfBounds = true;
            //    if (isBtnAutoSteerOn)
            //    {
            //        if (yt.isYouTurnBtnOn)
            //        {
            //            yt.ResetCreatedYouTurn();
            //            //sim.stepDistance = 0 / 17.86;
            //        }
            //    }
            //    else
            //    {
            //        yt.isTurnCreationTooClose = false;
            //    }

            //}
        }
    }
    else
    {
        mc.isOutOfBounds = false;
        isOutOfBounds = false;
    }

    //#endregion

    //update main window
    //oglMain.MakeCurrent();
    //oglMain.Refresh();

    AOGRendererInSG *renderer = mainWindow->findChild<AOGRendererInSG *>("openglcontrol");
    // CRITICAL: Force OpenGL update in GUI thread to prevent threading violation
    if (renderer) {
        QMetaObject::invokeMethod(renderer, "update", Qt::QueuedConnection);
    }

    //NOTE: Not sure here.
    //stop the timer and calc how long it took to do calcs and draw
    frameTimeRough = swFrame.elapsed();

    if (frameTimeRough > 80) frameTimeRough = 80;
    frameTime = frameTime * 0.90 + frameTimeRough * 0.1;
    
    // Update AOGInterface frameTime for QML display
    QObject *aog = qmlItem(mainWindow, "aog");
    if (aog) {
        aog->setProperty("frameTime", frameTime);
    
        //end of UppdateFixPosition
        //update AOGInterface.qml:
        //pn: latitude, longitude, easting, northing, heading
        //vehicle: avgSpeed
        //ahrs:  imuRoll
        //qDebug() << "frame time after processing a new position " << swFrame.elapsed();

        aog->setProperty("latitude",pn.latitude);
        aog->setProperty("longitude",pn.longitude);
        aog->setProperty("easting",CVehicle::instance()->pivotAxlePos.easting);
        aog->setProperty("northing",CVehicle::instance()->pivotAxlePos.northing);
        aog->setProperty("heading", CVehicle::instance()->pivotAxlePos.heading);
        aog->setProperty("fusedHeading", CVehicle::instance()->fixHeading);
        aog->setProperty("toolEasting", CVehicle::instance()->toolPos.easting);
        aog->setProperty("toolNorthing", CVehicle::instance()->toolPos.northing);
        aog->setProperty("toolHeading", CVehicle::instance()->toolPos.heading);
        aog->setProperty("rawHz", nowHz);
        aog->setProperty("hz", gpsHz);
        //aog->setProperty("isReverse" , CVehicle::instance()->isReverse);
        aog->setProperty("isReverseWithIMU", isReverseWithIMU);
        aog->setProperty("blockage_avg", tool.blockage_avg);
        aog->setProperty("blockage_min1", tool.blockage_min1);
        aog->setProperty("blockage_min2", tool.blockage_min2);
        aog->setProperty("blockage_max", tool.blockage_max);
        aog->setProperty("blockage_min1_i", tool.blockage_min1_i);
        aog->setProperty("blockage_min2_i", tool.blockage_min2_i);
        aog->setProperty("blockage_max_i", tool.blockage_max_i);
        aog->setProperty("blockage_blocked", tool.blockage_blocked);

    double tool_lat, tool_lon;
    pn.ConvertLocalToWGS84(CVehicle::instance()->pivotAxlePos.northing, CVehicle::instance()->pivotAxlePos.easting, tool_lat, tool_lon);
        aog->setProperty("toolLatitude", tool_lat);
        aog->setProperty("toolLongitude", tool_lon);

        aog->setProperty("imuRollDegrees",ahrs.imuRoll);
    avgPivDistance = avgPivDistance * 0.5 + CVehicle::instance()->guidanceLineDistanceOff * 0.5;
        aog->setProperty("avgPivDistance", avgPivDistance); //mm!
        aog->setProperty("offlineDistance", CVehicle::instance()->guidanceLineDistanceOff);
        aog->setProperty("speedKph", CVehicle::instance()->avgSpeed);
    /*            lblIMUHeading.Text = mf.GyroInDegrees;
            lblFix2FixHeading.Text = mf.GPSHeading;
            lblFuzeHeading.Text = (mf.fixHeading * 57.2957795).ToString("N1");
*/

        aog->setProperty("altitude", pn.altitude);
        aog->setProperty("hdop", pn.hdop);
        aog->setProperty("age", pn.age);
        aog->setProperty("fixQuality", (int)pn.fixQuality);
        aog->setProperty("satellitesTracked", pn.satellitesTracked);
        aog->setProperty("imuHeading", ahrs.imuHeading);
        aog->setProperty("angVel", ahrs.angVel);
        aog->setProperty("isYouTurnRight", yt.isYouTurnRight);
        aog->setProperty("distancePivotToTurnLine", distancePivotToTurnLine);
        aog->setProperty("imuCorrected", imuCorrected);
        aog->setProperty("vehicle_xy",CVehicle::instance()->pivot_axle_xy);
        aog->setProperty("vehicle_bounding_box",CVehicle::instance()->bounding_box);

        aog->setProperty("steerAngleActual", mc.actualSteerAngleDegrees);
    //aog->setProperty("steerAngleSet", CVehicle::instance()->guidanceLineSteerAngle);
    // aog->setProperty("droppedSentences", udpWatchCounts); // ❌ REMOVED - Phase 4.6: No more UDP FormGPS
    // Dropped sentences now tracked by AgIOService Workers if needed
        aog->setProperty("lblPWMDisplay", mc.pwmDisplay);
        aog->setProperty("steerAngleSet", CVehicle::instance()->driveFreeSteerAngle);

        aog->setProperty("lblCalcSteerAngleInner", lblCalcSteerAngleInner);
        aog->setProperty("lblCalcSteerAngleOuter", lblCalcSteerAngleOuter);
        aog->setProperty("lblDiameter", lblDiameter);
        aog->setProperty("startSA", isSA);

        aog->setProperty("lblmodeActualXTE", CVehicle::instance()->modeActualXTE);
        aog->setProperty("lblmodeActualHeadingError", CVehicle::instance()->modeActualHeadingError);

    //TODO: access this in QML directly from CTrack::instance()->howManyPathsAway property
        aog->setProperty("current_trackNum", CTrack::instance()->getHowManyPathsAway());
        aog->setProperty("isYouTurnTriggered", yt.isYouTurnTriggered);

    // was wizard
        aog->setProperty("sampleCount", SampleCount);
        aog->setProperty("confidenceLevel", ConfidenceLevel);
        aog->setProperty("hasValidRecommendation", HasValidRecommendation);

    if (!timerSim.isActive())
        //if running simulator pretend steer module
        //is okay
        if (steerModuleConnectedCounter++ > 30)
            steerModuleConnectedCounter = 31;

        aog->setProperty("steerModuleConnectedCounter", steerModuleConnectedCounter);
        aog->setProperty("steerSwitchHigh", mc.steerSwitchHigh);
    }

    newframe = true;

    if (isJobStarted) {
        GLint oldviewport[4];
        QOpenGLContext *glContext = QOpenGLContext::currentContext();

        //if there's no context we need to create one because
        //the qml renderer is in a different thread.
        if (!glContext) {
            glContext = new QOpenGLContext;
            glContext->create();
        }

        if (!backSurface.isValid()) {
            QSurfaceFormat format = glContext->format();
            backSurface.setFormat(format);
            backSurface.create();
            auto r = backSurface.isValid();
            qDebug() << "back surface creation: " << r;
        }

        auto result = glContext->makeCurrent(&backSurface);

        initializeBackShader();

        GLint origview[4];
        glContext->functions()->glGetIntegerv(GL_VIEWPORT, origview);

        oglBack_Paint();
        processSectionLookahead();

        oglZoom_Paint();
        processOverlapCount();

        glContext->functions()->glViewport(origview[0], origview[1], origview[2], origview[3]);
    }

    lock.unlock();
    //qDebug() << "frame time after processing a new position part 2 " << swFrame.elapsed();

}

void FormGPS::TheRest()
{
    //positions and headings
    CalculatePositionHeading();

    //calculate lookahead at full speed, no sentence misses
    CalculateSectionLookAhead(CVehicle::instance()->toolPos.northing, CVehicle::instance()->toolPos.easting, CVehicle::instance()->cosSectionHeading, CVehicle::instance()->sinSectionHeading);

    //To prevent drawing high numbers of triangles, determine and test before drawing vertex
    sectionTriggerDistance = glm::Distance(pn.fix, prevSectionPos);
    contourTriggerDistance = glm::Distance(pn.fix, prevContourPos);
    gridTriggerDistance = glm::DistanceSquared(pn.fix, prevGridPos);

    //NOTE: Michael, maybe verify this is all good
    if ( isLogElevation && gridTriggerDistance > 2.9 && patchCounter !=0 && isJobStarted)
    {
        //grab fix and elevation
        sbGrid.append(
            QString::number(pn.latitude, 'f', 7).toUtf8() + ","
            + QString::number(pn.longitude, 'f', 7).toUtf8() + ","
            + QString::number(pn.altitude - CVehicle::instance()->antennaHeight, 'f', 3).toUtf8() + ","
            + QString::number(pn.fixQuality).toUtf8() + ","
            + QString::number(pn.fix.easting, 'f', 2).toUtf8() + ","
            + QString::number(pn.fix.northing, 'f', 2).toUtf8() + ","
            + QString::number(CVehicle::instance()->pivotAxlePos.heading, 'f', 3).toUtf8() + ","
            + QString::number(ahrs.imuRoll, 'f', 3).toUtf8()
            + "\r\n");

        prevGridPos.easting = CVehicle::instance()->pivotAxlePos.easting;
        prevGridPos.northing = CVehicle::instance()->pivotAxlePos.northing;
    }

    //contour points
    if (isJobStarted &&(contourTriggerDistance > tool.contourWidth
                         || contourTriggerDistance > sectionTriggerStepDistance))
    {
        AddContourPoints();
    }

    //section on off and points
    if (sectionTriggerDistance > sectionTriggerStepDistance && isJobStarted)
    {
        AddSectionOrPathPoints();
    }

    //test if travelled far enough for new boundary point
    if (bnd.isOkToAddPoints)
    {
        double boundaryDistance = glm::Distance(pn.fix, prevBoundaryPos);
        if (boundaryDistance > 1) AddBoundaryPoint();
    }

    //calc distance travelled since last GPS fix
    //distance = glm::distance(pn.fix, prevFix);
    //if (CVehicle::instance()->avgSpeed > 1)

    if ((CVehicle::instance()->avgSpeed - previousSpeed  ) < -CVehicle::instance()->panicStopSpeed && CVehicle::instance()->panicStopSpeed != 0)
    {
        if (isBtnAutoSteerOn) onStopAutoSteer();
    }

    previousSpeed = CVehicle::instance()->avgSpeed;
}



void FormGPS::CalculatePositionHeading()
{
    // #region pivot hitch trail
    //Probably move this into CVehicle

    //translate from pivot position to steer axle and pivot axle position
    //translate world to the pivot axle
    CVehicle::instance()->pivotAxlePos.easting = pn.fix.easting - (sin(CVehicle::instance()->fixHeading) * CVehicle::instance()->antennaPivot);
    CVehicle::instance()->pivotAxlePos.northing = pn.fix.northing - (cos(CVehicle::instance()->fixHeading) * CVehicle::instance()->antennaPivot);
    CVehicle::instance()->pivotAxlePos.heading = CVehicle::instance()->fixHeading;

    CVehicle::instance()->steerAxlePos.easting = CVehicle::instance()->pivotAxlePos.easting + (sin(CVehicle::instance()->fixHeading) * CVehicle::instance()->wheelbase);
    CVehicle::instance()->steerAxlePos.northing = CVehicle::instance()->pivotAxlePos.northing + (cos(CVehicle::instance()->fixHeading) * CVehicle::instance()->wheelbase);
    CVehicle::instance()->steerAxlePos.heading = CVehicle::instance()->fixHeading;
    
    // PHASE 4.3: Measure execution latency for vehicle position update
    // This measures the TIME BETWEEN calls (which gives us frequency)
    static QElapsedTimer intervalTimer;
    static bool intervalTimerStarted = false;
    if (!intervalTimerStarted) {
        intervalTimer.start();
        intervalTimerStarted = true;
    }
    
    // Measure interval between calls (for frequency calculation)
    qint64 intervalBetweenCalls = intervalTimer.nsecsElapsed();
    intervalTimer.restart();
    
    // For actual execution latency, we need a different approach
    // The execution of this function should be < 1ms
    // The interval between calls is ~100ms at 10Hz which is normal for GPS
    
    static int latencyLogCounter = 0;
    if (++latencyLogCounter % 50 == 0) { // Log every 50 updates (~5s at 10Hz, same as NMEA parsed)
        double actualHz = 1000000000.0 / intervalBetweenCalls; // Convert ns to Hz
        qDebug() << "📊 UpdateFixPosition - Interval:" << intervalBetweenCalls/1000000 << "ms"
                 << "Actual Hz:" << actualHz << "GPS Hz:" << gpsHz;
    }

    //guidance look ahead distance based on time or tool width at least

    if (!CTrack::instance()->ABLine.isLateralTriggered && !CTrack::instance()->curve.isLateralTriggered)
    {
        double guidanceLookDist = (max(tool.width * 0.5, CVehicle::instance()->avgSpeed * 0.277777 * guidanceLookAheadTime));
        CVehicle::instance()->guidanceLookPos.easting = CVehicle::instance()->pivotAxlePos.easting + (sin(CVehicle::instance()->fixHeading) * guidanceLookDist);
        CVehicle::instance()->guidanceLookPos.northing = CVehicle::instance()->pivotAxlePos.northing + (cos(CVehicle::instance()->fixHeading) * guidanceLookDist);
    }

    //determine where the rigid vehicle hitch ends
    CVehicle::instance()->hitchPos.easting = pn.fix.easting + (sin(CVehicle::instance()->fixHeading) * (tool.hitchLength - CVehicle::instance()->antennaPivot));
    CVehicle::instance()->hitchPos.northing = pn.fix.northing + (cos(CVehicle::instance()->fixHeading) * (tool.hitchLength - CVehicle::instance()->antennaPivot));

    //tool attached via a trailing hitch
    if (tool.isToolTrailing)
    {
        double over;
        if (tool.isToolTBT)
        {
            //Torriem rules!!!!! Oh yes, this is all his. Thank-you
            if (distanceCurrentStepFix != 0)
            {
                CVehicle::instance()->tankPos.heading = atan2(CVehicle::instance()->hitchPos.easting - CVehicle::instance()->tankPos.easting, CVehicle::instance()->hitchPos.northing - CVehicle::instance()->tankPos.northing);
                if (CVehicle::instance()->tankPos.heading < 0) CVehicle::instance()->tankPos.heading += glm::twoPI;
            }

            ////the tool is seriously jacknifed or just starting out so just spring it back.
            over = fabs(M_PI - fabs(fabs(CVehicle::instance()->tankPos.heading - CVehicle::instance()->fixHeading) - M_PI));

            if ((over < 2.0) && (startCounter > 50))
            {
                CVehicle::instance()->tankPos.easting = CVehicle::instance()->hitchPos.easting + (sin(CVehicle::instance()->tankPos.heading) * (tool.tankTrailingHitchLength));
                CVehicle::instance()->tankPos.northing = CVehicle::instance()->hitchPos.northing + (cos(CVehicle::instance()->tankPos.heading) * (tool.tankTrailingHitchLength));
            }

            //criteria for a forced reset to put tool directly behind vehicle
            if (over > 2.0 || startCounter < 51 )
            {
                CVehicle::instance()->tankPos.heading = CVehicle::instance()->fixHeading;
                CVehicle::instance()->tankPos.easting = CVehicle::instance()->hitchPos.easting + (sin(CVehicle::instance()->tankPos.heading) * (tool.tankTrailingHitchLength));
                CVehicle::instance()->tankPos.northing = CVehicle::instance()->hitchPos.northing + (cos(CVehicle::instance()->tankPos.heading) * (tool.tankTrailingHitchLength));
            }

        }

        else
        {
            CVehicle::instance()->tankPos.heading = CVehicle::instance()->fixHeading;
            CVehicle::instance()->tankPos.easting = CVehicle::instance()->hitchPos.easting;
            CVehicle::instance()->tankPos.northing = CVehicle::instance()->hitchPos.northing;
        }

        //Torriem rules!!!!! Oh yes, this is all his. Thank-you
        if (distanceCurrentStepFix != 0)
        {
            CVehicle::instance()->toolPivotPos.heading = atan2(CVehicle::instance()->tankPos.easting - CVehicle::instance()->toolPivotPos.easting, CVehicle::instance()->tankPos.northing - CVehicle::instance()->toolPivotPos.northing);
            if (CVehicle::instance()->toolPivotPos.heading < 0) CVehicle::instance()->toolPivotPos.heading += glm::twoPI;
        }

        ////the tool is seriously jacknifed or just starting out so just spring it back.
        over = fabs(M_PI - fabs(fabs(CVehicle::instance()->toolPivotPos.heading - CVehicle::instance()->tankPos.heading) - M_PI));

        if ((over < 1.9) && (startCounter > 50))
        {
            CVehicle::instance()->toolPivotPos.easting = CVehicle::instance()->tankPos.easting + (sin(CVehicle::instance()->toolPivotPos.heading) * (tool.trailingHitchLength));
            CVehicle::instance()->toolPivotPos.northing = CVehicle::instance()->tankPos.northing + (cos(CVehicle::instance()->toolPivotPos.heading) * (tool.trailingHitchLength));
        }

        //criteria for a forced reset to put tool directly behind vehicle
        if (over > 1.9 || startCounter < 51 )
        {
            CVehicle::instance()->toolPivotPos.heading = CVehicle::instance()->tankPos.heading;
            CVehicle::instance()->toolPivotPos.easting = CVehicle::instance()->tankPos.easting + (sin(CVehicle::instance()->toolPivotPos.heading) * (tool.trailingHitchLength));
            CVehicle::instance()->toolPivotPos.northing = CVehicle::instance()->tankPos.northing + (cos(CVehicle::instance()->toolPivotPos.heading) * (tool.trailingHitchLength));
        }

        CVehicle::instance()->toolPos.heading = CVehicle::instance()->toolPivotPos.heading;
        CVehicle::instance()->toolPos.easting = CVehicle::instance()->tankPos.easting +
                                  (sin(CVehicle::instance()->toolPivotPos.heading) * (tool.trailingHitchLength - tool.trailingToolToPivotLength));
        CVehicle::instance()->toolPos.northing = CVehicle::instance()->tankPos.northing +
                                   (cos(CVehicle::instance()->toolPivotPos.heading) * (tool.trailingHitchLength - tool.trailingToolToPivotLength));

    }

    //rigidly connected to vehicle
    else
    {
        CVehicle::instance()->toolPivotPos.heading = CVehicle::instance()->fixHeading;
        CVehicle::instance()->toolPivotPos.easting = CVehicle::instance()->hitchPos.easting;
        CVehicle::instance()->toolPivotPos.northing = CVehicle::instance()->hitchPos.northing;

        CVehicle::instance()->toolPos.heading = CVehicle::instance()->fixHeading;
        CVehicle::instance()->toolPos.easting = CVehicle::instance()->hitchPos.easting;
        CVehicle::instance()->toolPos.northing = CVehicle::instance()->hitchPos.northing;
    }

    //#endregion

    //used to increase triangle count when going around corners, less on straight
    //pick the slow moving side edge of tool
    double distance = tool.width * 0.5;
    if (distance > 5) distance = 5;

    //whichever is less
    if (tool.farLeftSpeed < tool.farRightSpeed)
    {
        double twist = tool.farLeftSpeed / tool.farRightSpeed;
        twist *= twist;
        if (twist < 0.2) twist = 0.2;
        CVehicle::instance()->sectionTriggerStepDistance = distance * twist * twist;
    }
    else
    {
        double twist = tool.farRightSpeed / tool.farLeftSpeed;
        //twist *= twist;
        if (twist < 0.2) twist = 0.2;

        CVehicle::instance()->sectionTriggerStepDistance = distance * twist * twist;
    }

    //finally fixed distance for making a curve line
    if (!CTrack::instance()->curve.isMakingCurve) CVehicle::instance()->sectionTriggerStepDistance = CVehicle::instance()->sectionTriggerStepDistance + 0.5;
    //if (ct.isContourBtnOn) CVehicle::instance()->sectionTriggerStepDistance *=0.5;

    //precalc the sin and cos of heading * -1
    CVehicle::instance()->sinSectionHeading = sin(-CVehicle::instance()->toolPivotPos.heading);
    CVehicle::instance()->cosSectionHeading = cos(-CVehicle::instance()->toolPivotPos.heading);
}

//calculate the extreme tool left, right velocities, each section lookahead, and whether or not its going backwards
void FormGPS::CalculateSectionLookAhead(double northing, double easting, double cosHeading, double sinHeading)
{
    //calculate left side of section 1
    Vec2 left;
    Vec2 right = left;
    double leftSpeed = 0, rightSpeed = 0;

    //speed max for section kmh*0.277 to m/s * 10 cm per pixel * 1.7 max speed
    double meterPerSecPerPixel = fabs(CVehicle::instance()->avgSpeed) * 4.5;
    //qDebug() << pn.speed << ", m/s per pixel is " << meterPerSecPerPixel;

    //now loop all the section rights and the one extreme left
    for (int j = 0; j < tool.numOfSections; j++)
    {
        if (j == 0)
        {
            //only one first left point, the rest are all rights moved over to left
            tool.section[j].leftPoint = Vec2(cosHeading * (tool.section[j].positionLeft) + easting,
                                             sinHeading * (tool.section[j].positionLeft) + northing);

            left = tool.section[j].leftPoint - tool.section[j].lastLeftPoint;

            //save a copy for next time
            tool.section[j].lastLeftPoint = tool.section[j].leftPoint;

            //get the speed for left side only once

            leftSpeed = left.getLength() * gpsHz * 10;
            //qDebug() << leftSpeed << " - left speed";
            if (leftSpeed > meterPerSecPerPixel) leftSpeed = meterPerSecPerPixel;
        }
        else
        {
            //right point from last section becomes this left one
            tool.section[j].leftPoint = tool.section[j - 1].rightPoint;
            left = tool.section[j].leftPoint - tool.section[j].lastLeftPoint;

            //save a copy for next time
            tool.section[j].lastLeftPoint = tool.section[j].leftPoint;

            //save the slower of the 2
            if (leftSpeed > rightSpeed) leftSpeed = rightSpeed;
        }

        tool.section[j].rightPoint = Vec2(cosHeading * (tool.section[j].positionRight) + easting,
                                          sinHeading * (tool.section[j].positionRight) + northing);
        /*
        qDebug() << j << ": " << tool.section[j].leftPoint.easting << "," <<
                                 tool.section[j].leftPoint.northing <<" " <<
                                 tool.section[j].rightPoint.easting << ", " <<
                                 tool.section[j].rightPoint.northing;
                                 */


        //now we have left and right for this section
        right = tool.section[j].rightPoint - tool.section[j].lastRightPoint;

        //save a copy for next time
        tool.section[j].lastRightPoint = tool.section[j].rightPoint;

        //grab vector length and convert to meters/sec/10 pixels per meter
        rightSpeed = right.getLength() * gpsHz * 10;
        if (rightSpeed > meterPerSecPerPixel) rightSpeed = meterPerSecPerPixel;

        //Is section outer going forward or backward
        double head = left.headingXZ();

        if (head < 0) head += glm::twoPI;

        if (M_PI - fabs(fabs(head - CVehicle::instance()->toolPos.heading) - M_PI) > glm::PIBy2)
        {
            if (leftSpeed > 0) leftSpeed *= -1;
        }

        head = right.headingXZ();
        if (head < 0) head += glm::twoPI;
        if (M_PI - fabs(fabs(head - CVehicle::instance()->toolPos.heading) - M_PI) > glm::PIBy2)
        {
            if (rightSpeed > 0) rightSpeed *= -1;
        }

        double sped = 0;
        //save the far left and right speed in m/sec averaged over 20%
        if (j==0)
        {
            sped = (leftSpeed * 0.1);
            if (sped < 0.1) sped = 0.1;
            tool.farLeftSpeed = tool.farLeftSpeed * 0.7 + sped * 0.3;
            //qWarning() << sped << tool.farLeftSpeed << CVehicle::instance()->avgSpeed;
        }

        if (j == tool.numOfSections - 1)
        {
            sped = (rightSpeed * 0.1);
            if(sped < 0.1) sped = 0.1;
            tool.farRightSpeed = tool.farRightSpeed * 0.7 + sped * 0.3;
        }
        //choose fastest speed
        if (leftSpeed > rightSpeed)
        {
            sped = leftSpeed;
            leftSpeed = rightSpeed;
        }
        else sped = rightSpeed;
        tool.section[j].speedPixels = tool.section[j].speedPixels * 0.7 + sped * 0.3;
    }
}

//perimeter and boundary point generation
void FormGPS::AddBoundaryPoint()
{
    //save the north & east as previous
    prevBoundaryPos.easting = pn.fix.easting;
    prevBoundaryPos.northing = pn.fix.northing;

    //build the boundary line
    if (bnd.isOkToAddPoints)
    {
        if (bnd.isDrawRightSide)
        {
            //Right side
            Vec3 point(CVehicle::instance()->pivotAxlePos.easting + sin(CVehicle::instance()->pivotAxlePos.heading - glm::PIBy2) * -(double)bnd.createBndOffset,
                       CVehicle::instance()->pivotAxlePos.northing + cos(CVehicle::instance()->pivotAxlePos.heading - glm::PIBy2) * -(double)bnd.createBndOffset,
                       CVehicle::instance()->pivotAxlePos.heading);
            bnd.bndBeingMadePts.append(point);
        }

        //draw on left side
        else
        {
            //Right side
            Vec3 point(CVehicle::instance()->pivotAxlePos.easting + sin(CVehicle::instance()->pivotAxlePos.heading - glm::PIBy2) * (double)bnd.createBndOffset,
                       CVehicle::instance()->pivotAxlePos.northing + cos(CVehicle::instance()->pivotAxlePos.heading - glm::PIBy2) * (double)bnd.createBndOffset,
                       CVehicle::instance()->pivotAxlePos.heading);
            bnd.bndBeingMadePts.append(point);
        }
        boundary_calculate_area(); //in formgps_ui_boundary.cpp
    }
}

void FormGPS::AddContourPoints()
{
    //if (isConstantContourOn)
    {
        //record contour all the time
        //Contour Base Track.... At least One section on, turn on if not
        if (patchCounter != 0)
        {
            //keep the line going, everything is on for recording path
            if (ct.isContourOn) ct.AddPoint(CVehicle::instance()->pivotAxlePos);
            else
            {
                ct.StartContourLine();
                ct.AddPoint(CVehicle::instance()->pivotAxlePos);
            }
        }

        //All sections OFF so if on, turn off
        else
        {
            if (ct.isContourOn)
            { ct.StopContourLine(contourSaveList); }
        }

        //Build contour line if close enough to a patch
        if (ct.isContourBtnOn) ct.BuildContourGuidanceLine(secondsSinceStart, *CVehicle::instance(), CVehicle::instance()->pivotAxlePos);
    }
    //save the north & east as previous
    prevContourPos.northing = CVehicle::instance()->pivotAxlePos.northing;
    prevContourPos.easting = CVehicle::instance()->pivotAxlePos.easting;
}

//add the points for section, contour line points, Area Calc feature
void FormGPS::AddSectionOrPathPoints()
{
    if (recPath.isRecordOn)
    {
        //keep minimum speed of 1.0
        double speed = CVehicle::instance()->avgSpeed;
        if (CVehicle::instance()->avgSpeed < 1.0) speed = 1.0;
        bool autoBtn = (autoBtnState == btnStates::Auto);

        recPath.recList.append(CRecPathPt(CVehicle::instance()->pivotAxlePos.easting, CVehicle::instance()->pivotAxlePos.northing, CVehicle::instance()->pivotAxlePos.heading, speed, autoBtn));
    }

    CTrack::instance()->AddPathPoint(CVehicle::instance()->pivotAxlePos);

    //save the north & east as previous
    prevSectionPos.northing = pn.fix.northing;
    prevSectionPos.easting = pn.fix.easting;

    // if non zero, at least one section is on.
    patchCounter = 0;

    //send the current and previous GPS fore/aft corrected fix to each section
    for (int j = 0; j < triStrip.count(); j++)
    {
        if (triStrip[j].isDrawing)
        {
            if ((bool)isPatchesChangingColor)
            {
                triStrip[j].numTriangles = 64;
                isPatchesChangingColor = false;
            }

            triStrip[j].AddMappingPoint(tool, fd, j);
            patchCounter++;
        }
    }
}

//the start of first few frames to initialize entire program
void FormGPS::InitializeFirstFewGPSPositions()
{
    if (!isFirstFixPositionSet)
    {
        if (!isJobStarted)
        {
            pn.latStart = pn.latitude;
            pn.lonStart = pn.longitude;
            pn.SetLocalMetersPerDegree();
        }

        pn.ConvertWGS84ToLocal(pn.latitude, pn.longitude, pn.fix.northing, pn.fix.easting);

        //Draw a grid once we know where in the world we are.
        isFirstFixPositionSet = true;

        //most recent fixes
        prevFix.easting = pn.fix.easting;
        prevFix.northing = pn.fix.northing;

        //run once and return
        isFirstFixPositionSet = true;

        return;
    }

    else
    {

        //most recent fixes
        prevFix.easting = pn.fix.easting; prevFix.northing = pn.fix.northing;

        //keep here till valid data
        if (startCounter > (20))
        {
            isGPSPositionInitialized = true;
            lastReverseFix = pn.fix;
        }

        //in radians
        CVehicle::instance()->fixHeading = 0;
        CVehicle::instance()->toolPos.heading = CVehicle::instance()->fixHeading;

        //send out initial zero settings
        if (isGPSPositionInitialized)
        {
            //TODO determine if it is day from wall clock and date
            isDayTime = true;

            camera.SetZoom();
        }
        return;
    }
}
