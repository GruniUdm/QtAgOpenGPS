#include "ctool.h"
#include "cvehicle.h"
#include "glm.h"
#include "newsettings.h"
#include "glutils.h"
#include "ccamera.h"
#include "ctram.h"

void CTool::loadSettings()
{
    //from settings grab the vehicle specifics
    trailingToolToPivotLength = settings->value(SETTINGS_tool_trailingToolToPivotLength).value<double>();

    width = settings->value(SETTINGS_vehicle_toolWidth).value<double>();
    overlap = settings->value(SETTINGS_vehicle_toolOverlap).value<double>();

    offset = settings->value(SETTINGS_vehicle_toolOffset).value<double>();

    trailingHitchLength = settings->value(SETTINGS_tool_toolTrailingHitchLength).value<double>();
    tankTrailingHitchLength = settings->value(SETTINGS_vehicle_tankTrailingHitchLength).value<double>();
    hitchLength = settings->value(SETTINGS_vehicle_hitchLength).value<double>();

    isToolRearFixed = settings->value(SETTINGS_tool_isToolRearFixed).value<bool>();
    isToolTrailing = settings->value(SETTINGS_tool_isToolTrailing).value<bool>();
    isToolTBT = settings->value(SETTINGS_tool_isTBT).value<bool>();
    isToolFrontFixed = settings->value(SETTINGS_tool_isToolFront).value<bool>();

    lookAheadOnSetting = settings->value(SETTINGS_vehicle_toolLookAheadOn).value<double>();
    lookAheadOffSetting = settings->value(SETTINGS_vehicle_toolLookAheadOff).value<double>();
    turnOffDelay = settings->value(SETTINGS_vehicle_toolOffDelay).value<double>();

    isSectionOffWhenOut = settings->value(SETTINGS_tool_isSectionOffWhenOut).value<bool>();

    isSectionsNotZones = settings->value(SETTINGS_tool_isSectionsNotZones).value<bool>();

    if (isSectionsNotZones)
        numOfSections = settings->value(SETTINGS_vehicle_numSections).value<int>();
    else
        numOfSections = settings->value(SETTINGS_tool_numSectionsMulti).value<int>();

    minCoverage = settings->value(SETTINGS_vehicle_minCoverage).value<double>();
    isMultiColoredSections = settings->value(SETTINGS_color_isMultiColorSections).value<bool>();

    secColors[0] = settings->value(SETTINGS_color_sec01).value<QColor>();
    secColors[1] = settings->value(SETTINGS_color_sec02).value<QColor>();
    secColors[2] = settings->value(SETTINGS_color_sec03).value<QColor>();
    secColors[3] = settings->value(SETTINGS_color_sec04).value<QColor>();
    secColors[4] = settings->value(SETTINGS_color_sec05).value<QColor>();
    secColors[5] = settings->value(SETTINGS_color_sec06).value<QColor>();
    secColors[6] = settings->value(SETTINGS_color_sec07).value<QColor>();
    secColors[7] = settings->value(SETTINGS_color_sec08).value<QColor>();
    secColors[8] = settings->value(SETTINGS_color_sec09).value<QColor>();
    secColors[9] = settings->value(SETTINGS_color_sec10).value<QColor>();
    secColors[10] = settings->value(SETTINGS_color_sec11).value<QColor>();
    secColors[11] = settings->value(SETTINGS_color_sec12).value<QColor>();
    secColors[12] = settings->value(SETTINGS_color_sec13).value<QColor>();
    secColors[13] = settings->value(SETTINGS_color_sec14).value<QColor>();
    secColors[14] = settings->value(SETTINGS_color_sec15).value<QColor>();
    secColors[15] = settings->value(SETTINGS_color_sec16).value<QColor>();

    for (int c=0 ; c < 16; c++) {
        //check setColor[C] to make sure there's nothing over 254
    }

    zoneRanges = toVector<int>(settings->value(SETTINGS_tool_zones));
    zones = zoneRanges[0];
    //zoneRanges.removeAt(0); //remove first element since it was a count

    isDisplayTramControl = settings->value(SETTINGS_tool_isDisplayTramControl).value<bool>();

}


CTool::CTool()
{
    loadSettings();
}

