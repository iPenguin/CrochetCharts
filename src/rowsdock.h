#ifndef ROWSDOCK_H
#define ROWSDOCK_H

#include <QDockWidget>
#include <QButtonGroup>

namespace Ui {
    class RowsDock;
}

class RowsDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit RowsDock(QWidget *parent = 0);
    ~RowsDock();

signals:
    void arrangeGrid(QSize grid, QSize alignment, QSize spacing);
    
private slots:
    void generateArrangement();

private:
    Ui::RowsDock *ui;

    QButtonGroup horizonalGroup,
                 verticalGroup;
};

#endif // ROWSDOCK_H
