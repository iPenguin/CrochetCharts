/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef TABINTERFACE_H
#define TABINTERFACE_H

#include <QWidget>
#include <QMap>

class QGraphicsView;
class QGraphicsScene;
class CrochetTextView;
class QUndoStack;

class TabInterface : public QWidget
{
    Q_OBJECT
    friend class SaveFile;
public:
    
    virtual ~TabInterface() = 0;

    virtual void renderChart(QPainter* painter, QRectF rect = QRectF()) = 0;

    void setPatternStitches(QMap<QString, int>* stitches) { mPatternStitches = stitches; }
    void setPatternColors(QMap<QString, QMap<QString, qint64> >* colors) { mPatternColors = colors; }
    
    virtual QStringList editModes() = 0;

    virtual void setEditMode(int mode) = 0;
    virtual int editMode() const = 0;

    virtual void setEditFgColor(QColor color) = 0;
    virtual void setEditBgColor(QColor color) = 0;
    virtual void setEditStitch(QString stitch) = 0;

signals:
    void chartStitchChanged();
    void chartColorChanged();

public slots:
    virtual void zoomIn() = 0;
    virtual void zoomOut() = 0;

    virtual void stitchChanged(QString oldSt, QString newSt) = 0;
    virtual void colorChanged(QString oldColor, QString newColor) = 0;

    virtual QUndoStack* undoStack() = 0;

    virtual void createChart(int rows, int cols, QString defStitch) = 0;

    //void copyInstructions();

private:
    virtual QGraphicsScene* scene() = 0;

    QMap<QString, int>* mPatternStitches;
    QMap<QString, QMap<QString, qint64> >* mPatternColors;

};

#endif //TABINTERFACE_H
