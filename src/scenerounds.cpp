/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "scenerounds.h"

#include "crochetcell.h"

#include <QFontMetrics>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneEvent>
#include <QApplication> //start drag min distance.

#include <math.h>

#include <QDebug>

#include "chartview.h"

#include "settings.h"
#include "stitchset.h"
#include "appinfo.h"
#include "crochetchartcommands.h"

#include <QKeyEvent>
#include "stitchlibrary.h"

SceneRounds::SceneRounds(QObject* parent)
    : Scene(parent),
    mCenterSymbol(0)
{
}

SceneRounds::~SceneRounds()
{
}

void SceneRounds::setShowChartCenter(bool state)
{
    mShowChartCenter = state;

    if(mShowChartCenter) {
        if(!mCenterSymbol) {
            QPen pen;
            pen.setWidth(5);

            double radius = (defaultSize().height() * 0.45);

            mCenterSymbol = addEllipse(-radius, -radius, radius * 2, radius * 2, pen);
            mCenterSymbol->setToolTip(tr("Chart Center"));
        } else {
            addItem(mCenterSymbol);
        }
    } else {
        removeItem(mCenterSymbol);
    }

}

void SceneRounds::createChart(int rows, int cols, QString stitch, QSizeF rowSize)
{
 
    defaultSize() = rowSize;

    for(int i = 0; i < rows; ++i) {
        //FIXME: this padding should be dependant on the height of the sts.
        int pad = i * 12;

        createRow(i, cols + pad, stitch);
    }

    setShowChartCenter(Settings::inst()->value("showChartCenter").toBool());
    
    initDemoBackground();
}

void SceneRounds::createRow(int row, int columns, QString stitch)
{
    CrochetCell* c = 0;
    
    QList<CrochetCell*> modelRow;
    for(int i = 0; i < columns; ++i) {
        c = new CrochetCell();
        connect(c, SIGNAL(stitchChanged(QString,QString)), this, SIGNAL(stitchChanged(QString,QString)));
        connect(c, SIGNAL(colorChanged(QString,QString)), this, SIGNAL(colorChanged(QString,QString)));
        
        c->setStitch(stitch, (row % 2));
        addItem(c);
        modelRow.append(c);
        setCellPosition(row, i, c, columns, true);
    }
    rows.insert(row, modelRow);
}

void SceneRounds::setCellPosition(int row, int column, CrochetCell* c, int columns, bool updateAnchor)
{
    double widthInDegrees = 360.0 / columns;

    double radius = defaultSize().height() * (row + 1) + (defaultSize().height() * 0.5);

    double degrees = widthInDegrees * column;
    QPointF finish = calcPoint(radius, degrees, QPointF(0,0));

    qreal delta = defaultSize().width() * 0.5;
    if(updateAnchor || c->anchor().isNull())
        c->setAnchor(finish.x() - delta, finish.y());
    c->setPos(finish.x() - delta, finish.y());
    c->setTransform(QTransform().translate(delta,0).rotate(degrees + 90).translate(-delta, 0));
    c->setAngle(degrees + 90);
    c->setToolTip(tr("Row: %1, St: %2").arg(row+1).arg(column+1));
}

int SceneRounds::getClosestRow(QPointF mousePosition)
{
    //double radius = defaultSize().height() * (row + 1) + (defaultSize().height() * 0.5);
    qreal radius = sqrt(mousePosition.x() * mousePosition.x() + mousePosition.y() * mousePosition.y());

    qreal temp = radius - (defaultSize().height() * 0.5);
    qreal temp2 = temp / defaultSize().height();
    
    int row = round(temp2 - 1);
    if(row < 0)
        row = 0;
    
    if(row < rows.count()) {

        QList<CrochetCell*> r;
        rows.append(r);
    }

    return row;
}

int SceneRounds::getClosestColumn(QPointF mousePosition, int row)
{
    /*
              |
          -,- | +,-
        ------+------
          -,+ | +,+
              |
    */
    qreal tanx = mousePosition.y() / mousePosition.x();
    qreal rads = atan(tanx);
    qreal angleX = rads * 180 / M_PI;
    qreal angle = 0.0;
    if (mousePosition.x() >= 0 && mousePosition.y() >= 0)
        angle = angleX;
    else if(mousePosition.x() <= 0 && mousePosition.y() >= 0)
        angle = 180 + angleX;
    else if(mousePosition.x() <= 0 && mousePosition.y() <= 0)
        angle = 180 + angleX;
    else if(mousePosition.x() >= 0 && mousePosition.y() <= 0)
        angle = 360 + angleX;

    qreal degreesPerPos = 360.0 / rows[row].count();

    return ceil(angle / degreesPerPos);
}

QPointF SceneRounds::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    qreal x = (radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    qreal y = (radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
    return QPointF(x, y);
}

void SceneRounds::stitchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    if(e->buttons() != Qt::LeftButton)
        return;
    
    if(!mCurCell)
        return;

    //FIXME: if you're not draging a stitch you should be able to drag and paint.
}

void SceneRounds::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    
    //FIXME: foreach(stitch in selection()) create an undo group event.
    if(mCurCell) {
        
        if(mCurCell->name() != mEditStitch && !mMoving)
            undoStack()->push(new SetCellStitch(this, mCurCell, mEditStitch));
    
        mCurCell = 0;
    } else if(!mRubberBand && !mMoving){
        //FIXME: combine getClosestRow & getClosestColumn into 1 function returning a QPoint.
        int y = getClosestRow(e->scenePos());
        //FIXME: the row has to be passed in because getClosestRow modifies the row
        int x = getClosestColumn(e->scenePos(), y);

        if(e->button() == Qt::LeftButton && !(e->modifiers() & Qt::ControlModifier)) {
//FIXME: the xy should be pos not grid.
            AddCell* addCell = new AddCell(this, QPointF(x, y));
            CrochetCell* c = addCell->cell();
            c->setStitch(mEditStitch, (y % 2));
            undoStack()->push(addCell);

        } else {
            if(!mCurCell)
                return;

            undoStack()->push(new RemoveCell(this, mCurCell));
            mCurCell = 0;
        }
    }
}
