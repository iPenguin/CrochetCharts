#ifndef PROPERTIESDATA_H
#define PROPERTIESDATA_H

#include <QString>
#include <QColor>
#include <QPointF>

class PropertiesData
{
public:
    inline PropertiesData();

    enum UiSelection {
        None = 0,
        SceneUi = 5,
        CellUi = 10,
        ItemGroup = 15,
        CenterUi = 20,
        IndicatorUi = 25,
        MixedUi = 100
    };
    
    qreal angle;
    QPointF scale;
    QColor color;
    QColor bgColor;
    QString stitch;

    QPointF position;

    UiSelection selectionUi;
};

inline PropertiesData::PropertiesData() {}

#endif // PROPERTIESDATA_H
