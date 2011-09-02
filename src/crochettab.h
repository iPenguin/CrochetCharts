/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CROCHETTAB_H
#define CROCHETTAB_H

#include <QWidget>
#include <QMap>

#include "chartview.h"
#include <QPointer>

#include "scene.h"

#include "roweditdialog.h"

class QGraphicsView;
class TextView;
class QUndoStack;

namespace Ui {
    class OptionsBar;
}

class CrochetTab : public QWidget
{
    Q_OBJECT
    friend class SaveFile;
    friend class SaveThread;
    friend class ExportUi;
public:

    explicit CrochetTab(Scene::ChartStyle style, int defEditMode, QString defStitch, QColor defFgColor, QColor defBgColor, QWidget* parent = 0);
    ~CrochetTab();

    void renderChart(QPainter* painter, QRectF rect = QRectF());

    void setPatternStitches(QMap<QString, int>* stitches) { mPatternStitches = stitches; }
    void setPatternColors(QMap<QString, QMap<QString, qint64> >* colors) { mPatternColors = colors; }

    //list of modes available from this tab.
    QStringList editModes();

    void setEditMode(int mode);
    int editMode() const;

    void setEditFgColor(QColor color);
    void setEditBgColor(QColor color);

    void setEditStitch(QString stitch);

    void sceneUpdate();

    void updateRows();

    void alignSelection(int alignmentStyle);
    void distributeSelection(int distributionStyle);
    
signals:
    void chartStitchChanged();
    void chartColorChanged();
    void tabModified(bool state);
    
public slots:
    void zoomIn();
    void zoomOut();

    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);

    QUndoStack* undoStack();

    void createChart(int rows, int cols, QString defStitch, QSizeF rowSize);

    void copyInstructions();

    /**
     *Clear any selections of objects on the chart.
     */
    void clearSelection();

    void showRowEditor(bool state);
    
protected:
    QMap<QString, int>* patternStitches() { return mPatternStitches; }
    
private slots:
    void showChartOptions();
    void zoomChanged(int value);
    void updateZoomLevel(int percent);

    void setShowChartCenter(bool state);
    
protected:
    Scene* scene() { return mScene; }
    
private:    
    QPointer<ChartView> mView;
    Scene* mScene;
    TextView* mTextView;

    QMap<QString, int>* mPatternStitches;
    QMap<QString, QMap<QString, qint64> >* mPatternColors;
    
    QString mName;

    Ui::OptionsBar* ui;
    RowEditDialog* mRowEditDialog;
    

    Scene::ChartStyle mChartStyle;
};

#endif // CROCHETTAB_H
