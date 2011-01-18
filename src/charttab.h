/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CHARTTAB_H
#define CHARTTAB_H

#include <QWidget>

#include "chartview.h"

class QXmlStreamWriter;
class QXmlStreamReader;
class QGraphicsView;
class CrochetScene;

class ChartTab : public QWidget
{
    Q_OBJECT
public:
    explicit ChartTab(QWidget *parent = 0);

    QString name() { return mName; }
    void setName(QString n) { mName = n; }

    void savePdf(QPrinter printer, QString fileName, int resolution = 300);
    void saveSvg(QString fileName);
    void saveImage(QString fileName, QSize size, int resolution = 96);

    bool save(QXmlStreamWriter *stream);
    bool load(QXmlStreamReader *stream);
    
signals:

public slots:
    void zoomIn();
    void zoomOut();

private:
    ChartView *mView;
    CrochetScene *mScene;

    QString mName;
};

#endif // CHARTTAB_H
