/*************************************************\
| (c) 2011 Stitch Works Software                  |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#include "updater.h"
#include <qmessagebox.h>

#include <QDebug>

Updater::Updater(QWidget* parent)
    : QWidget(parent)
{
}

Updater::~Updater()
{
}

void Updater::checkForUpdates(bool silent)
{
qDebug() << "checkforupdates start";
    
    if(!silent)
        QMessageBox::information(this, tr("Check for Updates"),
                                 tr("You are currently using the most recent version of this software."),
                                QMessageBox::Ok);
    
}

