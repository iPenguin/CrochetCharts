#ifndef ALIGNDOCK_H
#define ALIGNDOCK_H

#include <QDockWidget>

namespace Ui {
    class AlignDock;
}

class AlignDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit AlignDock(QWidget *parent = 0);
    ~AlignDock();

private:
    Ui::AlignDock *ui;
};

#endif // ALIGNDOCK_H
