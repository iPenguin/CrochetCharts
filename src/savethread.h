/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef SAVETHREAD_H
#define SAVETHREAD_H

#include <QThread>
#include <QtConcurrentRun>

#include "crochettab.h"
#include <QXmlStreamReader>

#include "stitchlibrary.h"

class SaveThread : public QThread
{
public:
    SaveThread(CrochetTab* t, QXmlStreamReader* s);
    ~SaveThread();

    void run();

private:
    QTransform loadTransform(QXmlStreamReader* stream);

    CrochetTab* tab;
    QXmlStreamReader* stream;
    
};
#endif //SAVETHREAD_H