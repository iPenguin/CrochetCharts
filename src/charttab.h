/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CHARTTAB_H
#define CHARTTAB_H

#include <QWidget>
#include <QMap>

#include "chartview.h"
#include <QPointer>

class QGraphicsView;
class CrochetScene;
class CrochetTextView;
class QUndoStack;

class ChartTab : public QWidget
{
    Q_OBJECT
    friend class SaveFile;
public:
    explicit ChartTab(QWidget *parent = 0);
    ~ChartTab();

    void renderChart(QPainter *painter, QRectF rect = QRectF());

    void setPatternStitches(QMap<QString, int> *stitches) { mPatternStitches = stitches; }
    void setPatternColors(QMap<QString, QMap<QString, qint64> > *colors) { mPatternColors = colors; }

    //list of modes available from this tab.
    QStringList editModes();

    void setEditMode(int mode);
    int editMode() const;

    void setEditFgColor(QColor color);
    void setEditBgColor(QColor color);

    void setEditStitch(QString stitch);
    
signals:
    void chartStitchChanged();
    void chartColorChanged();
    
public slots:
    void zoomIn();
    void zoomOut();

    void stitchChanged(QString oldSt, QString newSt);
    void colorChanged(QString oldColor, QString newColor);

    CrochetScene* scene() { return mScene; }

    QUndoStack* undoStack();

private:   
    QPointer<ChartView> mView;
    CrochetScene *mScene;
    QPointer<CrochetTextView> mTextView;

    QMap<QString, int> *mPatternStitches;
    QMap<QString, QMap<QString, qint64> > *mPatternColors;
    
    QString mName;
};

#endif // CHARTTAB_H
