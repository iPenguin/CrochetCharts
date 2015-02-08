/****************************************************************************\
 Copyright (c) 2010-2014 Stitch Works Software
 Brian C. Milco <bcmilco@gmail.com>

 This file is part of Crochet Charts.

 Crochet Charts is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Crochet Charts is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Crochet Charts. If not, see <http://www.gnu.org/licenses/>.

 \****************************************************************************/
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QStringList>

class MainWindow;

class Settings : public QObject
{
    Q_OBJECT
    friend class TestSettings;
public:
    static Settings* inst();
    ~Settings();

    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key) const;

    /**
     * The folder where the user's settings are stored. W/trailing slash.
     */
    QString userSettingsFolder();

    /**
     * return the default value of a given key.
     */
    QVariant defaultValue ( const QString& key ) const;

    /**
     * list of open files. All files should be added toLower().
     */
    QMap<QString, MainWindow*> files;

    void saveSettings() { mSettings.sync(); }

    void addRecentFile(QString fileName);

    QStringList recentFiles() { return mRecentFiles; }
    void setRecentFiles(QStringList files);

protected:
    QString fileName() { return mSettings.fileName(); }
    
private:
    /**
     * list of recent files. All strings added case sensitively (they'll be used to open the files).
     */
    QStringList mRecentFiles;
    
    static Settings* mInstance;
    
    Settings();

    void setupValueList();
    
    QSettings mSettings;

    QMap<QString, QVariant> mValueList;

};

#endif //SETTINGS_H
