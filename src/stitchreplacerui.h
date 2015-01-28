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
#ifndef STITCHREPLACERUI_H
#define STITCHREPLACERUI_H

#include <QDialog>

class Scene;

namespace Ui {
    class StitchReplacerUi;
}

class StitchReplacerUi : public QDialog
{
    Q_OBJECT

public:
    explicit StitchReplacerUi(QString stitch, QList<QString> patternStitches, QWidget *parent = 0);
    ~StitchReplacerUi();

    QString original;
    QString replacement;

private slots:
    void updateStitches(QString newStitch);

private:
    //Hold the list of stitches used in this pattern.
    QList<QString> mOriginalStitchList;
    Ui::StitchReplacerUi *ui;

    void populateStitchLists();

};

#endif // STITCHREPLACERUI_H
