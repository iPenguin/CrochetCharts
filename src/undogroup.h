/*************************************************\
| Copyrigth (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef UNDOGROUP_H
#define UNDOGROUP_H

#include <QUndoGroup>

class UndoGroup : public QUndoGroup
{
    Q_OBJECT
public:
    UndoGroup(QObject *parent = 0);

signals:
    void documentCleanChanged(bool clean);
    
private slots:
    void checkAllCleanStates();

private:
    bool mClean;
};
#endif //UNDOGROUP_H
