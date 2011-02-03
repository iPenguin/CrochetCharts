/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitch.h"

#include <QPainter>
#include <QPixmap>
#include <QtSvg/QSvgRenderer>

#include <QDebug>

Stitch::Stitch()
{
    mSvgRenderer = new QSvgRenderer();
    mPixmap = 0;
}

Stitch::~Stitch()
{
    delete mSvgRenderer;
    mSvgRenderer = 0;
    delete mPixmap;
    mPixmap = 0;
}

void Stitch::setFile ( QString f )
{
    if(mFile != f) {
        mFile = f;
        //FIXME: don't hard code the path to the files!
        if(!mSvgRenderer->isValid())
            mSvgRenderer->load("/home/brian/crochet.git/" + mFile);
    }
    
}


bool Stitch::isSvg()
{
    QString fileName = mFile.toLower();

    if(fileName.endsWith(".svg") || fileName.endsWith(".svgz"))
        return true;
    else
        return false;
}

QPixmap* Stitch::renderPixmap()
{

    if(mPixmap && !mPixmap->isNull())
        return mPixmap;

    mPixmap = new QPixmap(mFile);

    return mPixmap;
}

QSvgRenderer* Stitch::renderSvg()
{
    if(!this->isSvg())
        return new QSvgRenderer();
//FIXME: don't hard code the path to the files!
    if(!mSvgRenderer->isValid())
        mSvgRenderer->load("/home/brian/crochet.git/" + mFile);

    return mSvgRenderer;
}

QDataStream& operator<<(QDataStream &out, const Stitch &s)
{
    out << s.name() << s.file() << s.description() << s.category() << s.wrongSide();
    return out;
}

Stitch& operator<<(Stitch &out, const Stitch &s)
{
    out.setName(s.name());
    out.setFile(s.file());
    out.setDescription(s.description());
    out.setCategory(s.category());
    out.setWrongSide(s.wrongSide());
    return out;
}

QDataStream& operator>>(QDataStream &in, Stitch &s)
{
    QString name, file, desc, cat, ws;
    in >> name >> file >> desc >> cat >> ws;
    s.setName(name);
    s.setFile(file);
    s.setDescription(desc);
    s.setCategory(cat);
    s.setWrongSide(ws);
    return in;
}
