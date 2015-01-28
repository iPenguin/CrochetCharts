/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
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

void SplashScreen::drawContents (QPainter* painter)
{
    QRect rect = QRect(QPoint(0,0), this->pixmap().size());
    painter->drawPixmap(rect, this->pixmap());

    QFont origFont = painter->font();
/*
    QFont titleFont = QFont(origFont.family());
    titleFont.setPointSize(50);    

    painter->setFont(titleFont);
    painter->drawText(QRect(25, 200, 400, 100), AppInfo::inst()->appName);

    QFont byFont = QFont(origFont.family());
    byFont.setPointSize(30);
    painter->setFont(byFont);
    painter->drawText(QRect(25, 125, 400, 100), AppInfo::inst()->appOrg);
 */   
    painter->setFont(origFont);
    QString version = QString("Version: %1").arg(AppInfo::inst()->appVersionShort);
    painter->drawText(QRectF(75, 280, 250, 50), version);
    painter->drawText(QRectF(75, 295, 250, 50), mMessage);
}

void SplashScreen::showMessage (const QString &message)
{
    if(message != mMessage) {
        mMessage = message;
        this->repaint();
    }
}
