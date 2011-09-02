#ifndef ALIGNANDDISTRIBUTE_H
#define ALIGNANDDISTRIBUTE_H

#include <QWidget>

namespace Ui {
    class AlignAndDistribute;
}

class AlignAndDistribute : public QWidget
{
    Q_OBJECT

public:
    explicit AlignAndDistribute(QWidget *parent = 0);
    ~AlignAndDistribute();

private:
    Ui::AlignAndDistribute *ui;
};

#endif // ALIGNANDDISTRIBUTE_H
