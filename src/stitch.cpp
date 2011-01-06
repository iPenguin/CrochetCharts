/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitch.h"

#include <QPainter>
#include <QPixmap>
#include <QtSvg/QSvgRenderer>

#include <QDebug>

Stitch::Stitch(Stitch *parent)
{
    parentItem = parent;
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

QVariant Stitch::data(int column) const
{
    switch(column) {
        case 0:
            return QVariant(mName);
        case 1:
            return QVariant(mFile);
        case 2:
            return QVariant(mDescription);
        case 3:
            return QVariant(mCategory);
        case 4:
            return QVariant(mWrongSide);
        default:
            return QVariant();
    }
}

Stitch* Stitch::child(int row)
{
    return childItems.value(row);
}

int Stitch::childCount() const
{
    return childItems.count();
}

Stitch* Stitch::parent()
{
    return parentItem;
}

int Stitch::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<Stitch*>(this));

    return 0;
}