void CTool::DrawTool(QOpenGLFunctions *gl, QMatrix4x4 &modelview, QMatrix4x4 projection,
                     bool isJobStarted,
                     CVehicle &v, CCamera &camera, CTram &tram)
{
    double tram_halfWheelTrack = settings->value(SETTINGS_vehicle_trackWidth).value<double>() * 0.5;
    bool tool_isDisplayTramControl = settings->value(SETTINGS_tool_isDisplayTramControl).value<bool>();
    //translate and rotate at pivot axle, caller's mvp will be changed
    //all subsequent draws will be based on this point
    modelview.translate(v.pivotAxlePos.easting, v.pivotAxlePos.northing, 0);

    GLHelperOneColor gldraw;

    QMatrix4x4 mv = modelview; //push matrix (just have to save it)

    //translate down to the hitch pin
    mv.translate(sin(v.fixHeading) * hitchLength,
                            cos(v.fixHeading) * hitchLength, 0);

    //settings doesn't change trailing hitch length if set to rigid, so do it here
    double trailingTank, trailingTool;
    if (isToolTrailing)
    {
        trailingTank = tankTrailingHitchLength;
        trailingTool = trailingHitchLength;
    }
    else { trailingTank = 0; trailingTool = 0; }

    //there is a trailing tow between hitch
    if (isToolTBT && isToolTrailing)
    {
        //rotate to tank heading
        mv.rotate(glm::toDegrees(-v.tankPos.heading), 0.0, 0.0, 1.0);


        //draw the tank hitch
        //draw the rigid hitch
        gldraw.append(QVector3D(-0.57, trailingTank, 0.0));
        gldraw.append(QVector3D(0, 0, 0));
        gldraw.append(QVector3D(0.57,trailingTank, 0.0));
        gldraw.draw(gl,projection*mv,QColor::fromRgbF(0.0, 0.0, 0.0),GL_LINE_LOOP, 6.0f);

        //draw the rigid hitch
        gldraw.draw(gl,projection*mv,QColor::fromRgbF(0.765f, 0.76f, 0.32f),GL_LINE_LOOP, 6.0f);

        //move down the tank hitch, unwind, rotate to section heading
        mv.translate(0.0, trailingTank, 0.0);
        mv.rotate(glm::toDegrees(v.tankPos.heading), 0.0, 0.0, 1.0);
        mv.rotate(glm::toDegrees(-v.toolPos.heading), 0.0, 0.0, 1.0);
    }

    //no tow between hitch
    else
    {
        mv.rotate(glm::toDegrees(-v.toolPos.heading), 0.0, 0.0, 1.0);
    }

    //draw the hitch if trailing
    if (isToolTrailing)
    {
        gldraw.clear();
        gldraw.append(QVector3D(-0.4 + offset, trailingTool, 0.0));
        gldraw.append(QVector3D(0,0,0));
        gldraw.append(QVector3D(0.4 + offset, trailingTool, 0.0));
        gldraw.draw(gl,projection*mv,QColor::fromRgbF(0.0f, 0.0f, 0.0f),GL_LINE_STRIP, 6.0f);

        gldraw.draw(gl,projection*mv,QColor::fromRgbF(0.7f, 0.4f, 0.2f),GL_LINE_STRIP, 1.0f);

        GLHelperTexture gldrawtex;

        gldrawtex.append( { QVector3D(1.5 + offset, trailingTool + 1, 0), QVector2D(1,0) } ); //Top Right
        gldrawtex.append( { QVector3D(-1.5 + offset, trailingTool + 1, 0), QVector2D(0,0) } ); //Top Left
        gldrawtex.append( { QVector3D(1.5 + offset, trailingTool - 1, 0), QVector2D(1,1) } ); //Bottom Right
        gldrawtex.append( { QVector3D(-1.5 + offset, trailingTool - 1, 0), QVector2D(0,1) } ); //Bottom LEft
        gldrawtex.draw(gl, projection*mv,Textures::TOOLWHEELS, GL_TRIANGLE_STRIP, true, QColor::fromRgbF(1,1,1,0.75));
    }

    trailingTool -= trailingToolToPivotLength;

    //look ahead lines
    GLHelperColors gldrawcolors;
    ColorVertex cv;
    QColor color;


    if (isJobStarted)
    {
        //lookahead section on
        cv.color = QVector4D(0.20f, 0.7f, 0.2f, 1);
        cv.vertex = QVector3D(farLeftPosition, (lookAheadDistanceOnPixelsLeft) * 0.1 + trailingTool, 0);
        gldrawcolors.append(cv);
        cv.vertex = QVector3D(farRightPosition, (lookAheadDistanceOnPixelsRight) * 0.1 + trailingTool, 0);
        gldrawcolors.append(cv);

        //lookahead section off
        cv.color = QVector4D(0.70f, 0.2f, 0.2f, 1);
        cv.vertex = QVector3D(farLeftPosition, (lookAheadDistanceOffPixelsLeft) * 0.1 + trailingTool, 0);
        gldrawcolors.append(cv);
        cv.vertex = QVector3D(farRightPosition, (lookAheadDistanceOffPixelsRight) * 0.1 + trailingTool, 0);
        gldrawcolors.append(cv);


        if (v.isHydLiftOn)
        {
            cv.color = QVector4D(0.70f, 0.2f, 0.72f, 1);
            cv.vertex = QVector3D(section[0].positionLeft, (v.hydLiftLookAheadDistanceLeft * 0.1) + trailingTool, 0);
            gldrawcolors.append(cv);
            cv.vertex = QVector3D(section[numOfSections - 1].positionRight, (v.hydLiftLookAheadDistanceRight * 0.1) + trailingTool, 0);
            gldrawcolors.append(cv);
        }

        gldrawcolors.draw(gl, projection * mv, GL_LINES, 1.0);
    }
    //draw the sections
    //line width 2 now

    double hite = camera.camSetDistance / -150;
    if (hite > 12) hite = 12;
    if (hite < 1) hite = 1;

    for (int j = 0; j < numOfSections; j++)
    {
        //if section is on, green, if off, red color
        if (section[j].isSectionOn)
        {
            if (sectionButtonState.get(j) == btnStates::Auto)
            {
                if (section[j].isMappingOn) color.setRgbF(0.0f, 0.95f, 0.0f, 1.0f);
                else color.setRgbF(0.970f, 0.30f, 0.970f);
            }
            else color.setRgbF(0.97, 0.97, 0, 1.0f);
        }
        else
        {
            if (!section[j].isMappingOn)
                color.setRgbF(0.950f, 0.2f, 0.2f, 1.0f);
            else
                color.setRgbF(0.0f, 0.250f, 0.97f, 1.0f);
        }

        double mid = (section[j].positionRight - section[j].positionLeft) / 2 + section[j].positionLeft;

        gldraw.clear();
        gldraw.append(QVector3D(section[j].positionLeft, trailingTool, 0));
        gldraw.append(QVector3D(section[j].positionLeft, trailingTool - hite, 0));

        gldraw.append(QVector3D(mid, trailingTool - hite * 1.5, 0));

        gldraw.append(QVector3D(section[j].positionRight, trailingTool - hite, 0));
        gldraw.append(QVector3D(section[j].positionRight, trailingTool, 0));

        gldraw.draw(gl, projection * mv, color, GL_TRIANGLE_FAN, 2.0f);

        if (camera.camSetDistance > -width * 200)
        {
            color.setRgbF(0.0, 0.0, 0.0);
            gldraw.draw(gl,projection * mv, color, GL_LINE_LOOP, 1.0);
        }
    }

    //zones
    if (!isSectionsNotZones && zones > 0 && camera.camSetDistance > -150)
    {
        gldraw.clear();
        color.setRgbF(0.5f, 0.80f, 0.950f);
        for (int i = 1; i < zones; i++)
        {
            gldraw.append(QVector3D(section[zoneRanges[i]].positionLeft, trailingTool - 0.4, 0));
            gldraw.append(QVector3D(section[zoneRanges[i]].positionLeft, trailingTool + 0.2, 0));
        }
        gldraw.draw(gl, projection * mv, color, GL_LINES, 2.0f);
    }

    float pointSize;

    //tram Dots
    if ( tool_isDisplayTramControl && tram.displayMode != 0)
    {
        if (camera.camSetDistance > -300)
        {
            if (camera.camSetDistance > -100)
                pointSize = 16;
            else pointSize = 12;

            if (tram.isOuter)
            {
                //section markers
                gldrawcolors.clear();
                //right side
                if (((tram.controlByte) & 1) == 1) cv.color = QVector4D(0.0f, 0.900f, 0.39630f,1.0f);
                else cv.color = QVector4D(0, 0, 0, 1.0f);
                cv.vertex = QVector3D(farRightPosition - tram_halfWheelTrack, trailingTool, 0);
                gldrawcolors.append(cv);

                //left side
                if ((tram.controlByte & 2) == 2) cv.color = QVector4D(0.0f, 0.900f, 0.3930f, 1.0f);
                else cv.color = QVector4D(0, 0, 0, 1.0f);
                cv.vertex = QVector3D(farLeftPosition + tram_halfWheelTrack, trailingTool, 0);
                gldrawcolors.append(cv);

                gldrawcolors.draw(gl, projection * mv,GL_POINTS, pointSize);
            }
            else
            {
                gldrawcolors.clear();

                //right side
                if (((tram.controlByte) & 1) == 1) cv.color = QVector4D(0.0f, 0.900f, 0.39630f, 1.0f);
                else cv.color = QVector4D(0, 0, 0, 1.0f);
                cv.vertex = QVector3D(tram_halfWheelTrack, trailingTool, 0);
                gldrawcolors.append(cv);

                //left side
                if ((tram.controlByte & 2) == 2) cv.color = QVector4D(0.0f, 0.900f, 0.3930f, 1.0f);
                else cv.color = QVector4D(0, 0, 0, 1.0f);
                cv.vertex = QVector3D(-tram_halfWheelTrack, trailingTool, 0);
                gldrawcolors.append(cv);
                gldrawcolors.draw(gl, projection * mv, GL_POINTS, pointSize);
            }
        }
    }
}

