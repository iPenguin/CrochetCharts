/*************************************************\
| Copyright (c) 2011 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef UPDATEFUNCTION_H
#define UPDATEFUNCTION_H

#include <QString>

/**
 * This function contains snippets of code that should be run before a new version of the software is
 * used.
 */
void updateFunction(QString lastUsed)
{

    if(lastUsed < "0.9.91-63-gb088c70") {

    }

}

#endif //UPDATEFUNCTION_H