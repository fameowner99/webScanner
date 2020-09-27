#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringListModel>
#include <thread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initialize();
    setupConnections();

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

void MainWindow::initialize()
{
    ui->scanningEdit->setText(R"(https://ru.wikipedia.org/wiki/%D0%9F%D0%B5%D1%80%D0%B2%D0%BE%D0%BC%D0%B0%D0%B9%D1%81%D0%BA_(%D0%9D%D0%B8%D0%BA%D0%BE%D0%BB%D0%B0%D0%B5%D0%B2%D1%81%D0%BA%D0%B0%D1%8F_%D0%BE%D0%B1%D0%BB%D0%B0%D1%81%D1%82%D1%8C))");
    ui->findTextEdit->setText(QString::fromStdWString(L"Первомайск"));
    
    ui->threadsEdit->setMinimum(1);
    ui->threadsEdit->setMaximum(std::thread::hardware_concurrency());
    ui->threadsEdit->setValue(1);

    ui->maxURLEdit->setMinimum(10);
    ui->maxURLEdit->setValue(10);
}

void MainWindow::setupConnections()
{
    connect(ui->startButton, &QPushButton::clicked, [this]
    {
        mWebScanner.setConfigurations({ui->scanningEdit->text(), ui->findTextEdit->text(),
            static_cast<size_t>(ui->threadsEdit->value()),  static_cast<size_t>(ui->maxURLEdit->value()) });
        mWebScanner.startScanning();
    });

    connect(ui->stopButton, &QPushButton::clicked, &mWebScanner, &WebScannerHandler::stopScanning);
    connect(ui->pauseButton, &QPushButton::clicked, &mWebScanner, &WebScannerHandler::pauseScanning);
}