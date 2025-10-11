#include "cfielddata.h"
#include "cvehicle.h"
#include "qmlutil.h"
#include "formgps.h"

CFieldData::CFieldData() {}

void CFieldData::UpdateFieldBoundaryGUIAreas(QVector<CBoundaryList> &bndList, QObject *mainWindow, FormGPS *formGPS)
{
    if (bndList.count() > 0)
    {
        formGPS->setAreaOuterBoundary(bndList[0].area);
        formGPS->setAreaBoundaryOuterLessInner((double)bndList[0].area);

        for (int i = 1; i < bndList.count(); i++)
        {
            double currentValue = formGPS->areaBoundaryOuterLessInner();
            formGPS->setAreaBoundaryOuterLessInner(currentValue - bndList[i].area);
        }
    }
    else
    {
        formGPS->setAreaOuterBoundary(0.0);
        formGPS->setAreaBoundaryOuterLessInner(0.0);
    }
}
