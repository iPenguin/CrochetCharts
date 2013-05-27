#ifndef COLORLISTWIDGET_H
#define COLORLISTWIDGET_H

#include <QListWidget>

class ColorListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ColorListWidget(QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *e);

    static QPixmap drawColorBox(QColor color, QSize size);

signals:
    void bgColorSelected(QModelIndex index);

public slots:

private:
    QPointF mDragStart;
};

#endif // COLORLISTWIDGET_H
