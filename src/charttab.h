/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CHARTTAB_H
#define CHARTTAB_H

#include <QWidget>
#include <QMap>

#include "chartview.h"
#include <qundostack.h>
#include <QPointer>

#include <QAction>

class QGraphicsView;
class CrochetScene;
class CrochetTextView;

class ChartTab : public QWidget
{
    Q_OBJECT
    friend class SaveFile;
public:
    explicit ChartTab(QWidget *parent = 0);
    ~ChartTab();

    void renderChart(QPainter *painter, QRectF rect = QRectF());

    void setPatternStitches(QMap<QString, int> *stitches) { mPatternStitches = stitches; }
    void setPatternColors(QMap<QString, QMap<QString, int> > *colors) { mPatternColors = colors; }


    QList<QAction*> editModeActions() { return mModeActions; }
    
signals:
    void chartStitchChanged();
    void chartColorChanged();
    
public slots:
    void zoomIn();
    void zoomOut();

    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);

    CrochetScene* scene() { return mScene; }

    QUndoStack* undoStack() { return &mUndoStack; }

private:
    void setupMenuItems();
    
    QPointer<ChartView> mView;
    CrochetScene *mScene;
    QPointer<CrochetTextView> mTextView;

    QMap<QString, int> *mPatternStitches;
    QMap<QString, QMap<QString, int> > *mPatternColors;

    QList<QAction*> mChartActions;
    QList<QAction*> mModeActions;
    
    QUndoStack mUndoStack;
    
    QString mName;
};

#endif // CHARTTAB_H
