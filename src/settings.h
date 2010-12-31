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
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

    bool isDemoVersion();

private:
    static Settings *mInstance;

    Settings();

    QSettings mSettings;
};

#endif //SETTINGS_H
