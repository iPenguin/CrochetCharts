/*************************************************\
| Copyrigth (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef STITCHICONUI_H
#define STITCHICONUI_H

#include <QDialog>
#include "ui_stitchicon.h"

#include <QDebug>

namespace Ui {
    class StitchIconDialog;
}

class StitchIconUi : public QDialog
{
    Q_OBJECT
public:
    StitchIconUi(QWidget *parent = 0);
    ~StitchIconUi();

    void loadIcons();

private:
    Ui::StitchIconDialog *ui;
    
};
#endif //STITCHICONUI_H
