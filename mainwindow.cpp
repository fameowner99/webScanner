#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto model = new QStringListModel(this);

    QStringList list;
    list << "cats" << "dogs" << "rats";

    model->setStringList(list);

    ui->listView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}
