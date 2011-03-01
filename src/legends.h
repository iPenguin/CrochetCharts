/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
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
    ColorLegend(QMap<QString, QMap<QString, qint64> > *colors, QGraphicsItem *parent = 0);
    ~ColorLegend();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    
private:

    QMap<QString, QMap<QString, qint64> > *mPatternColors;
    QMap<qint64, QString> sortedColors;
    
    bool showHexValues;
    int columnCount;
    QString colorNumber;
    QString prefix;
    
};


class StitchLegend : public QGraphicsWidget
{
    Q_OBJECT
public:
    StitchLegend(QMap<QString, int> *stitches, QGraphicsItem* parent = 0);
    ~StitchLegend();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,  QWidget *widget = 0);


private:
    QMap<QString, int> *mPatternStitches;

    bool drawDescription;
    bool drawWrongSide;
    int columnCount;
};

#endif //LEGENDS_H