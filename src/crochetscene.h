/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETSCENE_H
#define CROCHETSCENE_H

#include <QGraphicsScene>

#include "crochetdatamodel.h"
#include "crochetcell.h"

class QXmlStreamReader;
class QXmlStreamWriter;

class CrochetScene : public QGraphicsScene
{
    Q_OBJECT
public:
    CrochetScene(QObject *parent = 0);
/*
    void setCell(int row, int column, Stitch *s);
    void insertCell(int row, int columnBefore, Stitch *s);

*/
    void createRow(int row, int columns);

    void load(QXmlStreamReader* stream);
    void save(QXmlStreamWriter* stream);

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

    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    
private:
    QPointF calcPoint(double radius, double angleInDegrees, QPointF origin);

    void createCell(QDomElement* element);
    
    void initDemoBackground();

    int mStitchWidth;

    //Used in the mouse*Event()s to keep the mouse movements on the same cell.
    CrochetCell *mCurCell;
    CrochetDataModel *mModel;
};

#endif //CROCHETSCENE_H
