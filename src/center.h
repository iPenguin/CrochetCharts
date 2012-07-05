#ifndef CENTER_H
#define CENTER_H

#include "stitch.h"

class Center : public Stitch
{
public:
    Center(QObject *parent = 0);

    enum { Type = 1001 };
    int type () const { return Center::Type; }
};

#endif // CENTER_H
