#include "ctool.h"
#include "glm.h"
#include "glutils.h"
#include "classes/settingsmanager.h"
#include "glutils.h"
#include "ccamera.h"
#include "ctram.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QLabel>

extern QLabel *overlapPixelsWindow;
extern QOpenGLShaderProgram *interpColorShader;

Q_LOGGING_CATEGORY (ctool, "ctool.qtagopengps")

struct PatchBuffer {
    QOpenGLBuffer patchBuffer;
    int length;
};

struct PatchInBuffer {
    int which;
    int offset;
    int length;
};

#define PATCHBUFFER_LENGTH 16 * 1024 * 1024 //16 MB
#define VERTEX_SIZE sizeof(ColorVertex) //combined vertex and color, 7 floats

void CTool::loadSettings()
{
    //from settings grab the vehicle specifics
    trailingToolToPivotLength = SettingsManager::instance()->tool_trailingToolToPivotLength();

    width = SettingsManager::instance()->vehicle_toolWidth();
    overlap = SettingsManager::instance()->vehicle_toolOverlap();

    offset = SettingsManager::instance()->vehicle_toolOffset();

    trailingHitchLength = SettingsManager::instance()->tool_toolTrailingHitchLength();
    tankTrailingHitchLength = SettingsManager::instance()->vehicle_tankTrailingHitchLength();
    hitchLength = SettingsManager::instance()->vehicle_hitchLength();

    isToolRearFixed = SettingsManager::instance()->tool_isToolRearFixed();
    isToolTrailing = SettingsManager::instance()->tool_isToolTrailing();
    isToolTBT = SettingsManager::instance()->tool_isTBT();
    isToolFrontFixed = SettingsManager::instance()->tool_isToolFront();

    lookAheadOnSetting = SettingsManager::instance()->vehicle_toolLookAheadOn();
    lookAheadOffSetting = SettingsManager::instance()->vehicle_toolLookAheadOff();
    turnOffDelay = SettingsManager::instance()->vehicle_toolOffDelay();

    isSectionOffWhenOut = SettingsManager::instance()->tool_isSectionOffWhenOut();

    isSectionsNotZones = SettingsManager::instance()->tool_isSectionsNotZones();

    if (isSectionsNotZones)
        numOfSections = SettingsManager::instance()->vehicle_numSections();
    else
        numOfSections = SettingsManager::instance()->tool_numSectionsMulti();

    minCoverage = SettingsManager::instance()->vehicle_minCoverage();
    isMultiColoredSections = SettingsManager::instance()->color_isMultiColorSections();

    secColors[0] = SettingsManager::instance()->color_sec01();
    secColors[1] = SettingsManager::instance()->color_sec02();
    secColors[2] = SettingsManager::instance()->color_sec03();
    secColors[3] = SettingsManager::instance()->color_sec04();
    secColors[4] = SettingsManager::instance()->color_sec05();
    secColors[5] = SettingsManager::instance()->color_sec06();
    secColors[6] = SettingsManager::instance()->color_sec07();
    secColors[7] = SettingsManager::instance()->color_sec08();
    secColors[8] = SettingsManager::instance()->color_sec09();
    secColors[9] = SettingsManager::instance()->color_sec10();
    secColors[10] = SettingsManager::instance()->color_sec11();
    secColors[11] = SettingsManager::instance()->color_sec12();
    secColors[12] = SettingsManager::instance()->color_sec13();
    secColors[13] = SettingsManager::instance()->color_sec14();
    secColors[14] = SettingsManager::instance()->color_sec15();
    secColors[15] = SettingsManager::instance()->color_sec16();

    for (int c=0 ; c < 16; c++) {
        //check setColor[C] to make sure there's nothing over 254
    }

    zoneRanges = SettingsManager::instance()->tool_zones();
    if (zoneRanges.size() > 0) {
        zones = zoneRanges[0];
    } else {
        qDebug() << "ERROR: tool_zones is empty! Size:" << zoneRanges.size();
        zones = 2; // valeur par défaut
    }
    //zoneRanges.removeAt(0); //remove first element since it was a count

    isDisplayTramControl = SettingsManager::instance()->tool_isDisplayTramControl();

}


