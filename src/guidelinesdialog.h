#ifndef GUIDELINESDIALOG_H
#define GUIDELINESDIALOG_H

#include <QDialog>

namespace Ui {
class GuidelinesDialog;
}

class GuidelinesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GuidelinesDialog(QWidget *parent = 0);
    ~GuidelinesDialog();
    
private:
    Ui::GuidelinesDialog *ui;
};

#endif // GUIDELINESDIALOG_H
