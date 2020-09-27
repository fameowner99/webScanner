#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "WebScannerHandler.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void slotUpdateList(const QList<QPair<QString, URLStatus>> list);

private:
    void initialize();
    void setupConnections();

private:
    Ui::MainWindow *ui;
    WebScannerHandler mWebScanner;

};

#endif // MAINWINDOW_H
