/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETSCENE_H
#define CROCHETSCENE_H

#include <QGraphicsScene>

#include "crochetcell.h"

class CrochetScene : public QGraphicsScene
{
    Q_OBJECT
    friend class SaveFile;
public:
    CrochetScene(QObject *parent = 0);
/*
    void setCell(int row, int column, Stitch *s);
    void insertCell(int row, int columnBefore, Stitch *s);

*/
    void createRow(int row, int columns);

    void appendCell(int row, Cell *c);
    void insertCell(int row, int colBefore, Cell *c);

    int rowCount();
    int columnCount(int row);

    Cell* cell(int row, int column);

    void removeCell(int row, int column);
    void createChart(int rows, int cols);
    
signals:
    void rowChanged(int row);
    void rowsChanged(int startRow);
    void rowAdded(int newRow);
    void chartCreated(int rows, int cols);
    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);
    
protected:
/*
    virtual void    contextMenuEvent ( QGraphicsSceneContextMenuEvent * contextMenuEvent )
    virtual void    helpEvent ( QGraphicsSceneHelpEvent * helpEvent )
    virtual void    keyPressEvent ( QKeyEvent * keyEvent )
    virtual void    keyReleaseEvent ( QKeyEvent * keyEvent )
    virtual void    mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent )
    virtual void    wheelEvent ( QGraphicsSceneWheelEvent * wheelEvent )
*/
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
/*
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
*/
    
private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);
    
    void initDemoBackground();

    int mStitchWidth;

    //Used in the mouse*Event()s to keep the mouse movements on the same cell.
    CrochetCell *mCurCell;

    QList<QList<Cell *> > mGrid;
};

#endif //CROCHETSCENE_H
