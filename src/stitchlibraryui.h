#ifndef STITCHLIBRARYUI_H
#define STITCHLIBRARYUI_H

#include <QDialog>
#include "ui_stitchlibrary.h"

namespace Ui {
    class StitchLibraryDialog;
}

class StitchLibraryUi : public QDialog
{
    Q_OBJECT
public:
    explicit StitchLibraryUi(QWidget* parent = 0);
    ~StitchLibraryUi();

private slots:
    void changeStitchSet(QString setName);

private:
    Ui::StitchLibraryDialog *ui;

    void setDialogSize();

};

#endif //STITCHLIBRARYUI_H
