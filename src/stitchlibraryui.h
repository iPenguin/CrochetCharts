/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef STITCHLIBRARYUI_H
#define STITCHLIBRARYUI_H

#include <QDialog>
#include "ui_stitchlibrary.h"

class StitchSet;

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

    void addStitch();
    void removeStitch();
    void addSelected();
    
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
    Ui::StitchLibraryDialog* ui;

    void setDialogSize();
    void setupPropertiesBox();
    void setButtonStates(StitchSet* set);

    //update the source dropdown list and switch to setName
    void updateSourceDropDown(QString setName = "");

};

#endif //STITCHLIBRARYUI_H
