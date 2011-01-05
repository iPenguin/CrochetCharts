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

    if(!mSvgRenderer->isValid())
        mSvgRenderer->load(mFile);

    return mSvgRenderer;
}