CTool::CTool()
{
    // Initialize all section button states to Off
    for (int i = 0; i < 65; i++) {
        sectionButtonState[i] = btnStates::Off;
    }
    loadSettings();
}

void CTool::saveSettings()
{
    // Save all tool settings to SettingsManager (mirror of loadSettings)
    SettingsManager::instance()->setTool_trailingToolToPivotLength(trailingToolToPivotLength);

    SettingsManager::instance()->setVehicle_toolWidth(width);
    SettingsManager::instance()->setVehicle_toolOverlap(overlap);
    SettingsManager::instance()->setVehicle_toolOffset(offset);

    SettingsManager::instance()->setTool_toolTrailingHitchLength(trailingHitchLength);
    SettingsManager::instance()->setVehicle_tankTrailingHitchLength(tankTrailingHitchLength);
    SettingsManager::instance()->setVehicle_hitchLength(hitchLength);

    SettingsManager::instance()->setTool_isToolRearFixed(isToolRearFixed);
    SettingsManager::instance()->setTool_isToolTrailing(isToolTrailing);
    SettingsManager::instance()->setTool_isTBT(isToolTBT);
    SettingsManager::instance()->setTool_isToolFront(isToolFrontFixed);

    SettingsManager::instance()->setVehicle_toolLookAheadOn(lookAheadOnSetting);
    SettingsManager::instance()->setVehicle_toolLookAheadOff(lookAheadOffSetting);
    SettingsManager::instance()->setVehicle_toolOffDelay(turnOffDelay);

    SettingsManager::instance()->setTool_isSectionOffWhenOut(isSectionOffWhenOut);
    SettingsManager::instance()->setTool_isSectionsNotZones(isSectionsNotZones);

    if (isSectionsNotZones) {
        SettingsManager::instance()->setVehicle_numSections(numOfSections);
        // Note: sectionWidthMulti is not a member variable, it's accessed directly from settings when needed
    } else {
        SettingsManager::instance()->setTool_numSectionsMulti(numOfSections);
        // Save zone ranges
        SettingsManager::instance()->setTool_zones(zoneRanges);
    }

    SettingsManager::instance()->setTool_isDisplayTramControl(isDisplayTramControl);
}

