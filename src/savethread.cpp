    /*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "savethread.h"

#include "cell.h"
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

    Cell* c = new Cell();
    Stitch* s = 0;
    int row = -1, column = -1;
    int group = -1;
    QString color;
    QPointF position(0.0,0.0);
    QPointF pivotPoint;
    qreal angle = 0.0;
    QPointF scale = QPointF(1.0,1.0);
    QTransform transform;
    qreal   m11 = 1, m12 = 0, m13 = 0,
            m21 = 0, m22 = 1, m23 = 0,
            m31 = 0, m32 = 0, m33 = 1;

    while(!(stream->isEndElement() && stream->name() == "cell")) {
        stream->readNext();
        QString tag = stream->name().toString();

        if(tag == "stitch") {
            QString st = stream->readElementText();
            s = StitchLibrary::inst()->findStitch(st);

        } else if(tag == "grid") {
            row = stream->attributes().value("row").toString().toDouble();
            column = stream->attributes().value("column").toString().toDouble();
            stream->readElementText();
        } else if(tag == "color") {
            color = stream->readElementText();

        } else if(tag == "position") {
            position.rx() = stream->attributes().value("x").toString().toDouble();
            position.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "angle") {
            angle = stream->readElementText().toDouble();

        } else if(tag == "scale") {
            scale.rx() = stream->attributes().value("x").toString().toDouble();
            scale.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "pivotPoint") {
            pivotPoint.rx() = stream->attributes().value("x").toString().toDouble();
            pivotPoint.ry() = stream->attributes().value("y").toString().toDouble();
            stream->readElementText();

        } else if(tag == "group") {
            group = stream->readElementText().toInt();

        } else if(tag == "transformation") {
            m11 = stream->attributes().value("m11").toString().toDouble();
            m12 = stream->attributes().value("m12").toString().toDouble();
            m13 = stream->attributes().value("m13").toString().toDouble();
            m21 = stream->attributes().value("m21").toString().toDouble();
            m22 = stream->attributes().value("m22").toString().toDouble();
            m23 = stream->attributes().value("m23").toString().toDouble();
            m31 = stream->attributes().value("m31").toString().toDouble();
            m32 = stream->attributes().value("m32").toString().toDouble();
            m33 = stream->attributes().value("m33").toString().toDouble();
            transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
            stream->readElementText();
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

    c->setTransform(transform);
    c->setPos(position);
    c->setColor(QColor(color));
    c->setTransformOriginPoint(pivotPoint);
    c->setRotation(angle);
    
    if(group != -1)
        tab->scene()->mGroups[group]->addToGroup(c);
}
