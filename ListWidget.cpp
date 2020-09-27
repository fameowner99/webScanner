#include "ListWidget.h"
#include <QDebug>

ListWidget::ListWidget(QWidget* parent)
    : QListWidget(parent)
{
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::MoveAction);
    setAlternatingRowColors(true);
}

void ListWidget::dropEvent(QDropEvent* event)
{
    QListWidget::dropEvent(event);
}