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
#include "textview.h"

#include <QDebug>
#include "settings.h"
#include "scene.h"

#include <math.h>

TextView::TextView(QWidget* parent, Scene* scene)
    : QObject(parent), mScene(scene)
{
}

TextView::~TextView()
{
}

QString TextView::generateTextRow(int row, bool cleanOutput, bool useRepeats)
{

    QString rowText = "";
    QString curStitch;

    QStringList rowList;

    int cols = mScene->columnCount(row);

    //create a list of stitches
    for(int c = 0; c < cols; ++c) {
		qDebug() << "row iteration!";
        Cell* cell = mScene->grid[row][c];
        if(!cell)
            continue;

        curStitch = cell->name();
        if(cleanOutput) {
            //TODO: any special preprocessing that needs to be done.
        }
        rowList.append(curStitch);
        curStitch = "";
    }
    rowText = generateText(rowList, useRepeats);

    if(cleanOutput) {
        //capitalize the first letter.
        for(int i = 0; i < rowText.count(); ++i) {

            if(rowText.at(i).isLetter()) {
                rowText[i] = rowText.at(i).toUpper();
                break;
            }
        }

        rowText += ".";
    }

    return rowText;
}

QString TextView::generateText(QStringList row, bool useRepeats)
{
    QString text;
    QString curStitch, previousStitch;
    QMap<QString, QStringList> data;
    int count = 1;
    bool firstPass = true;

    bool genRepeats = Settings::inst()->value("generateTextRepeats").toBool();
    QString prefix = ".sws_";

    /*if(genRepeats && useRepeats) {
        data = generateRepeats(row, prefix);
        row = data.value("row");
    }*/

    previousStitch = "";
    for(int i = 0; i < row.count(); ++i) {
        curStitch = row.value(i);

        if(curStitch != previousStitch) {
            if(!firstPass) text += ", ";

            if(curStitch.startsWith(prefix)) {
                text += "[" + generateText(data.value(curStitch)) + "]";
            } else
                text += curStitch;
        }

        if(curStitch == previousStitch) {
            count++;
        } else {
            text += " " + QString::number(count);
            count = 1;
            if(curStitch.startsWith(prefix))
                text += " times";
        }

        previousStitch = curStitch;
        firstPass = false;
    }

    return text;
}

QMap< QString, QStringList > TextView::generateRepeats(QStringList stitches, QString prefix)
{
    QMap<QString, QStringList> data;
    QStringList row;
    int count = stitches.count();
    bool repeat = true;

    //loop through each stitch in the row to find matches.
    for(int i = 0; i < count; ++i) {
        QStringList stRepeat;

        int match = stitches.indexOf(stitches[i], i+1);

        //no match for this stitch
        if(match == -1) {
            continue;
        }

        int diff = match - i;
        //don't try to match beyond the end of the array.
        int endDiff = stitches.count() - match;
        if(endDiff < diff)
            diff = endDiff;

        //walk through the match and see if it is a repeat.
        for(int j = i; j < match; j++) {
            qDebug() << j << stitches[j] << j + diff << stitches[j + diff];
            if(stitches[j] != stitches[j + diff])
                repeat = false;
            else {
                stRepeat.append(stitches[j]);
            }
        }

        if(!repeat) {
            if(stRepeat.count() <= 1) {
                stRepeat.clear();
                repeat = true;
            }
        }

        data.insert(prefix + QString::number(i), stRepeat);

    }

    data.insert("row", row);

    qDebug() << data;
    return data;
}

int TextView::matchCount(QStringList stitches, int startPos, int length)
{
    QStringList sub;
    int count = 0;

    for(int i = 0; i < length; ++i)
        sub.append(stitches.value(startPos + i));

    for(int i = startPos; i < stitches.count(); ++i) {

        if(sub.value(i%length) != stitches.value(i))
            break;
        count = floor(double(i + 1 - startPos)/length);
    }

    return count;
}

QString TextView::cleanToken(QString token)
{
    token = token.simplified().toLower();
    if(token.startsWith("["))
        token.replace("[", "");

    if(token.contains(QRegExp("\\].{,}$")))
        token.replace(QRegExp("\\].{,}"), "");

    if(token.startsWith("*"))
        token.replace("*", "");
    if(token.contains(QRegExp(";.{,}$")))
        token.replace(QRegExp(";.{,}"), "");

    return token;
}

QString TextView::copyInstructions()
{
    QStringList rowText;
    QString text;

    int rows = mScene->rowCount();
    for(int r = 0; r < rows; ++r) {
        rowText << generateTextRow(r, true, true);
    }

    QMap<QString, QStringList> data;
    QString prefix = ".sws_rows_";

    data = generateRepeatRows(rowText, prefix);
    
    for(int i = 0; i < rowText.count(); ++i) {
        QString row = rowText[i];
        //don't match rows that are already repeats.
        if(row.startsWith(tr("Repeat row")))
            continue;

        if(rowText.count(row) > 1) {
            for(int j = i + 1; j < rowText.count(); ++j) {
                if(rowText[j] == row)
                    rowText[j] = tr("Repeat row %1.").arg(i+1);
            }
        }
    }

    for(int i = 0; i < rowText.count(); ++i)
        text += tr("Row %1: %2\n").arg(i+1).arg(rowText[i]);

    if(text == "")
        text = "There are no rows on this chart, to create rows please goto the \"Modes\" menu and select \"Row Edit\"";
    qDebug() << text;
    return text;

}

QMap< QString, QStringList > TextView::generateRepeatRows(QStringList rows, QString prefix)
{
    QMap<QString, QStringList> data;
    QStringList newRows;
    int count = rows.count();

    for(int i = 0; i < count; ++i) {
        QString value = rows.value(i);
        for(int j = i + 1; j < count; ++j) {
            if(rows.value(i) == rows.value(j)) {
                int diff = j - i;
                int diffSts = false;
                for(int l = 0; l <= diff; ++l) {
                    if(rows.value(i + l) != rows.value(i))
                        diffSts = true;
                }
                if(!diffSts)
                    continue;

                int count = matchCount(rows, i, diff);
                
                if(count > 1) {
                    QStringList sub;
                    for(int k = 0; k < diff; ++k) {
                        sub.append(rows.value(k));
                        newRows.append(prefix + QString::number(i));
                    }
                    data.insert(prefix + QString::number(i), sub);
                    j += (diff * count);
                    i += (diff * count);
                }
            }
        }
        if(i < count)
            newRows.append(value);
    }

    data.insert("rows", newRows);
    return data;
}
