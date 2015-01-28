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
