#include "MainWindow.h"
#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MainWindow mainWindow;

    mainWindow.resize(1024, 600);
    mainWindow.setWindowIcon(QIcon(":/images/window.png"));
    mainWindow.show();
    return app.exec();
}
