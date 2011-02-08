/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchlibrarydelegate.h"

#include "stitch.h"
#include <QPainter>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>

#include <QStyleOption>
#include <QStyleOptionViewItem>

#include <math.h>

#include <QApplication>

#include <QDebug>
#include "stitchcollection.h"
#include "stitchset.h"
#include <qsvgrenderer.h>

StitchLibraryDelegate::StitchLibraryDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
    mSignalMapper = new QSignalMapper(this);   
    connect(mSignalMapper, SIGNAL(mapped(int)), this, SIGNAL(addStitchToMasterSet(int)));
}

void StitchLibraryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;

    QStyleOption opt = option;
    QString buttonText;

    if(index.column() == 5) //FIXME: don't hard code the text here.
        buttonText = tr("Add Stitch");
    else
        buttonText = index.data(Qt::DisplayRole).toString();
    
    int width = option.fontMetrics.width(buttonText);
    int height = option.fontMetrics.height();
    int borderW = ceil((option.rect.width() - width) / 2.0);
    int borderH = ceil((option.rect.height() - height) / 4.0);

    if (index.column() == 1) {
        if(option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());
        
        StitchSet *set = static_cast<StitchSet*>((QAbstractItemModel*)index.model());
        QString st = set->index(index.row(), Stitch::Name).data(Qt::DisplayRole).toString();
        
        Stitch *s = set->findStitch(st);
        QRectF rect = QRectF((qreal)option.rect.x(), (qreal)option.rect.y(), (qreal)option.rect.width(), (qreal)option.rect.height());
        if(s->isSvg()) {
            s->renderSvg()->render(painter, rect);
        } else
            painter->drawPixmap(option.rect, *(s->renderPixmap()));
        
    } else if (index.column() == 3 || index.column() == 4) {

        if(option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());
        
        //FIXME: QStyle::PE_IndicatorButtonDropDown causes a crash.
        qApp->style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, painter);
        painter->drawText(option.rect.x() + 6, option.rect.y() + (borderH + height), buttonText);
        
    } else if(index.column() == 5) {
        if(option.state & QStyle::State_Selected)
            painter->fillRect(option.rect, option.palette.highlight());
        
        qApp->style()->drawPrimitive(QStyle::PE_PanelButtonCommand, &opt, painter);

        painter->drawText(option.rect.x() + borderW, option.rect.y() + (borderH + height), buttonText);
    } else {
        //fall back to the basic painter.
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize StitchLibraryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int padding = 0;
    
    if(!index.isValid())
        return QSize(100, 32);

    Stitch *s = static_cast<Stitch*>(index.internalPointer());
    if(!s)
        return QSize(100, 32);

    QString text;

    switch(index.column()) {
        case Stitch::Name:
            text = s->name();
            padding += 50;
            break;
        case Stitch::Icon: {
            StitchSet *set = static_cast<StitchSet*>((QAbstractItemModel*)index.model());
            QString st = set->index(index.row(), Stitch::Name).data(Qt::DisplayRole).toString();
            
            Stitch *s = set->findStitch(st);
            QSize retSize;
            
            if(s->isSvg())
                retSize = s->renderSvg()->defaultSize();
            else
                retSize = s->renderPixmap()->size();
            return retSize;
        }
        case Stitch::Description:
            padding +=150;
            text = s->description();
            break;
        case Stitch::Category:
            padding += 50;
            text = s->category();
            break;
        case Stitch::WrongSide:
            padding +=50;
            text = s->wrongSide();
            break;
        case 5:
            padding += 50;
            text = tr("Add Stitch"); //TODO: there's a button to estimate too.
            break;
        default:
            text = "";
            break;
    }
    
    QSize hint = option.fontMetrics.size(Qt::TextWordWrap, text);
    hint.setWidth(hint.width() + padding);

    //HACK: make the height of the icon the height of the whole row.
    StitchSet *set = static_cast<StitchSet*>((QAbstractItemModel*)index.model());
    QSize sizeH = sizeHint(option, set->index(index.row(), Stitch::Icon));
    hint.setHeight(sizeH.height());

    return hint;
}

QWidget* StitchLibraryDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    if(!index.isValid())
        return new QWidget(parent);
    
    switch(index.column()) {
        case Stitch::Name:{ //TODO: add a validator that checks if the name already exists.
            QLineEdit *editor = new QLineEdit(parent);
            
            editor->setText(index.data(Qt::EditRole).toString());
            return editor;
        }
        case Stitch::Icon: {
            
            return new QWidget(parent); //TODO: create an editor widget for selecting icons.
        }
        case Stitch::Description: {
            QLineEdit *editor = new QLineEdit(parent);

            editor->setText(index.data(Qt::EditRole).toString());
            return editor;
        }
        case Stitch::Category: {
            QComboBox *cb = new QComboBox(parent);
            cb->addItems(StitchCollection::inst()->categoryList());
            cb->setCurrentIndex(cb->findText(index.data(Qt::EditRole).toString()));
            return cb;
        }
        case Stitch::WrongSide: {
            QComboBox *cb = new QComboBox(parent);
            cb->addItems(StitchCollection::inst()->stitchList());
            cb->setCurrentIndex(cb->findText(index.data(Qt::EditRole).toString()));
            return cb;
        }
        case 5: {
            QPushButton *pb = new QPushButton(parent);
            pb->setText(tr("Add Stitch"));
            mSignalMapper->setMapping(pb, index.row());
            connect(pb, SIGNAL(clicked(bool)), mSignalMapper, SLOT(map()));
            return pb;
        }
        default:
            return new QWidget(parent);
    }

    return new QWidget(parent);
}

void StitchLibraryDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(!index.isValid())
        return;

    switch(index.column()) {
        case Stitch::Name: {
            QLineEdit *le = static_cast<QLineEdit*>(editor);
            le->setText(index.data(Qt::EditRole).toString());
            break;
        }
        case Stitch::Icon: {
            //TODO: custom widget for icon editing.
            break;
        }
        case Stitch::Description: {
            QLineEdit *le = static_cast<QLineEdit*>(editor);
            le->setText(index.data(Qt::EditRole).toString());
            break;
        }
        case Stitch::Category: {
            QComboBox *cb = static_cast<QComboBox*>(editor);
            cb->setCurrentIndex(cb->findText(index.data(Qt::EditRole).toString()));
            break;
        }
        case Stitch::WrongSide: {
            QComboBox *cb = static_cast<QComboBox*>(editor);
            cb->setCurrentIndex(cb->findText(index.data(Qt::EditRole).toString()));
            break;
        }
        default:
            break;
    }
}

void StitchLibraryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    switch(index.column()) {
        case Stitch::Icon: {
            //TODO: custom editor widget and data.
            break;
        }
        case Stitch::Name: //TODO: make the name check if it already exists before accepting any alterations.
        case Stitch::Description: {
            QLineEdit *le = static_cast<QLineEdit*>(editor);
            model->setData(index, le->text(), Qt::EditRole);
            break;
        }
        case Stitch::WrongSide:
        case Stitch::Category: {
            QComboBox *cb = static_cast<QComboBox*>(editor);
            model->setData(index, cb->currentText(), Qt::EditRole);
            break;
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
/*
void StitchLibraryDelegate::addStitchToMasterSet(int row)
{
    qDebug() << "row: " << row;
    
}
*/
