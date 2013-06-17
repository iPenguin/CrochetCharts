/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitch.h"

#include <QPainter>
#include <QPixmap>
#include <QtSvg/QSvgRenderer>

#include "debug.h"
#include <QFile>

#include "settings.h"

Stitch::Stitch(QObject *parent) :
    QObject(parent),
    isBuiltIn(false),
    mIsSvg(false),
    mPixmap(0)
{
}

Stitch::~Stitch()
{
    foreach(QString key, mRenderers.keys())
        mRenderers.value(key)->deleteLater();

    delete mPixmap;
    mPixmap = 0;
}

void Stitch::setFile ( QString f )
{
    if(mFile != f) {
        mFile = f;

        delete mPixmap;
        mPixmap = 0;

        setupSvgFiles();

        if(!isSvg()) {
            mPixmap = new QPixmap(mFile);
        }
    }
}

bool Stitch::setupSvgFiles()
{
    QFile file(mFile);
    if(!file.open(QIODevice::ReadOnly)) {
        WARN("cannot open file for svg setup");
        return false;
    }

    QByteArray data = file.readAll();
    QByteArray priData, secData;

    QString black = "#000000";

    QString pri = Settings::inst()->value("stitchPrimaryColor").toString();
    QString sec = Settings::inst()->value("stitchAlternateColor").toString();

    priData = data;
    secData = data;

    //Don't parse the color if we're using black
    if(pri != black)
        priData = priData.replace(QByteArray(black.toLatin1()), QByteArray(pri.toLatin1()));

    QSvgRenderer *svgR = new QSvgRenderer();
    if(!svgR->load(priData)) {
        mIsSvg = false;
        return false;
    }

    mRenderers.insert(pri, svgR);

    if(sec != black)
        secData = data.replace(QByteArray(black.toLatin1()), QByteArray(sec.toLatin1()));

    svgR = new QSvgRenderer();
    if(!svgR->load(secData)) {
        mIsSvg = false;
        return false;
    }

    mRenderers.insert(sec, svgR);

    mIsSvg = true;
    return true;
}

void Stitch::addStitchColor(QString color)
{

    //don't add colors already in the list.
    if(mRenderers.contains(color))
        return;

    QFile file(mFile);
    if(!file.open(QIODevice::ReadOnly)) {
        WARN("cannot open file for svg setup");
        return;
    }

    QByteArray data = file.readAll();

    QString black = "#000000";

    //Don't parse the color if we're using black
    if(color != black)
        data = data.replace(QByteArray(black.toLatin1()), QByteArray(color.toLatin1()));

    QSvgRenderer *svgR = new QSvgRenderer();
    svgR->load(data);
    mRenderers.insert(color, svgR);
}

bool Stitch::isSvg()
{

    return mIsSvg;
}

QPixmap* Stitch::renderPixmap()
{

    if(mPixmap && !mPixmap->isNull())
        return mPixmap;

    mPixmap = new QPixmap(mFile);

    return mPixmap;
}

QSvgRenderer* Stitch::renderSvg(QColor color)
{

    if(!isSvg())
        return 0;

    if(!mRenderers.contains(color.name())) {
        addStitchColor(color.name());
    }

    if(!mRenderers.value(color.name())->isValid())
        return 0;

    return mRenderers.value(color.name());

}

void Stitch::reloadIcon()
{
    setupSvgFiles();
}

qreal Stitch::width()
{
    qreal w = 32.0;
    if(isSvg()) {
        QSvgRenderer *r = mRenderers.value("#000000");
        if(!r)
            return w;
        w = r->viewBoxF().width();
    } else {
        if(mPixmap)
            w = mPixmap->width();
    }

    return w;
}


qreal Stitch::height()
{
    qreal h = 32.0;
    if(isSvg()) {
        QSvgRenderer* r = mRenderers.value("#000000");
        if(!r)
            return h;
        h = r->viewBoxF().height();
    } else {
        if(mPixmap)
            h = mPixmap->height();
    }
    return h;
}
