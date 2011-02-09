/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
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
    void addStitchToMasterSet(int row);
    void addStitchToSet();
    void resetLibrary();

    void hideProperties();
    void updateStitchSetProperties();
    void printStitchSet();

    void createSet();
    void removeSet();
    
    void importSet();
    void exportSet();
   
private slots:
    void iconDialog();
    
private:
    Ui::StitchLibraryDialog *ui;

    void setDialogSize();
    void setupPropertiesBox();

    //update the source dropdown list and switch to setName
    void updateSourceDropDown(QString setName = "");

};

#endif //STITCHLIBRARYUI_H
