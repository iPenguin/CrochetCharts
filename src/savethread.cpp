    /*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "savethread.h"

#include "crochetcell.h"
#include "stitch.h"

#include <QObject>

#include <QString>
#include <QTransform>

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

    CrochetCell *c = new CrochetCell();
    Stitch *s = 0;
    int row = -1, column = -1;
    QString color;
    qreal x = 0, y = 0, anchorX = 0, anchorY = 0;
    QTransform transform;
    qreal angle = 0.0, scale = 0.0;
    
    QObject::connect(c, SIGNAL(stitchChanged(QString,QString)), tab->scene(), SIGNAL(stitchChanged(QString,QString)));
    QObject::connect(c, SIGNAL(colorChanged(QString,QString)), tab->scene(), SIGNAL(colorChanged(QString,QString)));

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
        } else if(tag == "anchor_x") {
            anchorX = stream->readElementText().toDouble();
        } else if(tag == "anchor_y") {
            anchorY = stream->readElementText().toDouble();
        } else if(tag == "transformation") {
            transform = loadTransform(stream);
        } else if(tag == "angle") {
            angle = stream->readElementText().toDouble();
        } else if(tag == "scale") {
            scale = stream->readElementText().toDouble();
        }
    }
    
    if(row > -1 && column > -1) {
        c->setStitch(s, (row % 2));
        tab->scene()->grid[row].replace(column, c);
        c->setObjectName(QString::number(row) + ", " + QString::number(column));
    } else {
        c->setStitch(s);
    }
    
    tab->scene()->addCell(c);
    
    c->setAnchor(anchorX, anchorY);
    c->setPos(x, y);
    c->setTransform(transform);
    c->setColor(QColor(color));
    c->mAngle = angle;
    c->mScale = scale;
}

QTransform SaveThread::loadTransform(QXmlStreamReader* stream)
{
    QTransform transform;
    
    qreal m11 = transform.m11(), m12 = transform.m12(), m13 = transform.m13(),
          m21 = transform.m21(), m22 = transform.m22(), m23 = transform.m23(),
          m31 = transform.m31(), m32 = transform.m32(), m33 = transform.m33();
    
    while(!(stream->isEndElement() && stream->name() == "transformation")) {
        stream->readNext();
        QString tag = stream->name().toString();
        
        if(tag == "m11") {
            m11 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m12") {
            m12 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m13") {
            m13 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m21") {
            m21 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m22") {
            m22 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m23") {
            m23 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m31") {
            m31 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m32") {
            m32 = (qreal)stream->readElementText().toDouble();
        } else if(tag == "m33") {
            m33 = (qreal)stream->readElementText().toDouble();
        }
    }
    
    transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
    return transform;
}
