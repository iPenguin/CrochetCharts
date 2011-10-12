/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
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

    QString placeholder = Settings::inst()->value("placeholder").toString();
    QString rowText = "";
    QString curStitch;

    QStringList rowList;

    int cols = mScene->columnCount(row);

    //create a list of stitches
    for(int c = 0; c < cols; ++c) {
        Cell* cell = mScene->grid[row][c];
        if(!cell)
            continue;

        curStitch = cell->name();
        if(cleanOutput) {
            //parse out any placeholder stitches before continuing.
            if(curStitch == placeholder)
                continue;
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
    QString curStitch, previousStitch, nextStitch;
    QMap<QString, QStringList> data;
    int count = 1;
    bool firstPass = true;

    bool genRepeats = Settings::inst()->value("generateTextRepeats").toBool();
    QString prefix = ".sws_";

    //FIXME: useRepeats is a hard coded solution to prevent the software from acting weird.
    if(genRepeats && useRepeats) {
        data = generateRepeats(row, prefix);
        row = data.value("row");
    }

    previousStitch = "";
    for(int i = 0; i < row.count(); ++i) {
        curStitch = row.value(i);
        nextStitch = row.value(i + 1);

        if(curStitch != previousStitch) {
            if(!firstPass) text += ", ";

            if(curStitch.startsWith(prefix)) {
                text += "[" + generateText(data.value(curStitch)) + "] ";
            } else
                text += curStitch;
        }
        if(curStitch == previousStitch)
            count++;
        if(curStitch != nextStitch) {
            text += QString::number(count);
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

    for(int i = 0; i < count; ++i) {
        QString value = stitches.value(i);
        for(int j = i + 2; j < count; ++j) {
            if(stitches.value(i) == stitches.value(j)) {
                int diff = j - i;
                int diffSts = false;
                for(int l = 0; l < diff; ++l) {
                    if(stitches.value(i + l) != stitches.value(i))
                        diffSts = true;
                }
                if(!diffSts)
                    continue;

                int count = matchCount(stitches, i, diff);

                if(count > 1) {
                    QStringList sub;
                    for(int k = 0; k < diff; ++k)
                        sub.append(stitches.value(k));
                    for(int k = 0; k < count; ++k)
                        row.append(prefix + QString::number(i));
                    data.insert(prefix + QString::number(i), sub);
                    j += (diff * count);
                    i += (diff * count);
                }
            }
        }
        if(i < count)
            row.append(value);
    }

    data.insert("row", row);
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
    qDebug() << data;
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
qDebug() << count;
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