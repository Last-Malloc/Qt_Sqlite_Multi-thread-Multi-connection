#ifndef WORKER_H
#define WORKER_H

#include <QRunnable>
#include <QString>
#include <QSqlDatabase>
#include "dbmana.h"

class Worker : public QRunnable
{
public:
    Worker(QString connName);
    void run() override;

private:
    QString connName;
};

#endif // WORKER_H
