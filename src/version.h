#ifndef VERSION_H
#define VERSION_H

#include "../VERSION"
#include <QString>
#include <QList>
#include <QRegExp>
#include <QStringList>

class Version
{
protected:
    Version()
    {
        QString gitVersion = GIT_VERSION;

        QRegExp re("(\\d+)");
        QStringList list;
        int pos = 0;

        QStringList versionParts = gitVersion.split("-", QString::KeepEmptyParts);
        QString mainVersion = versionParts.takeFirst();

        while ((pos = re.indexIn(mainVersion, pos)) != -1) {
             list << re.cap(1);
             pos += re.matchedLength();
        }

        m_major = list.takeFirst();
        m_minor = list.takeFirst();
        m_rev = list.takeFirst();
        foreach(QString part, versionParts)
            m_postfix += "-" + part;

        m_build = QString(__DATE__) + " " + QString(__TIME__);
    }

public:
    static Version * instance()
    {
        static Version *m_singleton = 0;
        if (m_singleton == 0) {
            m_singleton = new Version();
        }
        return m_singleton;
    }

    QString version()
    {
        return (m_major + "." + m_minor + "." + m_rev);
    }

    QString fullVersion()
    {
        QString fullVer = this->version();
        if(!m_postfix.isEmpty())
            fullVer += m_postfix;
        fullVer += QString(" (Built %1)").arg(m_build);

        return fullVer;
    }

    QString major() { return m_major; }
    QString minor() { return m_minor; }
    QString rev() { return m_rev; }

private:
    QString m_major;
    QString m_minor;
    QString m_rev;
    QString m_postfix;
    QString m_build;
};

#endif // VERSION_H
