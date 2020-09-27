#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringListModel>
#include <thread>
#include "listitem.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initialize();
    setupConnections();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    ui->scanningEdit->setText(R"(https://en.wikipedia.org/wiki/Nokia)");
    ui->findTextEdit->setText(QString::fromStdWString(L"nokia"));
    
    ui->threadsEdit->setMinimum(1);
    ui->threadsEdit->setMaximum(std::thread::hardware_concurrency());
    ui->threadsEdit->setValue(1);

    ui->maxURLEdit->setMinimum(10);
    ui->maxURLEdit->setValue(1000);
    ui->maxURLEdit->setMaximum(1000000);
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
    connect(&mWebScanner, &WebScannerHandler::signalListUpdated, this, &MainWindow::slotUpdateList);
}

void MainWindow::slotUpdateList(const QList<QPair<QString, URLStatus>> list)
{
    for (const auto& item : list)
    {
        listItem* myListItem = new listItem(item);

        QListWidgetItem* defaultItem = new QListWidgetItem();
        defaultItem->setSizeHint(QSize(0, 50));
        ui->listWidget->addItem(defaultItem);
        ui->listWidget->setItemWidget(defaultItem, myListItem);
    }

}