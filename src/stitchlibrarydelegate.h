/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef STITCHLIBRARYDELEGATE_H
#define STITCHLIBRARYDELEGATE_H

#include <QStyledItemDelegate>
#include <QSignalMapper>
#include <qcoreevent.h>

#include <QComboBox>
#include <QAbstractItemView>

class IconComboBox : public QComboBox
{
    Q_OBJECT
public:
    IconComboBox(QWidget *parent = 0)
        : QComboBox(parent)
    {
    }

    void showPopup()
    {
        view()->setMinimumWidth(200);
        QComboBox::showPopup();
    }

    bool event(QEvent *event)
    {
        //Eat wheel Events if the list isn't showing.
        if(event->type() == QEvent::Wheel) {
            if(!view()->isVisible()) {
                return false;
            }
        }

        return QComboBox::event(event);
    }
};

class StitchLibraryDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    StitchLibraryDelegate(QWidget *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

private:
    void loadIcons(QComboBox *cb) const;

};

#endif //STITCHLIBRARYDELEGATE_H
