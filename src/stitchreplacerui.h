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
    explicit StitchReplacerUi(QList<QString> patternStitches, QWidget *parent = 0);
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