void CTool::sectionCalcWidths()
{
    if (isSectionsNotZones)
    {
        for (int j = 0; j < MAXSECTIONS; j++)
        {
            section[j].sectionWidth = (section[j].positionRight - section[j].positionLeft);
            section[j].rpSectionPosition = 250 + (int)(glm::roundMidAwayFromZero(section[j].positionLeft * 10));
            section[j].rpSectionWidth = (int)(glm::roundMidAwayFromZero(section[j].sectionWidth * 10));
        }

        //calculate tool width based on extreme right and left values
        double width = fabs(section[0].positionLeft) + fabs(section[numOfSections - 1].positionRight);
        settings->setValue(SETTINGS_vehicle_toolWidth, width);

        //left and right tool position
        farLeftPosition = section[0].positionLeft;
        farRightPosition = section[numOfSections - 1].positionRight;

        //find the right side pixel position
        rpXPosition = 250 + (int)(glm::roundMidAwayFromZero(farLeftPosition * 10));
        rpWidth = (int)(glm::roundMidAwayFromZero(width * 10));
    }
}

//moved from main form to here
void CTool::sectionCalcMulti()
{
    double leftside = width / -2.0;
    double defaultSectionWidth = settings->value(SETTINGS_tool_sectionWidthMulti).value<double>();
    double offset = settings->value(SETTINGS_vehicle_toolOffset).value<double>();
    section[0].positionLeft = leftside+offset;

    for (int i = 0; i < numOfSections - 1; i++)
    {
        leftside += defaultSectionWidth;

        section[i].positionRight = leftside + offset;
        section[i + 1].positionLeft = leftside + offset;
        section[i].sectionWidth = defaultSectionWidth;
        section[i].rpSectionPosition = 250 + (int)(glm::roundMidAwayFromZero(section[i].positionLeft * 10));
        section[i].rpSectionWidth = (int)(glm::roundMidAwayFromZero(section[i].sectionWidth * 10));
    }

    leftside += defaultSectionWidth;
    section[numOfSections - 1].positionRight = leftside + offset;
    section[numOfSections - 1].sectionWidth = defaultSectionWidth;
    section[numOfSections - 1].rpSectionPosition = 250 + (int)(glm::roundMidAwayFromZero(section[numOfSections - 1].positionLeft * 10));
    section[numOfSections - 1].rpSectionWidth = (int)(glm::roundMidAwayFromZero(section[numOfSections - 1].sectionWidth * 10));

    //calculate tool width based on extreme right and left values
    width = (section[numOfSections - 1].positionRight) - (section[0].positionLeft);
    settings->setValue(SETTINGS_vehicle_toolWidth, width);

    //left and right tool position
    farLeftPosition = section[0].positionLeft;
    farRightPosition = section[numOfSections - 1].positionRight;

    //find the right side pixel position
    rpXPosition = 250 + (int)(glm::roundMidAwayFromZero(farLeftPosition * 10));
    rpWidth = (int)(glm::roundMidAwayFromZero(width * 10));

}


