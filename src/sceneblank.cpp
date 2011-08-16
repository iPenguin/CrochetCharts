/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "sceneblank.h"

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
#include "indicatorundo.h"

#include <QKeyEvent>
#include "stitchlibrary.h"

SceneBlank::SceneBlank(QObject *parent)
    : Scene(parent)
{
}

SceneBlank::~SceneBlank()
{

}

CrochetCell* SceneBlank::cell(int row, int column)
{
    Q_ASSERT(mGrid.count() > row);
    if(mGrid[row].count() <= column)
        return 0;

    return mGrid[row][column];
}

CrochetCell* SceneBlank::cell(QPoint position)
{
    return cell(position.y(), position.x());
}

void SceneBlank::removeCell(CrochetCell *c)
{
    removeItem(c);
}

int SceneBlank::rowCount()
{
    return 1;
}

int SceneBlank::columnCount(int row)
{
    return items().count();
}

void SceneBlank::appendCell(int row, CrochetCell *c, bool fromSave)
{
    Q_UNUSED(fromSave);
    //append any missing rows.

            
            addCell(QPoint(mGrid[row].count(), row), c);

    int col = mGrid[row].count() -1;
    setCellPosition(row, col, c, mGrid[row].count());
    c->setColor(QColor(Qt::white));
   
}

void SceneBlank::addCell(QPoint p, CrochetCell* c)
{

    //TODO: simplify the connect() statements...
    addItem(c);

    c->setPos(p.x(), p.y());

    connect(c, SIGNAL(stitchChanged(QString,QString)), this, SIGNAL(stitchChanged(QString,QString)));
    connect(c, SIGNAL(colorChanged(QString,QString)), this, SIGNAL(colorChanged(QString,QString)));
    connect(c, SIGNAL(stitchChanged(QString,QString)), this, SLOT(stitchUpdated(QString,QString)));

}

void SceneBlank::createChart(int rows, int cols, QString stitch, QSizeF rowSize)
{

    initDemoBackground();
}

void SceneBlank::createRow(int row, int columns, QString stitch)
{
    Q_UNUSED(row);
    Q_UNUSED(columns);
    Q_UNUSED(stitch);
}

void SceneBlank::stitchUpdated(QString oldSt, QString newSt)
{
    Q_UNUSED(newSt);
    
    if(oldSt.isEmpty() || oldSt.isNull())
        return;
    
    CrochetCell *c = qobject_cast<CrochetCell*>(sender());
    if(!c)
        return;

    QPoint pos = findGridPosition(c);
    
}

QPoint SceneBlank::findGridPosition(CrochetCell* c)
{
    for(int y = 0; y < mGrid.count(); ++y) {
        if(mGrid[y].contains(c)) {
            return QPoint(mGrid[y].indexOf(c), y);
        }
    }
    
    return QPoint();
}

void SceneBlank::setCellPosition(int row, int column, CrochetCell* c, int columns, bool updateAnchor)
{
    c->setPos(row, column);
}

void SceneBlank::keyReleaseEvent(QKeyEvent* keyEvent)
{
    Scene::keyReleaseEvent(keyEvent);
}

void SceneBlank::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    Scene::mousePressEvent(e);
    
    switch(mMode) {
        case SceneBlank::AngleMode:
            angleModeMousePress(e);
            break;
        default:
            break;
    }

}

void SceneBlank::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{

    switch(mMode) {
        case SceneBlank::StitchMode:
            stitchModeMouseMove(e);
            break;
        case SceneBlank::AngleMode:
            angleModeMouseMove(e);
            break;
        case SceneBlank::StretchMode:
            stretchModeMouseMove(e);
            break;
        default:
            break;
    }
    
}

void SceneBlank::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
 
    switch(mMode) {
        case SceneBlank::StitchMode:
            stitchModeMouseRelease(e);
            break;
        case SceneBlank::AngleMode:
            angleModeMouseRelease(e);
            break;
        default:
            break;
    }

    Scene::mouseReleaseEvent(e);
}

void SceneBlank::stitchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void SceneBlank::stitchModeMouseRelease(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}

void SceneBlank::angleModeMousePress(QGraphicsSceneMouseEvent *e)
{
   Q_UNUSED(e);
}

void SceneBlank::angleModeMouseMove(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
}

void SceneBlank::angleModeMouseRelease(QGraphicsSceneMouseEvent *e)
{
    Q_UNUSED(e);
}

void SceneBlank::stretchModeMouseMove(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
}
