#include "logindialog.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    {
        LoginDialog d;
        if (d.exec() != QDialog::Accepted)
            return EXIT_SUCCESS;
    }
    MainWindow w;
    w.show();
    return a.exec();
}
