/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "scenerows.h"

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

SceneRows::SceneRows(QObject* parent)
    : Scene(parent)
{
}

SceneRows::~SceneRows()
{

}

void SceneRows::createChart(int rows, int cols, QString stitch, QSizeF rowSize)
{
    defaultSize() = rowSize;

    for(int i = 0; i < rows; ++i)
        createRow(i, cols, stitch);

    initDemoBackground();
}

void SceneRows::createRow(int row, int columns, QString stitch)
{
    CrochetCell* c = 0;
    
    QList<CrochetCell*> modelRow;
    for(int i = 0; i < columns; ++i) {
        c = new CrochetCell();
        addCell(c);
        c->setStitch(stitch, (row % 2));
        modelRow.append(c);

        setCellPosition(row, i, c, columns);
    }
    grid.insert(row, modelRow);

}

void SceneRows::setCellPosition(int row, int column, CrochetCell* c, int columns)
{
    //FIXME: use the columns and add rows.
    Q_UNUSED(columns);

    c->setPos(column * defaultSize().width() + column * 5, row * defaultSize().height());

    //FIXME: set tooltips from bottom right to top left.
    c->setToolTip(tr("Row: %1, St: %2").arg(row+1).arg(column+1));
}
