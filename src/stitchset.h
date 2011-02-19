/*************************************************\
| Copyright (c) 2010 Stitch Works Software        |
| Brian C. Milco <brian@stitchworkssoftware.com>  |
\*************************************************/
#ifndef STITCHSET_H
#define STITCHSET_H

#include <QList>
#include <QAbstractItemModel>
#include "stitch.h"

class QDomElement;
class QXmlStreamWriter;

class StitchSet : public QAbstractItemModel
{
    Q_OBJECT
    friend class StitchLibrary;
    friend class StitchLibraryUi;
    friend class SaveFile;
public:

    enum SaveVersion { Version_1_0_0 = 100 };
    
    StitchSet(QObject *parent = 0, bool isMasterSet = false, bool isBuiltIn = false);
    ~StitchSet();

    //load icons tells the function to test/replace the icon file names.
    bool loadXmlFile(QString fileName);
    //if you don't pass in a fileName the default setFileName will be used.
    void saveXmlFile(QString fileName = "");

    //load stitchset with icon data.
    void loadDataFile(QString fileName, QString dest);
    //save stitchset with icon data.
    void saveDataFile(QString fileName);

    const QString name() const { return mName; }
    void setName(QString n) { mName = n; }

    const QString author() { return mAuthor; }
    void setAuthor(QString a) { mAuthor = a; }

    const QString email() { return mEmail; }
    void setEmail(QString e) { mEmail = e; }

    const QString org() { return mOrg; }
    void setOrg(QString o) { mOrg = o; }

    const QString url() { return mUrl; }
    void setUrl(QString u) { mUrl = u; }

    Stitch* findStitch(QString name);

    bool hasStitch(QString name);
    void addStitch(Stitch *s);
    void createStitch(QString name);

    /**
     * removes the stitch from the set.
     * NOTE: this function doesn't delete the stitch.
     */
    void removeStitch(QString name);
    
    int stitchCount() const;

    QList<Stitch *> stitches() const { return mStitches; }
    void clearStitches();

    //The stitch set that the user is works with.
    bool isMasterSet;
    //The set of stitches that comes with the software.
    bool isBuiltInSet;
    //The set is temporary and wont be saved.
    bool isTemporary;

    QString stitchSetFileName;

    //returns the abs path to the stitch set. w/trailing slash.
    QString stitchSetFolder();

    //reset the model in all views.
    void refreshSet() { reset(); }
    void clearSelection() { mSelected.clear(); }

    //clear the stitches and load them from the orig file.
    void reset();
    
signals:
    void stitchNameChanged(QString setName, QString oldName, QString newName);
    
protected:
    void loadXmlStitchSet(QDomElement *element, bool loadIcons = false);
    void saveXmlStitchSet(QXmlStreamWriter *stream, bool saveIcons = false);

    void saveIcons(QDataStream *out);
    void loadIcons(QDataStream *in);
    
private:
    void loadXmlStitch(QDomElement *element, bool loadIcon = false);

    QList<Stitch*> mStitches;
    //list of checked items
    QList<Stitch*> mSelected;
    
    QString mName,
            mAuthor,
            mEmail,
            mOrg,
            mUrl;

/***************************************************************\
|QAbstractItemModel
\***************************************************************/
public:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    
/***************************************************************/
private:
    qint32 mSaveFileVersion;
};

#endif //STITCHSET_H
