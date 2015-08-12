/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
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
    ExportUi(QTabWidget* tabWidget, QMap<QString, int>* stitches,
             QMap<QString, QMap<QString, qint64> >* colors, QWidget* parent = 0);

    QString exportType,
            selection,
            fileName;
    int resolution,
        width,
        height;
    bool pageToChartSize;
	bool selectionOnly;
    QGraphicsScene* scene;
    
public slots:
    int exec();
    
private slots:
    void exportData();
    
    void updateExportOptions(QString expType);
    void setSelection(QString selection);

    void updateColorLegend();
    void updateStitchLegend();

    void updateHightFromWidth(int width);
    void updateWidthFromHeight(int height);

private:
    void generateSelectionList(bool showAll);

    void setupColorLegendOptions();
    void setupStitchLegendOptions();
    void setupChartOptions();
    
    void exportLegendPdf();
    void exportLegendSvg();
    void exportLegendImg();

    void exportPdf();
    void exportSvg();
    void exportImg();
    
    void updateChartSizeRatio(QString selection);
    qreal sceneRatio(QRectF rect);

    Ui::ExportDialog* ui;
    QTabWidget* mTabWidget;

    StitchLegend* sl;
    ColorLegend* cl;
    
    QMap<QString, int>* mStitches;
    QMap<QString, QMap<QString, qint64> >* mColors;

};

#endif //EXPORTUI_H

