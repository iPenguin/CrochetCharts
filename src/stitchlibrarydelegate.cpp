/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchlibrarydelegate.h"

#include "stitch.h"
#include <QPainter>
#include <QLineEdit>
#include <QComboBox>

#include <QDebug>

StitchLibraryDelegate::StitchLibraryDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
    qDebug() << "sld ctor";
}

void StitchLibraryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    qDebug() << "StitchLibraryDelegate::paint << start";
    //fall back to the basic painter.
    QStyledItemDelegate::paint(painter, option, index);
    //painter->drawText(QRectF(0,0,50,10), "Test");
    
}

QSize StitchLibraryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return QSize(100, 10);

    Stitch *s = static_cast<Stitch*>(index.internalPointer());
    if(!s)
        return QSize(100, 10);

    QString text;

    switch(index.column()) {
        case 0:
            text = s->name();
            break;
        case 1:
            return QSize(64, 64); //TODO: get this from the stitch class.
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

    return option.fontMetrics.size(Qt::TextWordWrap, text);
}

QWidget* StitchLibraryDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    if(!index.isValid())
        return new QWidget(parent);

    Stitch *s = static_cast<Stitch*>(index.internalPointer());
    
    switch(index.column()) {
        case Stitch::Name:
            return new QWidget(parent); //the name is the unique id and shouldn't be changed.
        case Stitch::Icon:
            return new QWidget(parent); //TODO: create an editor widget for selecting icons.
        case Stitch::Description: {
             QLineEdit *editor = new QLineEdit(parent);

            editor->setText(s->name());
            return editor;
        }
        case Stitch::Category:
            return new QComboBox(parent);
        case Stitch::WrongSide:
            return new QComboBox(parent);
        default:
            return new QWidget(parent);
    }

    return new QWidget(parent);
}

void StitchLibraryDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Q_UNUSED(editor);
    Q_UNUSED(index);

}

void StitchLibraryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    switch(index.column()) {
        case Stitch::Name: {
            QLineEdit *le = static_cast<QLineEdit*>(editor);
            model->setData(index, le->text(), Qt::EditRole);
        }
        case Stitch::Icon: {
            //TODO: custom editor widget and data.
        }
        case Stitch::Description: {
            QLineEdit *le = static_cast<QLineEdit*>(editor);
            model->setData(index, le->text(), Qt::EditRole); //TODO: consolidate with Stitch::Name ?
        }
        case Stitch::Category:{
            QComboBox *cb = static_cast<QComboBox*>(editor);
            model->setData(index, cb->currentText(), Qt::EditRole);
        }
        case Stitch::WrongSide: {
            QComboBox *cb = static_cast<QComboBox*>(editor);
            model->setData(index, cb->currentText(), Qt::EditRole); //TODO: consolidate with Stitch::Category ?
        }
        default:
            break;
    }
}

void StitchLibraryDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);

    editor->setGeometry(option.rect);
}
