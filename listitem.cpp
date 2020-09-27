#include "listitem.h"
#include "ui_listitem.h"


listItem::listItem(QPair<QString, URLStatus> data, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::listItem)
{
    ui->setupUi(this);
    
    ui->urlLabel->setText(data.first);
    
    const auto getMessage = [&](const URLStatus status)
    {
        switch (status)
        {
        case URLStatus::Error:
            return "Error";
        case URLStatus::Queue:
            return "In Queue";
        case URLStatus::Scanning:
            return "Scanning";
        case URLStatus::TextFound:
            return "Found";
        case URLStatus::TextNotFound:
            return "Not Found";
        }
    };
            
    ui->statusLabel->setText(getMessage(data.second));

}

listItem::~listItem()
{
    delete ui;
}
