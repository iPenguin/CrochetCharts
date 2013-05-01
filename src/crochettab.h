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
    friend class FileFactory;
    friend class FileLoad_v1;
    friend class FileLoad_v2;
    friend class ExportUi;
    friend class PropertiesDock;
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
    void arrangeGrid(QSize grid, QSize alignment, QSize spacing, bool useSelection);

    void mirror(int direction);
    void rotate(qreal degrees);

    void copy();
    void cut();
    void paste();

    void group();
    void ungroup();

    bool hasChartCenter();
    void setChartCenter(bool state);

    void setShowGuidelines(QString guides);
    bool hasGuidelines();
    
    void propertiesUpdate(QString property, QVariant newValue);

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

    void createChart(Scene::ChartStyle style, int rows, int cols, QString defStitch, QSizeF rowSize, int increaseBy);

    void copyInstructions();

    /**
     *Clear any selections of objects on the chart.
     */
    void clearSelection();

    void showRowEditor(bool state);

    void replaceStitches(QString original, QString replacement);
    
protected:
    QMap<QString, int>* patternStitches() { return mPatternStitches; }
    
private slots:
    void showChartOptions();
    void zoomChanged(int value);
    void updateZoomLevel(int percent);

    void setShowChartCenter(bool state);
    
protected:
    Scene* scene() { return mScene; }
    ChartView* view() { return mView; }
    
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
