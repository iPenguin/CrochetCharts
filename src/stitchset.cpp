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

void StitchSet::addStitch(Stitch *s)
{
    beginInsertRows(this->parent(QModelIndex()), stitchCount(), stitchCount());
    mStitches.append(s);
    endInsertRows();
}

Qt::ItemFlags StitchSet::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    //Qt::ItemIsEditable | Qt::ItemIsUserCheckable
    return (Qt::ItemIsSelectable); // | Qt::ItemIsEnabled);
}

QVariant StitchSet::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal) {
            switch(section) {
                case 0:
                    return QVariant(tr("Name"));
                case 1:
                    return QVariant(tr("Icon"));
                case 2:
                    return QVariant(tr("Description"));
                case 3:
                    return QVariant(tr("Category"));
                case 4:
                    return QVariant(tr("Wrong Side"));
                default:
                    return QVariant();
            }

        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant StitchSet::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    Stitch *s = static_cast<Stitch*>(index.internalPointer());

    if(role == Qt::DisplayRole) {
        switch(index.column()) {
            case 0:
                return QVariant(s->name());
            case 1:
                return QVariant(s->file()); //TODO: return the QIcon.
            case 2:
                return QVariant(s->description());
            case 3:
                return QVariant(s->category());
            case 4:
                return QVariant(s->wrongSide());
            default:
                return QVariant();
        }
    }

    return QVariant();
}

bool StitchSet::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;
    
    if(role == Qt::EditRole) {
        Stitch *s = static_cast<Stitch*>(index.internalPointer());
        
        switch(index.column()) {
            case Stitch::Name:
                s->setName(value.toString());
                return true;
            case Stitch::Icon:
                s->setFile(value.toString());
                return true;
            case Stitch::Description:
                s->setDescription(value.toString());
                return true;
            case Stitch::Category:
                s->setCategory(value.toString());
                return true;
            case Stitch::WrongSide:
                s->setWrongSide(value.toString());
                return true;
            default:
                return false;
        }
    }

    return false;
}

QModelIndex StitchSet::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(row < 0 || column < 0)
        return QModelIndex();
    
    return createIndex(row, column, mStitches[row]);
}

QModelIndex StitchSet::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();
    
    return createIndex(0, 0, (quint32)95973);
}

int StitchSet::stitchCount() const
{
    return mStitches.count();
}

int StitchSet::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return stitchCount();
}

int StitchSet::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QDataStream& operator<<(QDataStream &out, const StitchSet &set)
{
    Q_UNUSED(set);
    return out;
}

QDataStream& operator>>(QDataStream &in, StitchSet &set)
{
    Q_UNUSED(set);
    return in;
}
