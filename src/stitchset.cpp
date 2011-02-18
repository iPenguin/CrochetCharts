/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "stitchset.h"

#include <QFile>

#include <QXmlStreamWriter> //write the xml file

#include <QDomDocument> //read the xml file
#include <QDomElement>
#include <QDomNode>

#include <QDataStream> //read/write the set file w/icon data.

#include "settings.h"

#include <QDebug>
#include <QFileInfo>

#include <QMap>
#include "appinfo.h"

StitchSet::StitchSet(QObject *parent, bool isMasterSet, bool isBuiltIn)
    : QAbstractItemModel(parent), isMasterSet(isMasterSet),
    isBuiltInSet(isBuiltIn), isTemporary(false), mSaveFileVersion(StitchSet::Version_1_0_0)
{
}

StitchSet::~StitchSet()
{
    foreach(Stitch *s, mStitches) {
        mStitches.removeOne(s);
        delete s;
    }   
}

QString StitchSet::stitchSetFolder()
{
    QFileInfo file(stitchSetFileName);
    return file.path() + "/" + file.baseName() + "/";
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
    
    QDomElement docElem = doc.documentElement();
    
    loadXmlStitchSet(&docElem);
    return true;
}

void StitchSet::loadDataFile(QString fileName, QString dest)
{

    if(fileName.isEmpty())
        return;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    quint32 magicNumber;
    qint32 version;
    in >> magicNumber;

    if(magicNumber != AppInfo::magicNumberSet)
        return; //TODO: nice error message. not a set file.

    in >> version;
    
    if(version < StitchSet::Version_1_0_0)
        return; //TODO: unknown version.

    if(version > mSaveFileVersion)
        return; //TODO: unknown file version


    if(version == StitchSet::Version_1_0_0)
        in.setVersion(QDataStream::Qt_4_7);

    stitchSetFileName = dest;
    
    loadIcons(&in);

    QDomDocument doc("stitchset");

    QByteArray docData;
    
    in >> docData;

    if (!doc.setContent(QString(docData))) {
        qWarning() << "could not get contents of file";
        file.close();
        return;
    }
    file.close();
    
    QDomElement docElem = doc.documentElement();
    
    loadXmlStitchSet(&docElem, true);

}

void StitchSet::loadIcons(QDataStream *in)
{
    QMap<QString, QByteArray> icons;
    *in >> icons;
        
    foreach(QString key, icons.keys()) {
        qDebug() << "out file " << stitchSetFolder() + key;
        QFile f(stitchSetFolder() + key);
        f.open(QIODevice::WriteOnly);
        f.write(icons[key]);
        f.close();
    }
}

void StitchSet::loadXmlStitchSet(QDomElement *element, bool loadIcons)
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
                loadXmlStitch(&e, loadIcons);
            else
                 qWarning() << "Could not load part of the stitch set:" << e.tagName() << e.text();
        }
        n = n.nextSibling();
    }
}

void StitchSet::loadXmlStitch(QDomElement *element, bool loadIcon)
{
    Stitch *s = new Stitch();

    QDomNode n = element->firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(!e.isNull()) {
            if(e.tagName() == "name")
                s->setName(e.text());
            else if(e.tagName() == "icon") {
                if(loadIcon && !e.text().startsWith(":/"))
                    s->setFile(stitchSetFolder() + e.text());
                else
                    s->setFile(e.text());
            } else if(e.tagName() == "description")
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

void StitchSet::saveDataFile(QString fileName)
{
    if(fileName.isEmpty())
        return;
    
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QDataStream out(&file);

    out << AppInfo::magicNumberSet;
    out << (qint32)StitchSet::Version_1_0_0;
    out.setVersion(QDataStream::Qt_4_7);

    saveIcons(&out);

    QString *data = new QString();
    
    QXmlStreamWriter stream(data);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    
    saveXmlStitchSet(&stream, true);
    
    stream.writeEndDocument();

    out << data->toLatin1();
    
    file.close();
    delete data;
    data = 0;
    
}

void StitchSet::saveIcons(QDataStream *out)
{
    QMap<QString, QByteArray> icons;
    foreach(Stitch *s, mStitches) {
        if(!s->file().startsWith(":/")) {
            QFile f(s->file());
            f.open(QIODevice::ReadOnly);
            qDebug() << "file name" << QFileInfo(s->file()).fileName();
            icons.insert(QFileInfo(s->file()).fileName(), f.readAll());
            f.close();
        }
    }
    *out << icons;
}

void StitchSet::saveXmlStitchSet(QXmlStreamWriter *stream, bool saveIcons)
{
    stream->writeStartElement("stitch_set");
    stream->writeTextElement("name", name());
    stream->writeTextElement("author", author());
    stream->writeTextElement("email", email());
    stream->writeTextElement("org", org());
    stream->writeTextElement("url", url());
        
    foreach(Stitch *s, mStitches) {
        QString file;
        if(saveIcons && !s->file().startsWith(":/"))
            file = QFileInfo(s->file()).fileName();
        else
            file = s->file();
        
        stream->writeStartElement("stitch");
        
        stream->writeTextElement("name", s->name());
        stream->writeTextElement("icon", file);
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
    beginInsertRows(parent(QModelIndex()), stitchCount(), stitchCount());
    mStitches.append(s);
    endInsertRows();
}

void StitchSet::createStitch(QString name)
{
    Stitch *newS = new Stitch();
    newS->setName(name);
    newS->setFile(":/stitches/unknown.svg");
    addStitch(newS);
}

void StitchSet::removeStitch(QString name)
{
    Stitch *s = findStitch(name);
    int index = mStitches.indexOf(s);
    if(s) {
        beginRemoveRows(parent(QModelIndex()), index, index);
        mStitches.removeOne(s);
        endRemoveRows();
    }
}

Qt::ItemFlags StitchSet::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags f =  Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    
    if(index.column() != 5)
        f |= Qt::ItemIsEditable;

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
                case 5:
                    return QVariant(tr("Selected"));
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
                return QVariant(s->file());
            case Stitch::Description:
                return QVariant(s->description());
            case Stitch::Category:
                return QVariant(s->category());
            case Stitch::WrongSide:
                return QVariant(s->wrongSide());
            case 5:
                return QVariant(mSelected.contains(s));
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
            case Stitch::Name: {
                QString oldName = s->name();
                s->setName(value.toString());
                emit stitchNameChanged(name(), oldName, value.toString());
                retVal = true;
                break;
            }
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
            case 5: {
                if(value.toBool()) {
                    if(!mSelected.contains(s))
                        mSelected.append(s);
                } else {
                    if(mSelected.contains(s))
                        mSelected.removeOne(s);
                }
                retVal = false;
                break;
            }   
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
    return 6;
}

void StitchSet::clearStitches()
{
    mStitches.clear();
}
