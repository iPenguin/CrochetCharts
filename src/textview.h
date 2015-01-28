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
#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <QObject>
#include <QMap>

class Scene;

class TextView : public QObject
{
    Q_OBJECT

public:
    TextView(QWidget* parent = 0, Scene* scene = 0);
    ~TextView();

    void setScene(Scene* scene) { mScene = scene; }

    QString copyInstructions();

    /**
     * Generates the text for a given row of the chart without trailing \n.
     * If cleanOutput = true do any special processing for the text.
     **/
    QString generateTextRow(int row, bool cleanOutput = false, bool useRepeats = false);
    
private:
    /**
     * This function strips off any incomplete repeat indicators or other
     * special text input from the user input that might be incomplete.
     */
    QString cleanToken(QString token);

    /**
     * @brief generateRepeats - extract repeats from a row of stitches.
     * @param stitches - the row of stitches
     * @param prefix - the prefix to use as a substitute for a match
     * @return the original row and any substitutes created in place of repeats.
     */
    QMap<QString, QStringList> generateRepeats(QStringList stitches, QString prefix);

    QMap<QString, QStringList> generateRepeatRows(QStringList stitches, QString prefix);

    /**
     * Take a list of stitches and convert them into a crochet sentence.
     */
    QString generateText(QStringList row, bool useRepeats = false);
    
    /**
     * Returns the number of repeat matches occurring in stitches
     */
    int matchCount(QStringList stitches, int startPos, int length);

private:
    Scene* mScene;

};

#endif //TEXTVIEW_H
