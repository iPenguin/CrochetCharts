/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef LEGENDS_H
#define LEGENDS_H

#include <QGraphicsWidget>
#include <QMap>

class ColorLegend : public QGraphicsWidget
{
    Q_OBJECT
public:
    ColorLegend(QMap<QString, QMap<QString, qint64> > *colors, QGraphicsItem *parent = 0);
    ~ColorLegend();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QPixmap drawColorBox(QColor color, QSize size);
    
private:

    QMap<QString, QMap<QString, qint64> > *mPatternColors;
    QMap<qint64, QString> sortedColors;
    
    int margin;
    int iconWidth;
    int iconHeight;

    bool showHexValues;
    int columnCount;
    QString colorNumber;
    QString prefix;
    
};
#endif //LEGENDS_H