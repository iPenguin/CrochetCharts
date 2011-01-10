/*************************************************\
| (c) 2010-2011 Stitch Works Software             |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchmodel.h"
#include <QFile>

#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>

#include <QDebug>

StitchModel::StitchModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

StitchModel::~StitchModel()
{
}

QModelIndex StitchModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(row < 0 || column < 0)
        return QModelIndex();
    
    return createIndex(row, column, (quint32)95973);
}

QModelIndex StitchModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();

    return createIndex(0, 0, (quint32)95973);
}

Qt::ItemFlags StitchModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    //Qt::ItemIsEditable | Qt::ItemIsUserCheckable
    return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

QVariant StitchModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    qDebug() << "StitchModel::data() << start" <<index << role;
    
    if(role == Qt::DisplayRole) {
        Stitch *s = mStitches[index.row()];
        return QVariant(s->name());
    }

    return QVariant();
}

QVariant StitchModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);

    return QVariant();
}

int StitchModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mStitches.count();
}

int StitchModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

void StitchModel::addStitch(Stitch* s)
{  
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    mStitches.append(s);
    endInsertRows();
}

void StitchModel::loadXmlStitchSet(QString fileName)
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
/*
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
            else
*/
            if(e.tagName() == "stitch")
                this->loadXmlStitch(e);
            else
                qWarning() << "Could not load part of the stitch set:" << e.tagName() << e.text();
        }
        n = n.nextSibling();
    }
}

void StitchModel::loadXmlStitch(QDomElement element)
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
