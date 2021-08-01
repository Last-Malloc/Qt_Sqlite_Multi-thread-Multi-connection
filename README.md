[csdn文章地址](https://blog.csdn.net/qq_42283621/article/details/119303877)

[github仓库地址](https://github.com/Last-Malloc/Qt_Sqlite_Multi-thread-Multi-connection)

# 1. Qt连接Sqlite数据库的基本语句

```c++
//Qt中以数据库连接名来管理数据库连接，即 连接名 和 数据库连接 一一对应
//数据中是否存在连接connName，返回bool
QSqlDatabase::contains(connName)
//（当数据库连接不存在时）添加数据库连接并返回，参数1为驱动名，参数2为连接名
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
//为数据库连接设置要操作的数据库文件(.db)
db.setDatabaseName(dbFile);
//（当数据库连接已存在时）返回该数据库连接
QSqlDatabase::database(connName)
//打开数据库连接，返回bool数据库打开是否成功
db.open();
//测试数据库连接是否已经连接，返回bool
db.isOpen()
//关闭数据库连接
db.close();
//执行数据操作数据库
QSqlQuery sqlQuery(db);
sqlQuery.exec(sql);
//不需要返回值的操作可以写作
QSqlQuery sqlQuery(db);
bool flag = sqlQuery.exec(sql);
//需要返回值得操作可以写作
//通过QSqlQuery::next()不断获取数据行
QSqlQuery *sqlQuery = new QSqlQuery(t);
sqlQuery->exec(sql);
return sqlQuery;
//删除数据库连接connName
QSqlDatabase::removeDatabase(connName);
```

# 2. 封装数据库操作类DBMana

使用QMutex 对 连接 加锁，适用于多线程，但建议经常同时操作同一数据库的线程 使用 不同的连接名，因为锁的互斥等待会增大处理时间。
（数据库中实际的连接名是"dbFile+connName"，但若使用此类的封装则不用关心实际的连接名）

```c++
//共有函数
    //以connName连接对数据库dbFile执行sql语句，该sql语句无返回值，函数返回执行成功/失败
    //若该连接已存在则则获取，不存在则创建再获取
    static bool execute(QString sql, QString connName = "DBMana", QString dbFile = "systemData.db");
    //以connName连接对数据库dbFile执行sql语句，该sql语句有返回值，通过QSqlQuery::next()不断获取数据行
    //若该连接已存在则则获取，不存在则创建再获取
    static QSqlQuery* select(QString sql, QString connName = "DBMana", QString dbFile = "systemData.db");
    //删除db文件dbFile的connName连接和其对应的锁
    static void destroyConn(QString connName = "DBMana", QString dbFile = "systemData.db");
    //删除db文件dbFile的所有连接和锁
    static void destroyOneDBConn(QString dbFile = "systemData.db");
    //删除所有db文件的所有连接和锁
    static void destroyAllDBConn();
```

# 3. 详细代码

详细代码和使用方式放在了github中。

```C++
#ifndef DBMANA_H
#define DBMANA_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>
#include <QMap>

//数据库操作类
//使用QMutex 对 连接 加锁，适用于多线程
//但建议经常同时操作同一数据库的线程 使用 不同的连接名，因为锁的互斥等待会增大处理时间
//（数据库中实际的连接名是"dbFile+connName"，但若使用此类的封装则不用关心实际的连接名）
class DBMana : public QObject
{
    Q_OBJECT

public:
    //以connName连接对数据库dbFile执行sql语句，该sql语句无返回值，函数返回执行成功/失败
    //若该连接已存在则则获取，不存在则创建再获取
    static bool execute(QString sql, QString connName = "DBMana", QString dbFile = "systemData.db");
    //以connName连接对数据库dbFile执行sql语句，该sql语句有返回值，通过QSqlQuery::next()不断获取数据行
    //若该连接已存在则则获取，不存在则创建再获取
    static QSqlQuery* select(QString sql, QString connName = "DBMana", QString dbFile = "systemData.db");
    //删除db文件dbFile的connName连接和其对应的锁
    static void destroyConn(QString connName = "DBMana", QString dbFile = "systemData.db");
    //删除db文件dbFile的所有连接和锁
    static void destroyOneDBConn(QString dbFile = "systemData.db");
    //删除所有db文件的所有连接和锁
    static void destroyAllDBConn();

private:
    //获取对于dbFile文件的数据库连接connName
    //有则返回，没有则创建再返回
    static QSqlDatabase getSqlDataBase(QString connName, QString dbFile);
    //同步锁，外层key是db文件名，内层key是该db文件的数据库连接名
    static QMap<QString, QMap<QString, QMutex*>> mutexMap;

};

#endif // DBMANA_H
```

```c++
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
```

