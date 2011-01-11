/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchpalettedelegate.h"

#include "stitch.h"
#include <QPainter>
#include <QPixmap>

#include <QDebug>

StitchPaletteDelegate::StitchPaletteDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

void StitchPaletteDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //fall back to the basic painter.
    //QStyledItemDelegate::paint(painter, option, index);

    if(index.column() == 0) {
        int pad = 5;
        int iconWidth = 32;
        int iconHeight = 32;

        Stitch *s = static_cast<Stitch*>(index.internalPointer());
        QRect rect = option.rect;

        if(option.state == QStyle::State_Selected)
            painter->fillRect(rect, option.palette.highlight());
        
        //TODO: move image loading/caching into the stitch class.
        QPixmap pix = QPixmap(iconWidth, iconHeight);
        pix.load(":/stitches/chain.svg");
        painter->drawPixmap(rect.left() + pad, rect.top() + pad, pix);
        painter->drawText(rect.left() + iconWidth + (2*pad), rect.top() + pad, s->name());
       
    }
}

QSize StitchPaletteDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
/*
    if(!index.isValid())
        return QSize(100, 10);
*/
    Stitch *s = static_cast<Stitch*>(index.internalPointer());
    if(!s)
        return QSize(100, 42);

    QString text;

    switch(index.column()) {
        case 0:
            text = s->name();
            break;
        case 1:
            text = s->file(); //TODO: make this the icon. 32x32.
            break;
        case 2:
            text = s->description();
            break;
        case 3:
            text = s->category();
            break;
        case 4:
            text = s->wrongSide();
            break;
        default:
            text = "";
            break;
    }
    QSize textSize = option.fontMetrics.size(Qt::TextWordWrap, text);
    
    return QSize(textSize.width(), 42);
}

QWidget* StitchPaletteDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void StitchPaletteDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Q_UNUSED(editor);
    Q_UNUSED(index);
}

void StitchPaletteDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    Q_UNUSED(editor);
    Q_UNUSED(model);
    Q_UNUSED(index);
}
