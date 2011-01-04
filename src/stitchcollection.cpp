#include "stitchcollection.h"

// Global static pointer
StitchCollection* StitchCollection::mInstance = NULL;

// singleton constructor:
StitchCollection* StitchCollection::inst()
{
   if (!mInstance)   // Only allow one instance of the settings.
      mInstance = new StitchCollection();
   return mInstance;
}

StitchCollection::StitchCollection(QObject *parent)
    : QObject(parent)
{

}
