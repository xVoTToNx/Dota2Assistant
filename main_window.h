#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QSqlDatabase>


#include "Data/data_tab_widget.h"
#include "Icons/icons_tab_widget.h"
#include "Algor/algor_tab_widget.h"

#define HEADER(X, Y) X->headerData(Y, Qt::Horizontal, Qt::DisplayRole).toString()
#define DATA_ROLE 66

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
    static QLabel* CreateLabel(QString text);
    static QString VariantToSql(QVariant value, QVariant::Type type = QVariant::Type::UserType);

    QTabWidget* main_tab_widget;

    DataTabWidget* data_tab_widget;
    IconsTabWidget* icons_tab_widget;
    AlgorTabWidget* algor_tab_widget;


    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool configureDB();

    Ui::MainWindow *ui;


};
#endif // MAINWINDOW_H
