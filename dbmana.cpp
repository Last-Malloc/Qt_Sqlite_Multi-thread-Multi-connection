#include "dbmana.h"

QMap<QString, QMap<QString, QMutex*>> DBMana::mutexMap;

bool DBMana::execute(QString sql, QString connName, QString dbFile)
{
    QSqlDatabase t = getSqlDataBase(connName, dbFile);

    mutexMap[dbFile][connName]->lock();
    if (!t.isOpen())
        t.open();
    QSqlQuery sqlQuery(t);
    bool flag = sqlQuery.exec(sql);
    mutexMap[dbFile][connName]->unlock();
    return flag;
}

QSqlQuery* DBMana::select(QString sql, QString connName, QString dbFile)
{
    QSqlDatabase t = getSqlDataBase(connName, dbFile);

    mutexMap[dbFile][connName]->lock();
    if (!t.isOpen())
        t.open();
    QSqlQuery *sqlQuery = new QSqlQuery(t);
    sqlQuery->exec(sql);
    mutexMap[dbFile][connName]->unlock();
    return sqlQuery;
}

void DBMana::destroyConn(QString connName, QString dbFile)
{
    if (!QSqlDatabase::contains(dbFile + connName))
        return;
    QSqlDatabase t = QSqlDatabase::database(dbFile + connName);
    if (t.isOpen())
        t.close();
    QSqlDatabase::removeDatabase(dbFile + connName);
    delete mutexMap[dbFile][connName];
    mutexMap[dbFile].remove(connName);
    if (mutexMap[dbFile].keys().length() == 0)
        mutexMap.remove(dbFile);
}

void DBMana::destroyOneDBConn(QString dbFile)
{
    foreach (QString i, mutexMap[dbFile].keys())
    {
        destroyConn(i, dbFile);
    }
}

void DBMana::destroyAllDBConn()
{
    foreach (QString i, mutexMap.keys())
    {
        destroyOneDBConn(i);
    }
}

QSqlDatabase DBMana::getSqlDataBase(QString connName, QString dbFile)
{
    if (!QSqlDatabase::contains(dbFile + connName))
    {
        QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", dbFile + connName);
        database.setDatabaseName(dbFile);
        if (!mutexMap.contains(dbFile))
        {
            QMap<QString, QMutex*> tMap;
            mutexMap.insert(dbFile, tMap);
        }
        QMutex *mutex = new QMutex();
        mutexMap[dbFile].insert(connName, mutex);
    }
    return QSqlDatabase::database(dbFile + connName);
}
