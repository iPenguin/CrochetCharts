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

signals:
    /**
     * alignment:
     * 1 = left, 2 = center v, 3 = right
     * 4 = top,  5 = center h, 6 = bottom
     */
    void align(int alignment);

    /**
     * distribution:
     * 1 = left, 2 = center h, 3 = right
     * 4 = top,  5 = center v, 6 = bottom
     */
    void distribute(int distribution);

private slots:
    void generateAlignment();
    void generateDistribution();
    
private:
    Ui::AlignDock *ui;

    
};

#endif // ALIGNDOCK_H
