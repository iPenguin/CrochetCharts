#include "settings.h"

#include "license.h"

#include <QApplication>
#include <QDebug>

// Global static pointer
Settings* Settings::mInstance = NULL;

// singleton constructor:
Settings* Settings::inst()
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

bool Settings::isDemoVersion()
{
    QString license = mSettings.value("license", QVariant("")).toString();
    QString sn      = mSettings.value("serialNumber", QVariant("")).toString();
    QString email   = mSettings.value("email", QVariant("")).toString();

    if(!License::isValidSerialNumber(sn))
        return true;
    if(!License::isValidLicense(license, sn, email))
        return true;

    return false;
}
