/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchset.h"

#include <QFile>

#include <QXmlStreamWriter> //write the file

#include <QDomDocument> //read the file
#include <QDomElement>
#include <QDomNode>

#include "settings.h"

#include <QDebug>
#include <QFileInfo>

StitchSet::StitchSet(QObject *parent, bool isMasterSet, bool isBuiltIn)
    : QAbstractItemModel(parent), isMasterSet(isMasterSet),
    isBuiltInSet(isBuiltIn), isTemporary(false)
{
}

StitchSet::~StitchSet()
{
//TODO: delete stitches?
}

QString StitchSet::stitchSetFolder()
{
    QFileInfo file(stitchSetFileName);
    return file.baseName();
}

bool StitchSet::loadXmlFile(QString fileName)
{
    stitchSetFileName = fileName;
    
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open the file for reading" << fileName;
        //TODO: Add a nice error message.
        return false;
    }
    
    QDomDocument doc("stitchset");
    
    if (!doc.setContent(&file)) {
        qWarning() << "could not get contents of file";
        file.close();
        return false;
    }
    file.close();
    
    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomElement docElem = doc.documentElement();

    loadXmlStitchSet(&docElem);
    return true;
}

void StitchSet::loadXmlStitchSet(QDomElement *element)
{    
    QDomNode n = element->firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {

            if(e.tagName() == "name")
                setName(e.text());
            else if(e.tagName() == "author")
                setAuthor(e.text());
            else if(e.tagName() == "email")
                setEmail(e.text());
            else if(e.tagName() == "org")
                setOrg(e.text());
            else if(e.tagName() == "url")
                setUrl(e.text());
            else if(e.tagName() == "stitch")
                loadXmlStitch(e);
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

void StitchSet::saveXmlFile(QString fileName)
{
    if(fileName.isEmpty())
        fileName = stitchSetFileName;
    if(fileName.isEmpty())
       return;
    
    QString *data = new QString();
    
    QXmlStreamWriter stream(data);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    saveXmlStitchSet(&stream);

    stream.writeEndDocument();
    
    QFile file(fileName);
    
    if(!file.open(QIODevice::WriteOnly)) {
        //TODO: some nice dialog to warn the user.
        qWarning() << "Couldn't open file for writing..." << fileName;
        return;
    }
    
    file.write(data->toLatin1());
    
    delete data;
    data = 0;
}

void StitchSet::saveXmlStitchSet(QXmlStreamWriter *stream)
{
    QString fName = Settings::inst()->value("firstName").toString();
    QString lName = Settings::inst()->value("lastName").toString();
    QString email = Settings::inst()->value("email").toString();
    
    //TODO: use the settings as the default values, but allow the user to override those settings.
    stream->writeStartElement("stitch_set");
    stream->writeTextElement("name", "Master Stitch Set");
    stream->writeTextElement("author", fName + " " + lName);
    stream->writeTextElement("email", email);
    stream->writeTextElement("org", "");
    stream->writeTextElement("url", "");
    
    foreach(Stitch *s, mStitches) {
        stream->writeStartElement("stitch");
        
        stream->writeTextElement("name", s->name());
        stream->writeTextElement("icon", s->file());
        stream->writeTextElement("description", s->description());
        stream->writeTextElement("category", s->category());
        stream->writeTextElement("ws", s->wrongSide());
        
        stream->writeEndElement(); //stitch
    }
    
    stream->writeEndElement(); // stitch_set
    
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
    if(!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags f =  Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if(!isBuiltInSet || index.column() == 5)
        f |= Qt::ItemIsEditable;
    
    if(index.column() == 5)
        return f;// | Qt::ItemIsUserCheckable;
    else
        return f;
}

QVariant StitchSet::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal) {
            switch(section) {
                case Stitch::Name:
                    return QVariant(tr("Name"));
                case Stitch::Icon:
                    return QVariant(tr("Icon"));
                case Stitch::Description:
                    return QVariant(tr("Description"));
                case Stitch::Category:
                    return QVariant(tr("Category"));
                case Stitch::WrongSide:
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

    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        switch(index.column()) {
            case Stitch::Name:
                return QVariant(s->name());
            case Stitch::Icon:
                return QVariant(s->file()); //TODO: return QIcon for display role and filename for edit role.
            case Stitch::Description:
                return QVariant(s->description());
            case Stitch::Category:
                return QVariant(s->category());
            case Stitch::WrongSide:
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

    if(role == Qt::EditRole || role == Qt::DisplayRole) {
        Stitch *s = static_cast<Stitch*>(index.internalPointer());
        
        bool retVal = false;

        switch(index.column()) {
            case Stitch::Name:
                s->setName(value.toString());
                retVal = true;
                break;
            case Stitch::Icon:
                s->setFile(value.toString());
                retVal = true;
                break;
            case Stitch::Description:
                s->setDescription(value.toString());
                retVal = true;
                break;
            case Stitch::Category:
                s->setCategory(value.toString());
                retVal = true;
                break;
            case Stitch::WrongSide:
                s->setWrongSide(value.toString());
                retVal = true;
                break;
            default:
                retVal = false;
        }

        if(retVal)
            emit dataChanged(index, index);
        return retVal;

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
    Q_UNUSED(index);
    return QModelIndex(); //This is not a tree it doesn't have a parent.
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
    if(isMasterSet)
        return 5;
    else
        return 6;
}

void StitchSet::clearStitches()
{
    mStitches.clear();
}

void StitchSet::cloneStitchSet(StitchSet *orig)
{
    if(this == orig) // cannot clone yourself.
        return;

    mStitches.clear(); //TODO: make sure these are being deleted too...
    
    foreach(Stitch *s, orig->stitches()) {
        Stitch *newS = new Stitch();
        *newS << *s;
        addStitch(newS);
    }
    reset();
}
