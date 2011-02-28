/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "legends.h"
#include <QPainter>

#include "settings.h"

#include <QDebug>
#include <QGraphicsScene>

ColorLegend::ColorLegend(QMap<QString, QMap<QString, qint64> > *colors, QGraphicsItem *parent)
    : QGraphicsWidget(parent)
{
    mPatternColors = colors;

    margin = 5;
    iconHeight = 32;
    iconWidth = 32;

    QStringList keys = mPatternColors->keys();
    
    foreach(QString key, keys) {
        qint64 added = mPatternColors->value(key).value("added");
        sortedColors.insert(added, key);
    }

    showHexValues = Settings::inst()->value("showHexValues").toBool();
    columnCount = Settings::inst()->value("colorColumnCount").toInt();
    colorNumber = Settings::inst()->value("colorPrefix").toString();
    prefix = Settings::inst()->value("colorPrefix").toString();
    
}

ColorLegend::~ColorLegend()
{
}

void ColorLegend::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    int textHeight = painter->fontMetrics().height();
    
    QList<qint64> sortedKeys = sortedColors.keys();
    
    int imageHeight = sortedColors.count() * (margin + iconHeight) + margin;
    int imageWidth = margin + iconWidth + margin +
                     painter->fontMetrics().width(prefix + sortedColors.count()) +
                     painter->fontMetrics().width(" - #FFFFFF") + margin;
        
    painter->fillRect(QRect(0,0,imageWidth,imageHeight), Qt::white);
    
    for(int i = 0; i < sortedKeys.count(); ++i) {
        QString hex = sortedColors.value(sortedKeys.at(i));
        int width = painter->fontMetrics().width(hex);

        painter->drawPixmap(margin,
                            margin + ((margin + iconHeight) * i),
                            drawColorBox(QColor(hex), QSize(iconWidth, iconHeight)));
        painter->drawText(margin + iconWidth + margin,
                          margin + ((margin + iconHeight) * i) + (.5 * (iconHeight + textHeight)),
                          prefix + QString::number(i + 1));
        if(showHexValues)
            painter->drawText(margin + iconWidth + margin +
                              painter->fontMetrics().width(prefix + QString::number(i + 1)),
                              margin + ((margin + iconHeight) * i) + (.5 * (iconHeight + textHeight)),
                              " - " + hex.toUpper());
            painter->drawText(0,0, colorNumber);
    }

    scene()->setSceneRect(0, 0, imageWidth, imageHeight);
}

QPixmap ColorLegend::drawColorBox(QColor color, QSize size)
{
    QPixmap pix = QPixmap(size);
    QPainter p;
    p.begin(&pix);
    p.fillRect(QRect(QPoint(0, 0), size), QColor(color));
    p.drawRect(0, 0, size.width() - 1, size.height() - 1);
    p.end();
    
    return pix;
}
