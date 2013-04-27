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
    explicit StitchReplacerUi(QWidget *parent = 0);
    ~StitchReplacerUi();

    QString original;
    QString replacement;

private slots:
    void updateStitches(QString newStitch);

private:
    Ui::StitchReplacerUi *ui;

    void populateStitchLists();

};

#endif // STITCHREPLACERUI_H
