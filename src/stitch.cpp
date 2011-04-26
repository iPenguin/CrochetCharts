/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitch.h"

#include <QPainter>
#include <QPixmap>
#include <QtSvg/QSvgRenderer>

#include <QDebug>
#include <QFile>

#include "settings.h"

Stitch::Stitch()
{
    mSvgRenderer = new QSvgRenderer();
    mSvgRendererAlt = new QSvgRenderer();
    mPixmap = 0;
}

Stitch::~Stitch()
{
    delete mSvgRenderer;
    mSvgRenderer = 0;
    delete mSvgRendererAlt;
    mSvgRenderer = 0;
    delete mPixmap;
    mPixmap = 0;
}

void Stitch::setFile ( QString f )
{
    if(mFile != f) {
        mFile = f;
        
        if(isSvg())
            setupSvgFiles();
        
        if(!mSvgRenderer->isValid() && !isSvg()) {
            mPixmap = new QPixmap(mFile);
        }
    }
}

void Stitch::setupSvgFiles()
{
    QFile file(mFile);
    file.open(QIODevice::ReadOnly);

    QByteArray data = file.readAll();
    QByteArray priData, secData;
    
    QString black = "#000000";
    
    QString pri = Settings::inst()->value("stitchPrimaryColor").toString();
    QString sec = Settings::inst()->value("stitchAlternateColor").toString();

    priData = data;
    secData = data;

    if(pri != black)
        priData = priData.replace(QByteArray(black.toLatin1()), QByteArray(pri.toLatin1()));
    mSvgRenderer->load(priData);

    if(sec != black)
        secData = data.replace(QByteArray(black.toLatin1()), QByteArray(sec.toLatin1()));
    mSvgRendererAlt->load(secData);
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

QSvgRenderer* Stitch::renderSvg(bool useAltRenderer)
{
    if(!isSvg())
        return 0;

    if(!mSvgRenderer->isValid())
        return 0;
    bool useAltColors = Settings::inst()->value("useAltColors").toBool();
    if(useAltRenderer && useAltColors)
        return mSvgRendererAlt;
    else
        return mSvgRenderer;
}

Stitch* Stitch::copy()
{
    Stitch *s = new Stitch();

    s->setCategory(category());
    s->setDescription(description());
    s->setName(name());
    s->setWrongSide(wrongSide());
    s->setFile(file());

    return s;
}

void Stitch::reload()
{
    setupSvgFiles();
}
