/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchlibrarydelegate.h"

#include "stitch.h"
#include <QPainter>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include <QDebug>
#include "stitchcollection.h"

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
        case Stitch::Name:
            text = s->name();
            break;
        case Stitch::Icon:
            return QSize(64, 64); //TODO: get this from the stitch class.
        case Stitch::Description:
            text = s->description();
            break;
        case Stitch::Category:
            text = s->category();
            break;
        case Stitch::WrongSide:
            text = s->wrongSide();
            break;
        case 5:
            text = tr("Add Stitch");
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
        case Stitch::Icon: {
            
            return new QWidget(parent); //TODO: create an editor widget for selecting icons.
        }
        case Stitch::Description: {
            QLineEdit *editor = new QLineEdit(parent);

            editor->setText(s->name());
            return editor;
        }
        case Stitch::Category: {
            QComboBox *cb = new QComboBox(parent);
            cb->addItems(StitchCollection::inst()->categoryList());
            return cb;
        }
        case Stitch::WrongSide: {
            QComboBox *cb = new QComboBox(parent);
            cb->addItems(StitchCollection::inst()->stitchList());
            return cb;
        }
        case 5: {
            QPushButton *pb = new QPushButton(parent);
            pb->setText(tr("Add Stitch"));
            pb->setObjectName(QString("%1 %2").arg(index.row()).arg(index.column()));
            connect(pb, SIGNAL(clicked(bool)), this, SLOT(addStitch()));
            return pb;
        }
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
        case Stitch::Icon: {
            //TODO: custom editor widget and data.
        }
        case Stitch::Name:
        case Stitch::Description: {
            QLineEdit *le = static_cast<QLineEdit*>(editor);
            model->setData(index, le->text(), Qt::EditRole); //TODO: consolidate with Stitch::Name ?
        }
        case Stitch::WrongSide:
        case Stitch::Category: {
            QComboBox *cb = static_cast<QComboBox*>(editor);
            model->setData(index, cb->currentText(), Qt::EditRole);
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

void StitchLibraryDelegate::addStitch()
{
    QPushButton *pb = qobject_cast<QPushButton*>(sender());
    int row, column;
    QStringList numbers = pb->objectName().split(" ");
    row = numbers.first().toInt();
    column = numbers.last().toInt();

}
