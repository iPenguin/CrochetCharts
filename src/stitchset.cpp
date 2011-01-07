/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchset.h"

#include <QFile>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include <QDebug>

StitchSet::StitchSet(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new Stitch();
}

StitchSet::StitchSet(QString name, QObject *parent)
    : QAbstractItemModel(parent)
{
    setName(name);
    rootItem = new Stitch();
}

StitchSet::~StitchSet()
{
//TODO: delete stitches?
}

void StitchSet::loadXmlStitchSet(QString fileName)
{    
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open the file for reading" << fileName;
        //TODO: Add a nice error message.
        return;
    }

    QDomDocument doc("stitchset");

    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {

            if(e.tagName() == "name")
                this->setName(e.text());
            else if(e.tagName() == "author")
                this->setAuthor(e.text());
            else if(e.tagName() == "email")
                this->setEmail(e.text());
            else if(e.tagName() == "org")
                this->setOrg(e.text());
            else if(e.tagName() == "url")
                this->setUrl(e.text());
            else if(e.tagName() == "stitch")
                this->loadXmlStitch(e);
            else
                 qWarning() << "Could not load part of the stitch set:" << e.tagName() << e.text();
        }
        n = n.nextSibling();
    }
}

void StitchSet::loadXmlStitch(QDomElement element)
{
    Stitch *s = new Stitch();

    QDomNode n = element.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            if(e.tagName() == "name")
                s->setName(e.text());
            else if(e.tagName() == "icon")
                s->setFile(e.text());
            else if(e.tagName() == "description")
                s->setDescription(e.text());
            else if(e.tagName() == "category")
                s->setCategory(e.text());
            else if(e.tagName() == "ws")
                s->setWrongSide(e.text());
            else
                qWarning() << "Cannot load unknown stitch property:" << e.tagName() << e.text();
        }
        n = n.nextSibling();
    }
    addStitch(s);
}

Stitch* StitchSet::findStitch(QString name)
{
    foreach(Stitch *s, mStitches) {
        if(s->name() == name)
            return s;
    }

    return 0;
}

bool StitchSet::hasStitch(QString name)
{
    Stitch *s = this->findStitch(name);
    if(s)
        return true;
    else
        return false;
}

void StitchSet::addStitch(Stitch *s, Stitch *parent)
{
    if(parent == 0)
        parent = rootItem;
    QModelIndex parentIdx = createIndex(parent->row(), 0, parent);

    beginInsertRows(parentIdx, parent->childCount(), parent->childCount());
    parent->appendChild(s);
    mStitches.append(s);
    endInsertRows();
}

QVariant StitchSet::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    Stitch *s = static_cast<Stitch *>(index.internalPointer());

    if(!s) {
        qDebug() << "data cannot get valid stitch";
        return QVariant();
    }
    /*if(role == Qt::DisplayRole) {
        return QVariant(s->name());
    } else if(role == Qt::DecorationRole) {
        return QIcon(QPixmap(stitch(index)->file()));
    }*/

    return s->data(index.column());
}

QModelIndex StitchSet::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    Stitch *parentItem;

    if(!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<Stitch*>(parent.internalPointer());

    Stitch *childItem = parentItem->child(row);

    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();

}

QModelIndex StitchSet::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    Stitch *childItem = static_cast<Stitch*>(index.internalPointer());
    Stitch *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int StitchSet::stitchCount()
{
    return rowCount(rootItemIndex());
}

int StitchSet::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return 0;

    Stitch *s = static_cast<Stitch*>(parent.internalPointer());
    if(s)
        return s->childCount();
    return 0;
}

int StitchSet::columnCount(const QModelIndex &/*parent*/) const
{
    return 5;
}

QDataStream& operator<<(QDataStream &out, const StitchSet &set)
{
    return out;
}

QDataStream& operator>>(QDataStream &in, StitchSet &set)
{
    return in;
}
