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

class QGraphicsView;
class CrochetScene;
class CrochetTextView;
class QUndoStack;

namespace Ui {
    class OptionsBar;
}

class CrochetTab : public QWidget
{
    Q_OBJECT
    friend class SaveFile;
public:
    explicit CrochetTab(int defEditMode, QString defStitch, QColor defFgColor, QColor defBgColor, QWidget *parent = 0);
    ~CrochetTab();

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

    QUndoStack* undoStack();

    void createChart(int rows, int cols, QString defStitch);

    void copyInstructions();
    
private:
    CrochetScene* scene() { return mScene; }
    
private:   
    QPointer<ChartView> mView;
    CrochetScene *mScene;
    QPointer<CrochetTextView> mTextView;

    QMap<QString, int> *mPatternStitches;
    QMap<QString, QMap<QString, qint64> > *mPatternColors;
    
    QString mName;

    Ui::OptionsBar *ui;
};

#endif // CROCHETTAB_H
