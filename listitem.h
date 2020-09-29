#ifndef LISTITEM_H
#define LISTITEM_H

#include <QWidget>
#include "URLStatus.h"

namespace Ui {
class listItem;
}

class listItem : public QWidget
{
    Q_OBJECT

public:
    explicit listItem(QPair<QString, URLStatus> data, QWidget *parent = nullptr);
    ~listItem();

private:
    Ui::listItem *ui;
};

#endif // LISTITEM_H
