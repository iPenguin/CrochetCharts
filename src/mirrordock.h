#ifndef MIRRORDOCK_H
#define MIRRORDOCK_H

#include <QDockWidget>

namespace Ui {
    class MirrorDock;
}

class MirrorDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit MirrorDock(QWidget *parent = 0);
    ~MirrorDock();

signals:
    void mirror(int direction);
    void rotate(qreal degrees);

private slots:
    void rotateCustom();

    void genRotate();
    void genMirror();

private:
    Ui::MirrorDock *ui;
};

#endif // MIRRORDOCK_H
