/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings* inst();

    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key) const;

    bool isDemoVersion() { return mIsDemoVersion; }
    void setDemoVersion(bool isDemo) { mIsDemoVersion = isDemo; }

    QString userSettingsFolder();
    
private:
    void initDemoVersion();
    static Settings *mInstance;

    //return the default value of a given key.
    QVariant defaultValue ( const QString& key ) const;
    
    Settings();

    QSettings mSettings;

    bool mIsDemoVersion;
};

#endif //SETTINGS_H
