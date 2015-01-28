/****************************************************************************\
 Copyright (c) 2011-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
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
