#ifndef CROCHETSCENE_H
#define CROCHETSCENE_H

#include <QGraphicsScene>

class CrochetScene : public QGraphicsScene
{
    Q_OBJECT
public:
    CrochetScene(QObject *parent = 0);
/*
    void setCell(int row, int column, Stitch *s);
    void insertCell(int row, int columnBefore, Stitch *s);

*/

};

#endif //CROCHETSCENE_H
