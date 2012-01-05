/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef UPDATEFUNCTION_H
#define UPDATEFUNCTION_H

#include <QString>
#include "debug.h"

/**
 * This function contains snippets of code that should be run before a new version of the software is
 * used.
 */
void updateFunction(QString lastUsed)
{

    if(lastUsed < "1.2.0") {
        warn("convert default stitches -> default uid");
    }

}

#endif //UPDATEFUNCTION_H
