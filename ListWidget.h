#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include <QListWidget>

class ListWidget : public QListWidget
{
    Q_OBJECT

public:

    ListWidget(QWidget* parent = 0);

protected:
    void dropEvent(QDropEvent* event);
};

#endif // MYLISTWIDGET_H