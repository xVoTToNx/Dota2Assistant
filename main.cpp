#include "main_window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    return a.exec();

    // Ты переписал DataItemDelegate::paint и поэтому не видно выделение, так что на этапе косметики пофикси.
}
