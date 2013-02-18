/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef STITCHPALETTEDELEGATE_H
#define STITCHPALETTEDELEGATE_H

#include <QStyledItemDelegate>

class StitchPaletteDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    
public:
    StitchPaletteDelegate(QWidget* parent = 0);

    void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif //STITCHPALETTEDELEGATE_H
