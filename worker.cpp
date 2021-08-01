#include "worker.h"

Worker::Worker(QString connName)
{
    this->connName = connName;
}

void Worker::run()
{
    for (int i = 0; i < 10; ++i)
    {
        DBMana::execute("insert into tab values('" + connName + QString::number(i + 1) +"');", connName);
    }
}
