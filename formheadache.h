#ifndef FORMHEADACHE_H
#define FORMHEADACHE_H

#include <QObject>
#include <QVector>
#include "vec3.h"
#include "classes/headachedesigner.h"

class CBoundary;
class CHeadLine;
class CTool;
class CVehicle;
class QOpenGLFunctions;


class FormGPS; // Forward declaration

class FormHeadache : public QObject
{
    Q_OBJECT
protected:
    FormGPS* formGPS = nullptr; // Reference to parent FormGPS
    //InterfaceProperty<HeadacheDesigner,double> maxFieldDistance = InterfaceProperty<HeadacheDesigner,double>("maxFieldDistance");
    //InterfaceProperty<HeadacheDesigner,double> fieldCenterX = InterfaceProperty<HeadacheDesigner,double>("fieldCenterX");
    //InterfaceProperty<HeadacheDesigner,double> fieldCenterY = InterfaceProperty<HeadacheDesigner,double>("fieldCenterY");

    double maxFieldDistance = 0;
    double fieldCenterX = 0;
    double fieldCenterY = 0;


    // ⚡ PHASE 6.3.0: DUPLICATION REMOVED - isYouTurnBtnOn only exists in cyouturn.h (main version)
    // Access via: formGPS->isYouTurnBtnOn

    int fixX, fixY;
    //InterfaceProperty<HeadacheDesigner,bool> isA = InterfaceProperty<HeadacheDesigner,bool>("isA");
    bool isA = true;
    bool draw = false;
    int start = 99999, end = 99999;
    int bndSelect = 0, mode;
    QVector<Vec3> sliceArr;
    QVector<Vec3> backupList;


    bool zoomToggle;

    Vec3 pint = Vec3(0,1,0);
    bool isLinesVisible = true;

    QVector3D mouseClickToField(int mouseX, int mouseY);

    void setup_matrices(QMatrix4x4 &modelview, QMatrix4x4 &projection);

public:
    double iE = 0, iN = 0;
    QVector<int> crossings;

    CBoundary *bnd;
    CHeadLine *hdl;
    CTool *tool;
    QObject *headache_designer_instance;
    QObject *mainWindow;

    // Phase 6.0.4.3 - Native Q_PROPERTY designer
    HeadacheDesigner *designer;

    explicit FormHeadache(QObject *parent = nullptr);
    void setFormGPS(FormGPS* gps) { formGPS = gps; }

    //this class is pretty closely coupled to the QML file
    //of necessity
    void connect_ui(QObject *headache_designer_instance);

public slots:
    void setFieldInfo(double maxFieldDistance,
                      double fieldCenterX,
                      double fieldCenterY);

    void load_headline();
    void update_lines(); //update the boundary lines in GUI
    void update_headland(); //update headland line
    void update_headlines();
    void update_ab(); //update slice and A B points in GUI

    void clicked(int mouseX, int mouseY);

    void btnCycleForward_Click();
    void btnCycleBackward_Click();
    void btnDeleteCurve_Click();

    void FormHeadLine_FormClosing();
    void btnExit_Click();
    void isSectionControlled(bool wellIsIt);
    void btnBndLoop_Click();
    void btnSlice_Click();
    void btnALength_Click();
    void btnBLength_Click();
    void btnBShrink_Click();
    void btnAShrink_Click();
    void btnHeadlandOff_Click();
    void btnCancelTouch_Click();
    void btnDeleteHeadland_Click();

signals:
    void timedMessageBox(int, QString, QString);
    void saveHeadlines();
    void loadHeadlines();
    void saveHeadland();
};

#endif // FORMHEADLAND_H
