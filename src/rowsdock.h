#ifndef ROWSDOCK_H
#define ROWSDOCK_H

#include <QDockWidget>

namespace Ui {
    class RowsDock;
}

class RowsDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit RowsDock(QWidget *parent = 0);
    ~RowsDock();

private:
    Ui::RowsDock *ui;
};

#endif // ROWSDOCK_H
