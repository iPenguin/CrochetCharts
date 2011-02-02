/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "splashscreen.h"

#include <QPainter>
#include <QBitmap>
#include <QApplication>

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

    QString version = QString("Version: %1").arg(qApp->applicationVersion());
    painter->drawText(QRectF(135, 115, 250, 50), version);
    painter->drawText(QRectF(140, 128, 250, 50), mMessage);
}

void SplashScreen::showMessage (const QString &message)
{
    if(message != mMessage) {
        mMessage = message;
        this->repaint();
    }
}
