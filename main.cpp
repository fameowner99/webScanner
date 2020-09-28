#include <iostream>
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;

    mainWindow.setWindowTitle("Web Scanner");
    mainWindow.show();

    return app.exec();
}