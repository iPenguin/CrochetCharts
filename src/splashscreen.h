/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>

class SplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    SplashScreen();

public slots:
    void showMessage (const QString &message);


protected:
    void drawContents (QPainter *painter);

private:
    QString mMessage;
};
#endif //SPLASHSCREEN_H
