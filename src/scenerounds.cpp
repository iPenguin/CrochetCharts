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
        setCellPosition(row, i, c, columns);
    }
    grid.insert(row, modelRow);
}

void SceneRounds::setCellPosition(int row, int column, CrochetCell* c, int columns)
{
    double widthInDegrees = 360.0 / columns;

    double radius = defaultSize().height() * (row + 1) + (defaultSize().height() * 0.5);

    double degrees = widthInDegrees * column;
    QPointF finish = calcPoint(radius, degrees, QPointF(0,0));

    qreal delta = defaultSize().width() * 0.5;
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
    
    if(row < grid.count()) {

        QList<CrochetCell*> r;
        grid.append(r);
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

    qreal degreesPerPos = 360.0 / grid[row].count();

    return ceil(angle / degreesPerPos);
}

QPointF SceneRounds::calcPoint(double radius, double angleInDegrees, QPointF origin)
{
    // Convert from degrees to radians via multiplication by PI/180
    qreal x = (radius * cos(angleInDegrees * M_PI / 180)) + origin.x();
    qreal y = (radius * sin(angleInDegrees * M_PI / 180)) + origin.y();
    return QPointF(x, y);
}
