    /*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "savethread.h"

#include "crochetcell.h"
#include "stitch.h"

#include <QObject>
#include <QString>

#include "scene.h"

SaveThread::SaveThread(CrochetTab* t, QXmlStreamReader* s)
{
    tab = t;
    stream = s;
}

SaveThread::~SaveThread()
{
}

void SaveThread::run()
{

    CrochetCell* c = new CrochetCell();
    Stitch* s = 0;
    int row = -1, column = -1;
    int group = -1;
    QString color;
    qreal x = 0, y = 0,
          pivotPtX = 0, pivotPtY = 0;
    qreal angle = 0.0, scale = 0.0;

    while(!(stream->isEndElement() && stream->name() == "cell")) {
        stream->readNext();
        QString tag = stream->name().toString();
        
        if(tag == "stitch") {
            QString st = stream->readElementText();
            s = StitchLibrary::inst()->findStitch(st);
        } else if(tag == "row") {
            row = stream->readElementText().toInt();
        } else if(tag == "column") {
            column = stream->readElementText().toInt();
        } else if(tag == "color") {
            color = stream->readElementText();
        } else if(tag == "x") {
            x = stream->readElementText().toDouble();
        } else if(tag == "y") {
            y = stream->readElementText().toDouble();
        } else if(tag == "angle") {
            angle = stream->readElementText().toDouble();
        } else if(tag == "scale") {
            scale = stream->readElementText().toDouble();
        } else if(tag == "pivotPtX") {
            pivotPtX = stream->readElementText().toDouble();
        } else if(tag == "pivotPtY") {
            pivotPtY = stream->readElementText().toDouble();
        } else if(tag == "group") {
            group = stream->readElementText().toInt();
        }
    }

    tab->scene()->addItem(c);
    
    if(row > -1 && column > -1) {
        c->setStitch(s, (row % 2));
        tab->scene()->grid[row].replace(column, c);
        c->setObjectName(QString::number(row) + ", " + QString::number(column));
    } else {
        c->setStitch(s);
    }

    QPointF pivotPt = QPointF(pivotPtX, pivotPtY);
    
    c->setPos(x, y);
    c->setColor(QColor(color));
    c->setTransformOriginPoint(pivotPt);
    c->setRotation(angle);
    c->setScale(scale, pivotPt);
    if(group != -1)
        tab->scene()->mGroups[group]->addToGroup(c);
}
