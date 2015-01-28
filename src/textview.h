/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
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
