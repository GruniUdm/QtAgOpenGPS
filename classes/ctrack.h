#ifndef CTRACK_H
#define CTRACK_H

#include <QObject>
#include <QVector>
#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>
#include <QQmlEngine>
#include <QJSEngine>
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
    CTrk(const CTrk &orig);
};

class CTrack : public QAbstractListModel
{
    Q_OBJECT
    // QML registration handled manually in main.cpp

public:
    // C++ singleton access (strict singleton pattern)
    static CTrack* instance() {
        static CTrack* s_instance = new CTrack(nullptr);
        return s_instance;
    }

    // ===== QML PROPERTIES =====
    Q_PROPERTY(int idx MEMBER idx NOTIFY idxChanged)
    Q_PROPERTY(QObject* model READ getModel CONSTANT)
    Q_PROPERTY(int newRefSide READ getNewRefSide WRITE setNewRefSide NOTIFY newRefSideChanged)
    Q_PROPERTY(bool isAutoTrack MEMBER isAutoTrack NOTIFY isAutoTrackChanged)
    Q_PROPERTY(bool isAutoSnapToPivot MEMBER isAutoSnapToPivot NOTIFY isAutoSnapToPivotChanged)
    Q_PROPERTY(bool isAutoSnapped MEMBER isAutoSnapped NOTIFY isAutoSnappedChanged)
    Q_PROPERTY(int howManyPathsAway READ getHowManyPathsAway NOTIFY howManyPathsAwayChanged)
    Q_PROPERTY(int mode READ getMode NOTIFY modeChanged)
    Q_PROPERTY(int newMode READ getNewMode NOTIFY newModeChanged)
    Q_PROPERTY(QString newName READ getNewName WRITE setNewName NOTIFY newNameChanged)
    Q_PROPERTY(double newHeading READ getNewHeading WRITE setNewHeading NOTIFY newHeadingChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged())
    Q_PROPERTY(QString currentName READ getCurrentName NOTIFY currentNameChanged)

    // Membres données principales (publics pour accès externe via singleton)
    QVector<CTrk> gArr;
    CABCurve curve;
    CABLine ABLine;
    CTrk newTrack;
    int idx, autoTrack3SecTimer;

    // Membres et méthodes publiques nécessaires pour accès externe
    bool isLine, isAutoTrack = false, isAutoSnapToPivot = false, isAutoSnapped;
    QVector<Vec2> designRefLine;

    // CTrack interface (publiques pour accès via singleton)
    int FindClosestRefTrack(Vec3 pivot, const CVehicle &vehicle);
    void SwitchToClosestRefTrack(Vec3 pivot, const CVehicle &vehicle);
    void BuildCurrentLine(Vec3 pivot,
                          double secondsSinceStart, bool isBtnAutoSteerOn,
                          CYouTurn &yt,
                          CVehicle &vehicle,
                          const CBoundary &bnd,
                          const CAHRS &ahrs,
                          CGuidance &gyd,
                          CNMEA &pn);
    void ResetCurveLine();
    void AddPathPoint(Vec3 point);
    void DrawTrackNew(QOpenGLFunctions *gl, const QMatrix4x4 &mvp, const CCamera &camera, const CVehicle &vehicle);
    void DrawTrack(QOpenGLFunctions *gl, const QMatrix4x4 &mvp,
                   bool isFontOn,
                   bool isRateMapOn,
                   CYouTurn &yt, const CCamera &camera,
                   const CGuidance &gyd);
    void DrawTrackGoalPoint(QOpenGLFunctions *gl, const QMatrix4x4 &mvp);
    int getHowManyPathsAway();
    int getMode() { if (idx >=0) return gArr[idx].mode; else return 0; }

    void reloadModel() {
        //force QML to reload the model to reflect changes
        //that may have been made in C++ code.
        beginResetModel();
        endResetModel();
        emit modelChanged(); //not sure if this is necessary
    }


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
    int newRefSide = 0;

    ~CTrack();

    void NudgeRefABLine(CTrk &track, double dist);
    void NudgeRefCurve(CTrk &track, double distAway);
    void NudgeTrack(double dist);
    void NudgeDistanceReset();
    void SnapToPivot();
    void NudgeRefTrack(double dist);

    //getters and setters for properties
    QString getNewName(void);
    void setNewName(QString new_name);

    int getNewMode(void);
    void setNewMode(TrackMode);

    int getNewRefSide(void);
    void setNewRefSide(int which_side);

    double getNewHeading(void);
    void setNewHeading(double new_heading);

    QString getCurrentName(void);

    void setIdx(int new_idx);
    SETTER(bool, isAutoTrack, setIsAutoTrack)
    SETTER(bool, isAutoSnapToPivot, setIsAutoSnapToPivot)
    SETTER(bool, isAutoSnapped, setIsAutoSnapped)
    QObject *getModel() { return this;}

    Q_INVOKABLE QString getTrackName(int index);
    Q_INVOKABLE bool getTrackVisible(int index);
    Q_INVOKABLE double getTrackNudge(int index);

    // QML model interface
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

    void update_ab_refline();

protected:
    // QML model interface
    virtual QHash<int, QByteArray> roleNames() const override;
signals:
    void resetCreatedYouTurn();
    void saveTracks();

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
    void newHeadingChanged();
    void countChanged();
    void currentNameChanged();

public slots:
    //slots are by definition invoke-able from QML directly.
    //or they can be connected to qml-emitted signals
    void select(int index);
    void next();
    void prev();

    void start_new(int mode);
    void mark_start(double easting, double northing, double heading);
    void mark_end(int refSide, double easting,
                  double northing);

    void finish_new(QString name);

    void cancel_new();
    void pause(bool pause);
    void add_point(double easting, double northing, double heading);

    void delete_track(int index);
    void changeName(int index, QString new_name);
    void swapAB(int index);
    void setVisible(int index, bool isVisible);
    void copy(int index, QString new_name);

    void ref_nudge(double dist_m);
    void nudge_zero();
    void nudge_center();
    void nudge(double dist_m);

private:
    // Used by QML model interface
    QHash<int, QByteArray> m_roleNames;

private:
    // Constructeur privé pour pattern singleton strict
    explicit CTrack(QObject* parent = nullptr);
};

#endif // CTRACK_H
