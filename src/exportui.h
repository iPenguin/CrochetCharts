/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef EXPORTUI_H
#define EXPORTUI_H

#include <QDialog>
#include <QTabWidget>

namespace Ui {
    class ExportDialog;
}

class ExportUi : public QDialog
{
    Q_OBJECT
public:
    ExportUi(QTabWidget *tabWidget, QWidget *parent = 0);

    QString exportType,
            selection;
    int resolution,
        width,
        height;
    
private slots:
    void updateExportOptions(QString expType);
    void setValues();

private:
    void generateSelectionList();
    
    Ui::ExportDialog *eui;
    QTabWidget *mTabWidget;
};

#endif //EXPORTUI_H

