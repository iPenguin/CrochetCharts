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

private slots:
    void addIcon();
    void removeIcon();
    void saveIcon();
    void updateIconList(QString fileName);
    
signals:
    void newIconAdded(QString fileName);

private:
    void loadIcons();
    
    Ui::StitchIconDialog *ui;
    
};
#endif //STITCHICONUI_H
