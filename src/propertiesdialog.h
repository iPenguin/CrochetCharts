#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include <QDockWidget>

namespace Ui {
    class PropertiesDialog;
}

class PropertiesDialog : public QDockWidget
{
    Q_OBJECT

public:
    explicit PropertiesDialog(QWidget *parent = 0);
    ~PropertiesDialog();

private:
    Ui::PropertiesDialog *ui;
};

#endif // PROPERTIESDIALOG_H
