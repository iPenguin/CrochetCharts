#ifndef STITCHPROPERTIES_H
#define STITCHPROPERTIES_H

#include <QString>
#include <QColor>
#include <QPointF>

class StitchProperties
{
public:
    inline StitchProperties();

    qreal angle;
    QPointF scale;
    QColor color;
    QColor bgColor;
    QString stitch;

    QPointF position;
};

inline StitchProperties::StitchProperties() {}

#endif // STITCHPROPERTIES_H