void CTool::DrawTool(QOpenGLFunctions *gl, QMatrix4x4 mv,
                     QMatrix4x4 projection,
                     bool isJobStarted,
                     bool isHydLiftOn,
                     CCamera &camera, CTram &tram)
{
    double tram_halfWheelTrack = SettingsManager::instance()->vehicle_trackWidth() * 0.5;
    bool tool_isDisplayTramControl = SettingsManager::instance()->tool_isDisplayTramControl();
    //translate and rotate at pivot axle, caller's mvp will be changed
    //all subsequent draws will be based on this point


    GLHelperOneColor gldraw;

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
        mv.rotate(glm::toDegrees(-tankPos.heading), 0.0, 0.0, 1.0);


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
        mv.rotate(glm::toDegrees(tankPos.heading), 0.0, 0.0, 1.0);
        mv.rotate(glm::toDegrees(-toolPos.heading), 0.0, 0.0, 1.0);
    }

    //no tow between hitch
    else
    {
        mv.rotate(glm::toDegrees(-toolPos.heading), 0.0, 0.0, 1.0);
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


        if (isHydLiftOn)
        {
            cv.color = QVector4D(0.70f, 0.2f, 0.72f, 1);
            cv.vertex = QVector3D(section[0].positionLeft, (hydLiftLookAheadDistanceLeft * 0.1) + trailingTool, 0);
            gldrawcolors.append(cv);
            cv.vertex = QVector3D(section[numOfSections - 1].positionRight, (hydLiftLookAheadDistanceRight * 0.1) + trailingTool, 0);
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
        if (sectionButtonState[j] == btnStates::Auto)
        {
            // Mode Auto: couleur dépend de si section vraiment active (dans le champ)
            if (section[j].isSectionOn)
            {
                if (section[j].isMappingOn) color.setRgbF(0.0f, 0.95f, 0.0f, 1.0f);  // Vert si dans champ
                else color.setRgbF(0.970f, 0.30f, 0.970f);  // Magenta si pas de mapping
            }
            else
            {
                color.setRgbF(0.950f, 0.2f, 0.2f, 1.0f);  // Rouge si hors champ
            }
        }
        else if (sectionButtonState[j] == btnStates::On)
        {
            color.setRgbF(0.97, 0.97, 0, 1.0f);  // Jaune pour On (forçé)
        }
        else if (section[j].isSectionOn)
        {
            // Logic originale pour sections actives automatiquement
            if (section[j].isMappingOn) color.setRgbF(0.0f, 0.95f, 0.0f, 1.0f);
            else color.setRgbF(0.970f, 0.30f, 0.970f);
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

void CTool::DrawPatches(QOpenGLFunctions *gl,
                        QMatrix4x4 mvp,
                        int patchCounter,
                        const CCamera &camera,
                        QElapsedTimer &swFrame)
{
    GLHelperOneColor gldraw1;
    int currentPatchBuffer = 0;
    double frustum[24];
    QColor color;

    double sinSectionHeading = sin(-toolPivotPos.heading);
    double cosSectionHeading = cos(-toolPivotPos.heading);

    CalcFrustum(mvp, frustum);

    if (patchesBufferDirty) {
        //destroy all GPU patches buffers
        patchBuffer.clear();
        patchesInBuffer.clear();

        for (int j = 0; j < triStrip.count(); j++) {
            patchesInBuffer.append(QVector<PatchInBuffer>());
            for (int k=0; k < triStrip[j].patchList.size()-1 ; k++) {
                patchesInBuffer[j].append({ -1, -1, -1});
            }
        }

        patchBuffer.append( { QOpenGLBuffer(), 0} );
        patchBuffer[0].patchBuffer.create();
        patchBuffer[0].patchBuffer.bind();
        patchBuffer[0].patchBuffer.allocate(PATCHBUFFER_LENGTH); //16 MB
        patchBuffer[0].patchBuffer.release();
        if (!patchesInBuffer.count()) {
            patchesInBuffer.append(QVector<PatchInBuffer>());
            patchesInBuffer[0].append({ -1, -1, -1});
        }
        currentPatchBuffer = 0;

        patchesBufferDirty = false;
    } else {
        currentPatchBuffer = patchBuffer.count() - 1;
    }

    bool draw_patch = false;
    //int total_vertices = 0;

    //initialize the steps for mipmap of triangles (skipping detail while zooming out)
    int mipmap = 0;
    if (camera.camSetDistance < -800) mipmap = 2;
    if (camera.camSetDistance < -1500) mipmap = 4;
    if (camera.camSetDistance < -2400) mipmap = 8;
    if (camera.camSetDistance < -5000) mipmap = 16;

    if (mipmap > 1)
        qDebug(ctool) << "mipmap is" << mipmap;

    //QVector<GLuint> indices;
    //indices.reserve(PATCHBUFFER_LENGTH / 28 * 3);  //enough to index 16 MB worth of vertices
    QVector<QVector<GLuint>> indices2;
    for (int i=0; i < patchBuffer.size(); i++) {
        indices2.append(QVector<GLuint>());
        indices2[i].reserve(PATCHBUFFER_LENGTH / 28 * 3);
    }

    bool enough_indices = false;

    //draw patches j= # of sections
    for (int j = 0; j < triStrip.count(); j++)
    {
        //every time the section turns off and on is a new patch
        int patchCount = triStrip[j].patchList.size();

        for (int k=0; k < patchCount; k++) {
            QSharedPointer<PatchTriangleList> triList = triStrip[j].patchList[k];
            QVector3D *triListRaw = triList->data();
            int count2 = triList->size();
            //total_vertices += count2;
            draw_patch = false;

            draw_patch = false;
            for (int i = 1; i < count2; i += 3) //first vertice is color
            {
                //determine if point is in frustum or not, if < 0, its outside so abort, z always is 0
                //x is easting, y is northing
                if (frustum[0] * triListRaw[i].x() + frustum[1] * triListRaw[i].y() + frustum[3] <= 0)
                    continue;//right
                if (frustum[4] * triListRaw[i].x() + frustum[5] * triListRaw[i].y() + frustum[7] <= 0)
                    continue;//left
                if (frustum[16] * triListRaw[i].x() + frustum[17] * triListRaw[i].y() + frustum[19] <= 0)
                    continue;//bottom
                if (frustum[20] * triListRaw[i].x() + frustum[21] * triListRaw[i].y() + frustum[23] <= 0)
                    continue;//top
                if (frustum[8] * triListRaw[i].x() + frustum[9] * triListRaw[i].y() + frustum[11] <= 0)
                    continue;//far
                if (frustum[12] * triListRaw[i].x() + frustum[13] * triListRaw[i].y() + frustum[15] <= 0)
                    continue;//near

                //point is in frustum so draw the entire patch. The downside of triangle strips.
                draw_patch = true;
                break;
            }

            if (!draw_patch) continue;
            color.setRgbF((*triList)[0].x(), (*triList)[0].y(), (*triList)[0].z(), 0.596 );

            if (k == patchCount - 1) {
                //If this is the last patch in the list, it's currently being worked on
                //so we don't save this one.
                QOpenGLBuffer triBuffer;

                triBuffer.create();
                triBuffer.bind();

                //triangle lists are now using QVector3D, so we can allocate buffers
                //directly from list data.

                //first vertice is color, so we should skip it
                triBuffer.allocate(triList->data() + 1, (count2-1) * sizeof(QVector3D));
                //triBuffer.allocate(triList->data(), count2 * sizeof(QVector3D));
                triBuffer.release();

                //draw the triangles in each triangle strip
                glDrawArraysColor(gl,mvp,
                                  GL_TRIANGLE_STRIP, color,
                                  triBuffer,GL_FLOAT,count2-1);

                triBuffer.destroy();
                //qDebug(ctool) << "Last patch, not cached.";
                continue;
            } else {
                while (j >= patchesInBuffer.size())
                    patchesInBuffer.append(QVector<PatchInBuffer>());
                while (k >= patchesInBuffer[j].size())
                    patchesInBuffer[j].append({ -1, -1, -1});

                if (patchesInBuffer[j][k].which == -1) {
                    //patch is not in one of the big buffers yet, so allocate it.
                    if ((patchBuffer[currentPatchBuffer].length + (count2-1) * VERTEX_SIZE) >= PATCHBUFFER_LENGTH ) {
                        //add a new buffer because the current one is full.
                        currentPatchBuffer ++;
                        patchBuffer.append( { QOpenGLBuffer(), 0 });
                        patchBuffer[currentPatchBuffer].patchBuffer.create();
                        patchBuffer[currentPatchBuffer].patchBuffer.bind();
                        patchBuffer[currentPatchBuffer].patchBuffer.allocate(PATCHBUFFER_LENGTH); //4MB
                        patchBuffer[currentPatchBuffer].patchBuffer.release();
                        indices2.append(QVector<GLuint>());
                        indices2[currentPatchBuffer].reserve(PATCHBUFFER_LENGTH / 28 * 3);
                    }

                    //there's room for it in the current patch buffer
                    patchBuffer[currentPatchBuffer].patchBuffer.bind();
                    QVector<ColorVertex> temp_patch;
                    temp_patch.reserve(count2-1);
                    for (int i=1; i < count2; i++) {
                        temp_patch.append( { triListRaw[i], QVector4D(triListRaw[0], 0.596) } );
                    }
                    patchBuffer[currentPatchBuffer].patchBuffer.write(patchBuffer[currentPatchBuffer].length,
                                                                      temp_patch.data(),
                                                                      (count2-1) * VERTEX_SIZE);
                    patchesInBuffer[j][k].which = currentPatchBuffer;
                    patchesInBuffer[j][k].offset = patchBuffer[currentPatchBuffer].length / VERTEX_SIZE;
                    patchesInBuffer[j][k].length = count2 - 1;
                    patchBuffer[currentPatchBuffer].length += (count2 - 1) * VERTEX_SIZE;
                    qDebug(ctool) << "buffering" << j << k << patchesInBuffer[j][k].which << ", " << patchBuffer[currentPatchBuffer].length;
                    patchBuffer[currentPatchBuffer].patchBuffer.release();
                }
                //generate list of indices for this patch
                int index_offset = patchesInBuffer[j][k].offset;
                int which_buffer = patchesInBuffer[j][k].which;

                int step = mipmap;
                if (count2 - 1 < mipmap + 2) {
                    for (int i = 1; i < count2 - 2 ; i ++)
                    {
                        if (i % 2) {  //preserve winding order
                            indices2[which_buffer].append(i-1 + index_offset);
                            indices2[which_buffer].append(i   + index_offset);
                            indices2[which_buffer].append(i+1 + index_offset);
                        } else {
                            indices2[which_buffer].append(i-1 + index_offset);
                            indices2[which_buffer].append(i+1   + index_offset);
                            indices2[which_buffer].append(i + index_offset);
                        }

                    }
                } else {
                    //use mipmap to make fewer triangles
                    int last_index2 = indices2[which_buffer].count();

                    int vertex_count = 0;
                    for (int i=1; i < count2; i += step) {
                        //convert triangle strip to triangles
                        if (vertex_count > 2 ) { //even, normal winding
                            indices2[which_buffer].append(indices2[which_buffer][last_index2 - 1]);
                            indices2[which_buffer].append(indices2[which_buffer][last_index2 - 2]);
                            last_index2+=3;
                        } else {
                            last_index2 ++;
                        }
                        indices2[which_buffer].append(i-1 + index_offset);

                        i++;
                        vertex_count++;

                        if (vertex_count > 2) { //odd, reverse winding
                            indices2[which_buffer].append(indices2[which_buffer][last_index2 - 2]);
                        }
                        indices2[which_buffer].append(i-1 + index_offset);

                        if (vertex_count > 2) {
                            indices2[which_buffer].append(indices2[which_buffer][last_index2 - 1 ]);
                            last_index2 += 3;
                        } else {
                            last_index2 ++;
                        }
                        i++;
                        vertex_count++;

                        if (count2 - i <= (mipmap + 2))
                            //too small to mipmap, so add each one
                            //individually.
                            step = 0;
                    }
                }
                if (indices2[which_buffer].count() > 2)
                    enough_indices = true;
            }
        }

        qDebug(ctool) << "time after preparing patches for drawing" << swFrame.nsecsElapsed() / 1000000;

        if (enough_indices) {
            interpColorShader->bind();
            interpColorShader->setUniformValue("mvpMatrix", mvp);
            interpColorShader->setUniformValue("pointSize", 0.0f);

            //glDrawElements needs a vertex array object to hold state
            QOpenGLVertexArrayObject vao;
            vao.create();
            vao.bind();

            //create ibo
            QOpenGLBuffer ibo{QOpenGLBuffer::IndexBuffer};
            ibo.create();

            for (int i=0; i < indices2.count(); i++) {
                if (indices2[i].count() > 2) {
                    patchBuffer[i].patchBuffer.bind();

                    //set up vertex positions in buffer for the shader
                    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr); //3D vector
                    gl->glEnableVertexAttribArray(0);

                    gl->glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float))); //color
                    gl->glEnableVertexAttribArray(1);


                    ibo.bind();
                    ibo.allocate(indices2[i].data(), indices2[i].size() * sizeof(GLuint));

                    gl->glDrawElements(GL_TRIANGLES, indices2[i].count(), GL_UNSIGNED_INT, nullptr);
                    patchBuffer[i].patchBuffer.release();

                    ibo.release();
                }
            }
            ibo.destroy();
            vao.release();
            vao.destroy();
            interpColorShader->release();
        }
    }

    // the follow up to sections patches
    int patchCount = 0;

    if (patchCounter > 0)
    {
        color = SettingsManager::instance()->display_colorSectionsDay();
        if (SettingsManager::instance()->display_isDayMode())
            color.setAlpha(152);
        else
            color.setAlpha(76);

        for (int j = 0; j < triStrip.count(); j++)
        {
            if (triStrip[j].isDrawing)
            {
                if (isMultiColoredSections)
                {
                    color = secColors[j];
                    color.setAlpha(152);
                }
                patchCount = triStrip[j].patchList.count();

               //draw the triangle in each triangle strip
                gldraw1.clear();

                //left side of triangle
                QVector3D pt((cosSectionHeading * section[triStrip[j].currentStartSectionNum].positionLeft) + toolPos.easting,
                             (sinSectionHeading * section[triStrip[j].currentStartSectionNum].positionLeft) + toolPos.northing, 0);
                gldraw1.append(pt);

                //Right side of triangle
                pt = QVector3D((cosSectionHeading * section[triStrip[j].currentEndSectionNum].positionRight) + toolPos.easting,
                               (sinSectionHeading * section[triStrip[j].currentEndSectionNum].positionRight) + toolPos.northing, 0);
                gldraw1.append(pt);

                int last = triStrip[j].patchList[patchCount -1]->count();
                //antenna
                gldraw1.append(QVector3D((*triStrip[j].patchList[patchCount-1])[last-2].x(), (*triStrip[j].patchList[patchCount-1])[last-2].y(),0));
                gldraw1.append(QVector3D((*triStrip[j].patchList[patchCount-1])[last-1].x(), (*triStrip[j].patchList[patchCount-1])[last-1].y(),0));

                gldraw1.draw(gl, mvp, color, GL_TRIANGLE_STRIP, 1.0f);
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
        SettingsManager::instance()->setVehicle_toolWidth(width);

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
    double defaultSectionWidth = SettingsManager::instance()->tool_sectionWidthMulti();
    double offset = SettingsManager::instance()->vehicle_toolOffset();
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
    SettingsManager::instance()->setVehicle_toolWidth(width);

    //left and right tool position
    farLeftPosition = section[0].positionLeft;
    farRightPosition = section[numOfSections - 1].positionRight;

    //find the right side pixel position
    rpXPosition = 250 + (int)(glm::roundMidAwayFromZero(farLeftPosition * 10));
    rpWidth = (int)(glm::roundMidAwayFromZero(width * 10));

}


void CTool::sectionSetPositions()
{
    double vehicle_toolOffset = SettingsManager::instance()->vehicle_toolOffset();
    double section_position1 = SettingsManager::instance()->section_position1();
    double section_position2 = SettingsManager::instance()->section_position2();
    double section_position3 = SettingsManager::instance()->section_position3();
    double section_position4 = SettingsManager::instance()->section_position4();
    double section_position5 = SettingsManager::instance()->section_position5();
    double section_position6 = SettingsManager::instance()->section_position6();
    double section_position7 = SettingsManager::instance()->section_position7();
    double section_position8 = SettingsManager::instance()->section_position8();
    double section_position9 = SettingsManager::instance()->section_position9();
    double section_position10 = SettingsManager::instance()->section_position10();
    double section_position11 = SettingsManager::instance()->section_position11();
    double section_position12 = SettingsManager::instance()->section_position12();
    double section_position13 = SettingsManager::instance()->section_position13();
    double section_position14 = SettingsManager::instance()->section_position14();
    double section_position15 = SettingsManager::instance()->section_position15();
    double section_position16 = SettingsManager::instance()->section_position16();
    double section_position17 = SettingsManager::instance()->section_position17();

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
