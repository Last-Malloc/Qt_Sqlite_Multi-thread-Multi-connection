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
