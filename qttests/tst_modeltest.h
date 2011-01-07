#ifndef TST_MODELTEST_H
#define TST_MODELTEST_H

#include <QtTest/QTest>
#include <QObject>
#include <QSortFilterProxyModel>

class tst_ModelTest : public QObject
{
    Q_OBJECT

public:
    tst_ModelTest() {}
    virtual ~tst_ModelTest() {}

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void stringListModel();
    void treeWidgetModel();
    void standardItemModel();
    void testInsertThroughProxy();
    void moveSourceItems();
    void testResetThroughProxy();
};

/**
  Makes the persistent index list publicly accessible
*/
class AccessibleProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    AccessibleProxyModel(QObject *parent = 0) : QSortFilterProxyModel(parent) {}

    QModelIndexList persistent()
    {
        return persistentIndexList();
    }
};



class ObservingObject : public QObject
{
    Q_OBJECT
public:
    ObservingObject(AccessibleProxyModel  *proxy, QObject *parent = 0)
    : QObject(parent),
    m_proxy(proxy)
    {
        connect(m_proxy, SIGNAL(layoutAboutToBeChanged()), SLOT(storePersistent()));
        connect(m_proxy, SIGNAL(layoutChanged()), SLOT(checkPersistent()));
    }

public slots:

    void storePersistent(const QModelIndex &parent)
    {
        for (int row = 0; row < m_proxy->rowCount(parent); ++row) {
            QModelIndex proxyIndex = m_proxy->index(row, 0, parent);
            QModelIndex sourceIndex = m_proxy->mapToSource(proxyIndex);
            Q_ASSERT(proxyIndex.isValid());
            Q_ASSERT(sourceIndex.isValid());
            m_persistentSourceIndexes.append(sourceIndex);
            m_persistentProxyIndexes.append(proxyIndex);
            if (m_proxy->hasChildren(proxyIndex))
                storePersistent(proxyIndex);
        }
    }

    void storePersistent()
    {
      foreach(const QModelIndex &idx, m_persistentProxyIndexes)
        Q_ASSERT(idx.isValid()); // This is called from layoutAboutToBeChanged. Persistent indexes should be valid

        Q_ASSERT(m_proxy->persistent().isEmpty());
        storePersistent(QModelIndex());
        Q_ASSERT(!m_proxy->persistent().isEmpty());
    }

    void checkPersistent()
    {
        for (int row = 0; row < m_persistentProxyIndexes.size(); ++row) {
            QModelIndex updatedProxy = m_persistentProxyIndexes.at(row);
            QModelIndex updatedSource = m_persistentSourceIndexes.at(row);
        }
        for (int row = 0; row < m_persistentProxyIndexes.size(); ++row) {
            QModelIndex updatedProxy = m_persistentProxyIndexes.at(row);
            QModelIndex updatedSource = m_persistentSourceIndexes.at(row);
            QCOMPARE(m_proxy->mapToSource(updatedProxy), updatedSource);
        }
        m_persistentSourceIndexes.clear();
        m_persistentProxyIndexes.clear();
    }

private:
    AccessibleProxyModel  *m_proxy;
    QList<QPersistentModelIndex> m_persistentSourceIndexes;
    QList<QPersistentModelIndex> m_persistentProxyIndexes;
};


#endif //TST_MODELTEST_H
