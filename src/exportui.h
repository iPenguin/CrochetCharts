#ifndef EXPORTUI_H
#define EXPORTUI_H

#include <QDialog>

namespace Ui {
    class ExportDialog;
}

class ExportUi : public QDialog
{
    Q_OBJECT
public:
    ExportUi(QWidget *parent = 0);

private slots:
    void updateExportOptions(QString exportType);

private:
    Ui::ExportDialog *eui;
};

#endif //EXPORTUI_H

