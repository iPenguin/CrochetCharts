#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QStringList>

#include "mainwindow.h"

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);

    static void resendFileOpenEvents(QObject *receiver);

    void setMainWindow(MainWindow *window);

protected:
    bool event(QEvent *event);

private:
    void loadFile(const QString &fileName);
    static QStringList mFileOpenEventList;
    MainWindow *mW;
};

#endif //APPLICATION_H
