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
     * If cleanOutput = true remove the placeholder stitches 'x' by default.
     **/
    QString generateTextRow(int row, bool cleanOutput = false, bool useRepeats = false);
    
private:
    /**
     * This function strips off any incomplete repeat indicators or other
     * special text input from the user that might be incomplete.
     */
    QString cleanToken(QString token);

    QMap<QString, QStringList> generateRepeats(QStringList stitches, QString prefix);

    /**
     * Take a list of stitches and convert them into a crochet sentence.
     */
    QString generateText(QStringList row, bool useRepeats = false);
    int matchCount(QStringList stitches, int startPos, int length);

private:
    Scene* mScene;

};

#endif //TEXTVIEW_H
