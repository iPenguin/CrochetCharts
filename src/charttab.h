/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CHARTTAB_H
#define CHARTTAB_H

#include <QWidget>

class QGraphicsView;
class CrochetScene;

class ChartTab : public QWidget
{
    Q_OBJECT
public:
    explicit ChartTab(QWidget *parent = 0);

    void savePdf(QPrinter printer, QString fileName, int resolution = 300);
    void saveSvg(QString fileName);
    void saveImage(QString fileName, QSize size, int resolution = 96);

signals:

public slots:


private:
    QGraphicsView *mView;
    CrochetScene *mScene;
};

#endif // CHARTTAB_H
