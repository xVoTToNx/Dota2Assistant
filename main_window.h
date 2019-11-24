#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QSqlDatabase>


#include "data_tab_widget.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
public:
    Q_OBJECT

public:
    static QSqlDatabase data_base;
    static void ThrowError(QString&& text);
    static int ThrowQuestion(QString &&text);

    QTabWidget* main_tab_widget;

    DataTabWidget* data_tab_widget;


    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool configureDB();

    Ui::MainWindow *ui;


};
#endif // MAINWINDOW_H
