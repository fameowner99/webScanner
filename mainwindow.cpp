#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringListModel>
#include <thread>
#include "listitem.h"
#include "ButtonsState.h"


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
    ui->maxURLEdit->setValue(10);
    ui->maxURLEdit->setMaximum(1000000);

    ui->pauseButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
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
    connect(&mWebScanner, &WebScannerHandler::signalButtonsStatusChanged, [this](ButtonsState state)
        {
            switch (state)
            {
            case ButtonsState::Running:
            case ButtonsState::Paused:
                ui->startButton->setEnabled(false); ui->stopButton->setEnabled(true); ui->pauseButton->setEnabled(true);
                break;
            case ButtonsState::Finished:
                ui->startButton->setEnabled(true); ui->stopButton->setEnabled(false); ui->pauseButton->setEnabled(false);
                break;
            default:
                break;
            }
        });

}

void MainWindow::slotUpdateList(const QList<QPair<QString, URLStatus>> list)
{
    ui->listWidget->clear();
    for (const auto& item : list)
    {
        listItem* myListItem = new listItem(item);

        QListWidgetItem* defaultItem = new QListWidgetItem();
        defaultItem->setSizeHint(QSize(0, 50));
        ui->listWidget->addItem(defaultItem);
        ui->listWidget->setItemWidget(defaultItem, myListItem);
    }

}