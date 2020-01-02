#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Application information
    QApplication::setApplicationName("Process Dependency Monitor");
    QApplication::setOrganizationName("Alexander Kirsanov");
    QApplication::setOrganizationDomain("https://github.com/3dproger");
    QApplication::setApplicationVersion("1.1.1");

    MainWindow w;
    w.show();

    return a.exec();
}
