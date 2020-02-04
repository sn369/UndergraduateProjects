#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.setWindowTitle("Project 5-姓名哈希表");
    w.setWindowIcon(QIcon("icon.ico"));
    return a.exec();
}
