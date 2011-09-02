#ifndef ROWSANDCOLUMNS_H
#define ROWSANDCOLUMNS_H

#include <QWidget>

namespace Ui {
    class RowsAndColumns;
}

class RowsAndColumns : public QWidget
{
    Q_OBJECT

public:
    explicit RowsAndColumns(QWidget *parent = 0);
    ~RowsAndColumns();

private:
    Ui::RowsAndColumns *ui;
};

#endif // ROWSANDCOLUMNS_H
