#include "settings.h"

#include <QApplication>
#include <QDebug>

// Global static pointer
Settings* Settings::mInstance = NULL;

// singleton constructor:
Settings* Settings::instance()
{
   if (!mInstance)   // Only allow one instance of the settings.
      mInstance = new Settings();
   return mInstance;
}

Settings::Settings()
{
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    mSettings.setValue(key, value);
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
    return mSettings.value(key, defaultValue);
}