void CTool::sectionSetPositions()
{
    double vehicle_toolOffset = settings->value(SETTINGS_vehicle_toolOffset).value<double>();
    double section_position1 = settings->value(SETTINGS_section_position1).value<double>();
    double section_position2 = settings->value(SETTINGS_section_position2).value<double>();
    double section_position3 = settings->value(SETTINGS_section_position3).value<double>();
    double section_position4 = settings->value(SETTINGS_section_position4).value<double>();
    double section_position5 = settings->value(SETTINGS_section_position5).value<double>();
    double section_position6 = settings->value(SETTINGS_section_position6).value<double>();
    double section_position7 = settings->value(SETTINGS_section_position7).value<double>();
    double section_position8 = settings->value(SETTINGS_section_position8).value<double>();
    double section_position9 = settings->value(SETTINGS_section_position9).value<double>();
    double section_position10 = settings->value(SETTINGS_section_position10).value<double>();
    double section_position11 = settings->value(SETTINGS_section_position11).value<double>();
    double section_position12 = settings->value(SETTINGS_section_position12).value<double>();
    double section_position13 = settings->value(SETTINGS_section_position13).value<double>();
    double section_position14 = settings->value(SETTINGS_section_position14).value<double>();
    double section_position15 = settings->value(SETTINGS_section_position15).value<double>();
    double section_position16 = settings->value(SETTINGS_section_position16).value<double>();
    double section_position17 = settings->value(SETTINGS_section_position17).value<double>();

    section[0].positionLeft = section_position1 + vehicle_toolOffset;
    section[0].positionRight = section_position2 + vehicle_toolOffset;

    section[1].positionLeft = section_position2 + vehicle_toolOffset;
    section[1].positionRight = section_position3 + vehicle_toolOffset;

    section[2].positionLeft = section_position3 + vehicle_toolOffset;
    section[2].positionRight = section_position4 + vehicle_toolOffset;

    section[3].positionLeft = section_position4 + vehicle_toolOffset;
    section[3].positionRight = section_position5 + vehicle_toolOffset;

    section[4].positionLeft = section_position5 + vehicle_toolOffset;
    section[4].positionRight = section_position6 + vehicle_toolOffset;

    section[5].positionLeft = section_position6 + vehicle_toolOffset;
    section[5].positionRight = section_position7 + vehicle_toolOffset;

    section[6].positionLeft = section_position7 + vehicle_toolOffset;
    section[6].positionRight = section_position8 + vehicle_toolOffset;

    section[7].positionLeft = section_position8 + vehicle_toolOffset;
    section[7].positionRight = section_position9 + vehicle_toolOffset;

    section[8].positionLeft = section_position9 + vehicle_toolOffset;
    section[8].positionRight = section_position10 + vehicle_toolOffset;

    section[9].positionLeft = section_position10 + vehicle_toolOffset;
    section[9].positionRight = section_position11 + vehicle_toolOffset;

    section[10].positionLeft = section_position11 + vehicle_toolOffset;
    section[10].positionRight = section_position12 + vehicle_toolOffset;

    section[11].positionLeft = section_position12 + vehicle_toolOffset;
    section[11].positionRight = section_position13 + vehicle_toolOffset;

    section[12].positionLeft = section_position13 + vehicle_toolOffset;
    section[12].positionRight = section_position14 + vehicle_toolOffset;

    section[13].positionLeft = section_position14 + vehicle_toolOffset;
    section[13].positionRight = section_position15 + vehicle_toolOffset;

    section[14].positionLeft = section_position15 + vehicle_toolOffset;
    section[14].positionRight = section_position16 + vehicle_toolOffset;

    section[15].positionLeft = section_position16 + vehicle_toolOffset;
    section[15].positionRight = section_position17 + vehicle_toolOffset;
}
