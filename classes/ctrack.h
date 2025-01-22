#ifndef CTRACK_H
#define CTRACK_H

#include <QVector>
#include <QAbstractListModel>
#include "vec3.h"
#include "vec2.h"
#include "setter.h"
#include "cabcurve.h"
#include "cabline.h"

class QOpenGLFunctions;
class CVehicle;
class CABLine;
class CABCurve;
class CBoundary;
class CYouTurn;
class CAHRS;
class CGuidance;
class CNMEA;
class CCamera;
class CTram;


enum TrackMode {
    None = 0,
    AB = 2,
    Curve = 4,
    bndTrackOuter = 8,
    bndTrackInner = 16,
    bndCurve = 32,
    waterPivot = 64
};//, Heading, Circle, Spiral

class CTrk
{
public:
    QVector<Vec3> curvePts;
    double heading;
    QString name;
    bool isVisible;
    Vec2 ptA;
    Vec2 ptB;
    Vec2 endPtA;
    Vec2 endPtB;
    int mode;
    double nudgeDistance;

    CTrk();
};

class CTrack : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RoleNames {
        index = Qt::UserRole,
        NameRole,
        IsVisibleRole,
        ModeRole,
        ptA,
        ptB,
        endPtA,
        endPtB,
        nudgeDistance
    };

    QVector<CTrk> gArr;
    CABCurve curve;
    CABLine ABLine;
    CTrk newTrack;
    int newRefSide = 0;

    Q_PROPERTY(int idx MEMBER idx NOTIFY idxChanged)
    //put a pointer to ourselves as a model.  This class is both
    //a list model, and also a bunch of properties
    Q_PROPERTY(QObject* model READ getModel CONSTANT)
    Q_PROPERTY(int newRefSide MEMBER newRefSide NOTIFY newRefSideChanged)

    int idx, autoTrack3SecTimer;

    Q_PROPERTY(bool isAutoTrack MEMBER isAutoTrack NOTIFY isAutoTrackChanged)
    Q_PROPERTY(bool isAutoSnapToPivot MEMBER isAutoSnapToPivot NOTIFY isAutoSnapToPivotChanged)
    Q_PROPERTY(bool isAutoSnapped MEMBER isAutoSnapped NOTIFY isAutoSnappedChanged)
    Q_PROPERTY(int howManyPathsAway READ getHowManyPathsAway NOTIFY howManyPathsAwayChanged)
    Q_PROPERTY(int mode READ getMode NOTIFY modeChanged)

    bool isLine, isAutoTrack = false, isAutoSnapToPivot = false, isAutoSnapped;

    //creating new track
    Q_PROPERTY (int newMode READ getNewMode NOTIFY newModeChanged)
    Q_PROPERTY (QString newName READ getNewName WRITE setNewName NOTIFY newNameChanged)

    Q_PROPERTY (int count READ rowCount NOTIFY countChanged())

    explicit CTrack(QObject* parent = nullptr);

    // CTrack interface
    int FindClosestRefTrack(Vec3 pivot, const CVehicle &vehicle);
    void SwitchToClosestRefTrack(Vec3 pivot, const CVehicle &vehicle);

    void NudgeRefABLine(CTrk &track, double dist);
    void NudgeRefCurve(CTrk &track, double distAway);

    void DrawTrackNew(QOpenGLFunctions *gl, const QMatrix4x4 &mvp, const CCamera &camera);
    void DrawTrack(QOpenGLFunctions *gl, const QMatrix4x4 &mvp,
                   bool isFontOn,
                   CYouTurn &yt, const CCamera &camera
                   , const CGuidance &gyd);
    void DrawTrackGoalPoint(QOpenGLFunctions *gl, const QMatrix4x4 &mvp);

    void BuildCurrentLine(Vec3 pivot,
                          double secondsSinceStart, bool isAutoSteerbtnOn, int &makeUTurnCounter,
                          CYouTurn &yt,
                          CVehicle &vehicle,
                          const CBoundary &bnd,
                          const CAHRS &ahrs,
                          CGuidance &gyd,
                          CNMEA &pn);

    void ResetCurveLine();
    void AddPathPoint(Vec3 point); //add point while building new curve or AB Line


    //getters and setters for properties
    QString getNewName(void);
    void setNewName(QString new_name);

    int getNewMode(void);
    void setNewMode(TrackMode);

    int getHowManyPathsAway();
    int getMode() { if (idx >=0) return gArr[idx].mode; else return 0; }
    void setIdx(int new_idx);
    SETTER(bool, isAutoTrack, setIsAutoTrack)
    SETTER(bool, isAutoSnapToPivot, setIsAutoSnapToPivot)
    SETTER(bool, isAutoSnapped, setIsAutoSnapped)
    QObject *getModel() { return this;}

    // QML model interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

protected:
    // QML model interface
    virtual QHash<int, QByteArray> roleNames() const override;
signals:
    void idxChanged();
    void modeChanged();
    void modelChanged();
    void isAutoSnapToPivotChanged();
    void isAutoSnappedChanged();
    void isAutoTrackChanged();
    void howManyPathsAwayChanged();
    void newModeChanged();
    void newNameChanged();
    void newRefSideChanged();
    void countChanged();

public slots:
    void reloadModel() {
        //force QML to reload the model to reflect changes
        //that may have been made in C++ code.
        beginResetModel();
        endResetModel();
        emit modelChanged(); //not sure if this is necessary
    }
    //put these here so Qt can call them directly.
    void NudgeTrack(double dist);
    void NudgeDistanceReset();
    void SnapToPivot();
    void NudgeRefTrack(double dist);

private:
    // Used by QML model interface
    QHash<int, QByteArray> m_roleNames;
};

#endif // CTRACK_H
