#include "listitem.h"
#include "ui_listitem.h"


listItem::listItem(QPair<QString, URLStatus> data, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::listItem)
{
    ui->setupUi(this);
    
    ui->urlLabel->setText(QString("<a href=\"%1\">%1</a>").arg(data.first));
    ui->urlLabel->setTextFormat(Qt::RichText);
    ui->urlLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->urlLabel->setOpenExternalLinks(true);

    const auto getMessageAndSetColor = [&](const URLStatus status)
    {
        switch (status)
        {
        case URLStatus::Error:
            ui->statusLabel->setProperty("color", "orange");
            return "Error";
        case URLStatus::Queue:
            ui->statusLabel->setProperty("color", "blue");
            return "In Queue";
        case URLStatus::Scanning:
            ui->statusLabel->setProperty("color", "yellow");
            return "Scanning";
        case URLStatus::TextFound:
            ui->statusLabel->setProperty("color", "green");
            return "Found";
        case URLStatus::TextNotFound:
            ui->statusLabel->setProperty("color", "red");
            return "Not Found";
        }
    };
   
    ui->statusLabel->setText(getMessageAndSetColor(data.second));

}

listItem::~listItem()
{
    delete ui;
}
