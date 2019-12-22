#include "main_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
// Как вариант сделать добавление ВСЕХ полей сразу, а не только ключей.
    return a.exec();
}
