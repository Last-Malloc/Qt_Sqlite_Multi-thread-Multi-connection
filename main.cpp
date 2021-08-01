#include "mainwindow.h"
#include "dbmana.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    //关闭所有的数据库连接
    DBMana::destroyAllDBConn();
    return a.exec();
}
