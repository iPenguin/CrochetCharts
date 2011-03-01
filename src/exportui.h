/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef EXPORTUI_H
#define EXPORTUI_H

#include <QDialog>
#include <QTabWidget>
#include <QGraphicsScene>
#include <QMap>
#include "legends.h"

namespace Ui {
    class ExportDialog;
}

class ExportUi : public QDialog
{
    Q_OBJECT
public:
    ExportUi(QTabWidget *tabWidget, QMap<QString, int> *stitches,
             QMap<QString, QMap<QString, qint64> > *colors, QWidget *parent = 0);

    QString exportType,
            selection,
            fileName;
    int resolution,
        width,
        height;
    QGraphicsScene *scene;
    
public slots:
    int exec();
    
private slots:
    void updateExportOptions(QString expType);
    void setValues();
    void setSelection(QString selection);

    void updateColorLegend();
    void updateStitchLegend();

private:
    void generateSelectionList(bool showAll);

    void setupColorLegendOptions();
    void setupStitchLegendOptions();
    
    void exportLegendPdf();
    void exportLegendSvg();
    void exportLegendImg();
    
    Ui::ExportDialog *ui;
    QTabWidget *mTabWidget;

    StitchLegend *sl;
    ColorLegend  *cl;
    
    QMap<QString, int> *mStitches;
    QMap<QString, QMap<QString, qint64> > *mColors;
};

#endif //EXPORTUI_H

