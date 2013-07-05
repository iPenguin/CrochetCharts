#ifndef COLORLABEL_H
#define COLORLABEL_H

#include <QLabel>

class ColorLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ColorLabel(QWidget *parent = 0);
    
    void setColor(QColor c);
    void setText(const QString &text);

    void mouseReleaseEvent(QMouseEvent *ev);

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

    QColor color() { return mColor; }

signals:
    void colorChanged(QColor c);

protected:
    void updateColor();
    void selectColor();

    void paintEvent(QPaintEvent *event);

private:
    QColor mColor;
};

#endif // COLORLABEL_H
