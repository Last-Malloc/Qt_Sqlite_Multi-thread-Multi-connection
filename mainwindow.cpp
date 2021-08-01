#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_1_clicked()
{
    //启动新线程，以数据库连接名"aaa"对数据库进行操作
    Worker *t = new Worker("aaa");
    QThreadPool::globalInstance()->start(t);
}

void MainWindow::on_pushButton_2_clicked()
{
    Worker *t = new Worker("bbb");
    QThreadPool::globalInstance()->start(t);
}

void MainWindow::on_pushButton_3_clicked()
{
    Worker *t = new Worker("ccc");
    QThreadPool::globalInstance()->start(t);
}
