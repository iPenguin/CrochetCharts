#include "stitchset.h"

#include <QFile>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include <QDebug>

StitchSet::StitchSet()
{
}

StitchSet::~StitchSet()
{
    foreach(Stitch *s, mStitches)
        delete s;
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

    mStitches.append(s);
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
    mStitches.append(s);
}

Stitch* StitchSet::stitch(QModelIndex index) const
{
    if(!index.isValid())
        return 0;

    

    return 0;
}

QVariant StitchSet::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DecorationRole) {
        return QIcon(QPixmap(stitch(index)->file()));
    } else if( role == Qt::UserRole) {
        return "Custom user role";
    }

    //fall back to the default implementation
    return QStandardItemModel::data(index, role);
}
