/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "splashscreen.h"

#include <QPainter>
#include <QBitmap>
#include "appinfo.h"

#include <QDebug>

SplashScreen::SplashScreen()
{
    QPixmap p(":/images/splashscreen.png");
    this->setPixmap(p);
    this->setMask(p.mask());
}

void SplashScreen::drawContents (QPainter *painter)
{
    QRect rect = QRect(QPoint(0,0), this->pixmap().size());
    painter->drawPixmap(rect, this->pixmap());

    QFont origFont = painter->font();

    QFont titleFont = QFont(origFont.family());
    titleFont.setPointSize(50);    

    painter->setFont(titleFont);
    painter->drawText(QRect(25, 100, 400, 100), AppInfo::inst()->appName);

    QFont byFont = QFont(origFont.family());
    byFont.setPointSize(16);
    painter->setFont(byFont);
    painter->drawText(QRect(40, 175, 400, 100), tr("By %1").arg(AppInfo::inst()->appOrg));
    
    painter->setFont(origFont);
    QString version = QString("Version: %1").arg(AppInfo::inst()->appVersion);
    painter->drawText(QRectF(75, 230, 250, 50), version);
    painter->drawText(QRectF(75, 245, 250, 50), mMessage);
}

void SplashScreen::showMessage (const QString &message)
{
    if(message != mMessage) {
        mMessage = message;
        this->repaint();
    }
}
