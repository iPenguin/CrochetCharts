/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CHARTTAB_H
#define CHARTTAB_H

#include <QWidget>

#include "chartview.h"

class QGraphicsView;
class CrochetScene;

class ChartTab : public QWidget
{
    Q_OBJECT
    friend class SaveFile;
public:
    explicit ChartTab(QWidget *parent = 0);

    QString name() { return mName; }
    void setName(QString n) { mName = n; }

    void savePdf(QPrinter printer, QString fileName, int resolution = 300);
    void saveSvg(QString fileName);
    void saveImage(QString fileName, QSize size, int resolution = 96);

signals:

public slots:
    void zoomIn();
    void zoomOut();

protected:
    CrochetScene* scene() { return mScene; }

private:
    ChartView *mView;
    CrochetScene *mScene;

    QString mName;
};

#endif // CHARTTAB_H
