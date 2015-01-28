/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
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
#ifndef STITCH_H
#define STITCH_H

#include <QString>
#include <QList>
#include <QObject>
#include <QMap>
#include <QColor>

class QSvgRenderer;
class QPixmap;

class Stitch : public QObject
{
    friend class StitchSet;
    friend class StitchLibrary;
    friend class TestStitch;
public:

    enum StitchParts { Name = 0,
                       Icon = 1,
                       Description = 2,
                       Category = 3,
                       WrongSide = 4
                     };
    
    Stitch(QObject *parent = 0);
    ~Stitch();

    QString name() const { return mName; }
    QString file() const { return mFile; }
    QString description() const { return mDescription; }
    QString category() const { return mCategory; }
    QString wrongSide() const { return mWrongSide; }

    qreal width();
    qreal height();

    bool isSvg();

    QPixmap* renderPixmap();
    QSvgRenderer* renderSvg(QColor color = QColor(Qt::black));

    //reload the svg with new colors.
    void reloadIcon();

    /**
     *used to track individual stitches as they are moved to the overlay.
     */
    bool isBuiltIn;

protected:
    void setName(QString n) { mName = n; }
    void setFile(QString f);
    void setDescription(QString desc) { mDescription = desc; }
    void setCategory(QString cat) { mCategory = cat; }
    void setWrongSide(QString ws) { mWrongSide = ws; }

    void addStitchColor(QString color);

private:
    bool setupSvgFiles();

    QString mName;
    QString mFile;
    QString mDescription;
    QString mCategory;
    QString mWrongSide;
    bool mIsSvg;

    QMap<QString, QSvgRenderer*> mRenderers;

    QPixmap* mPixmap;
};

#endif //STITCH_H
