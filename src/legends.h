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
#ifndef LEGENDS_H
#define LEGENDS_H

#include <QGraphicsWidget>
#include <QMap>

namespace Legend {
    const int margin = 5;
    
    const int iconHeight = 32;
    const int iconWidth = 32;

    QPixmap drawColorBox(QColor color, QSize size);
    
}

class ColorLegend : public QGraphicsWidget
{
    Q_OBJECT
public:
    ColorLegend(QMap<QString, QMap<QString, qint64> >* colors, QGraphicsItem* parent = 0);
    ~ColorLegend();

    bool showTitle;
    bool showHexValues;
    bool showBorder;
    int columnCount;
    QString prefix;
    QString sortBy;

    QRectF boundingRect() const;
    
protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    
private:

    QMap<QString, QMap<QString, qint64> >* mPatternColors;
    QMap<qint64, QString> sortedColors;
    QSizeF mSize;
};


class StitchLegend : public QGraphicsWidget
{
    Q_OBJECT
public:
    StitchLegend(QMap<QString, int>* stitches, QGraphicsItem* parent = 0);
    ~StitchLegend();

    bool showTitle;
    bool showBorder;
    bool showDescription;
    bool showWrongSide;
    int columnCount;

    QRectF boundingRect() const;
    
protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,  QWidget* widget = 0);

private:
    QMap<QString, int>* mPatternStitches;
    QSizeF mSize;
};

#endif //LEGENDS_H
