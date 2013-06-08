#include "application.h"

#include <QFileOpenEvent>

#include "stitchlibrary.h"

QStringList Application::mFileOpenEventList;

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv),
      mW(0)
{
    StitchLibrary* library = StitchLibrary::inst();
    library->loadStitchSets();
}

bool Application::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::FileOpen: {
            QString file = static_cast<QFileOpenEvent *>(event)->file();
            loadFile(file);
            return true;
        }
        default:{
            return QApplication::event(event);
        }
    }
}

void Application::loadFile(const QString &fileName)
{
    if(!mW) {
        qWarning() << "Application::loadFile cannot find a window to load this file!";
        return;
    }

    mW->loadFile(fileName);
}

void Application::resendFileOpenEvents(QObject *receiver)
{

    if(mFileOpenEventList.count() > 0)
    {

        QStringListIterator i(mFileOpenEventList);
        while(i.hasNext())
        {
            QFileOpenEvent event(i.next());
            Application::sendEvent(receiver, &event);
        }
        mFileOpenEventList.clear();
    }
}

void Application::setMainWindow(MainWindow *window)
{
    mW = window;
}
