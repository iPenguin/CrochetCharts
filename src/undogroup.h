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
#ifndef UNDOGROUP_H
#define UNDOGROUP_H

#include <QUndoGroup>

class UndoGroup : public QUndoGroup
{
    Q_OBJECT
public:
    UndoGroup(QObject* parent = 0);

    void addStack(QUndoStack* stack);
    
signals:
    void isModified(bool clean);
    
private slots:
    void checkAllCleanStates();

};
#endif //UNDOGROUP_H
