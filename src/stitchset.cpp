#include "stitchset.h"

#include <QFile>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

#include <QXmlStreamWriter>

#include "settings.h"

#include <QDebug>

StitchSet::StitchSet()
{
    qDebug() << "create a stitchset";
}

StitchSet::~StitchSet()
{
    qDebug() << "~StitchSet()";
    //FIXME: dont hard code file name
    this->saveXmlStitchSet("/home/brian/stitches.xml");
}

void StitchSet::loadXmlStitchSet(QString fileName)
{    
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open the file:" << fileName;
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
                qDebug() << "Could not load part of the stitch set:" << e.tagName() << e.text();
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
                qDebug() << "Cannot load unknown stitch property:" << e.tagName() << e.text();
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

void StitchSet::saveXmlStitchSet(QString fileName)
{

    QString *data = new QString();

    QXmlStreamWriter stream(data);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    QString fName = Settings::inst()->value("firstName").toString();
    QString lName = Settings::inst()->value("lastName").toString();
    QString email = Settings::inst()->value("email").toString();

    //TODO: figure out all the pieces or remove them...
    stream.writeStartElement("stitch_set");
    stream.writeTextElement("name", "User Overlay");
    stream.writeTextElement("author", fName + " " + lName);
    stream.writeTextElement("email", email);
    stream.writeTextElement("org", "");
    stream.writeTextElement("url", "");

    foreach(Stitch *s, mStitches) {
        stream.writeStartElement("stitch");

        stream.writeTextElement("name", s->name());
        stream.writeTextElement("icon", s->file());
        stream.writeTextElement("description", s->description());
        stream.writeTextElement("category", s->category());
        stream.writeTextElement("ws", s->wrongSide());

        stream.writeEndElement(); //stitch
    }

    stream.writeEndElement(); // stitch_set

    stream.writeEndDocument();

    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly)) {
    //TODO: some nice dialog to warn the user.
        qDebug() << "couldn't open file for writing...";
        return;
    }

    file.write(data->toLatin1());

    delete data;
    data = 0;
}

void StitchSet::saveXmlStitches(QXmlStreamWriter stream)
{


}
