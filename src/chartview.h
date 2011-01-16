/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QGraphicsView>

class ChartView : public QGraphicsView
{
    Q_OBJECT
public:
    ChartView();
    ~ChartView();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void scrollContentsBy(int dx, int dy);
    void wheelEvent(QWheelEvent *event);
    
private:

};
#endif //CHARTVIEW_H