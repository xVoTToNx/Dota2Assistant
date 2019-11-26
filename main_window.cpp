#include "main_window.h"
#include "ui_mainwindow.h"

#include <QtSql>
#include <QDebug>
#include <QTabWidget>
#include <QTableView>
#include <QHBoxLayout>
#include <QPushButton>



QSqlDatabase MainWindow::data_base = QSqlDatabase::addDatabase("QMYSQL");

QLabel* MainWindow::CreateLabel(QString text)
{
    QLabel* label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    return label;
}

void MainWindow::ThrowError(QString &&text)
{
    QMessageBox box;
    box.setText(text);
    box.setStandardButtons(QMessageBox::Ok);
    box.exec();
}

int MainWindow::ThrowQuestion(QString &&text)
{
    QMessageBox box;
    box.setText(text);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    return box.exec();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , main_tab_widget(new QTabWidget())
    , ui(new Ui::MainWindow)

{
    qDebug()<<"Start Ctor";

    ui->setupUi(this);
    configureDB() ? qDebug()<<"Everything is OK."
                  : qDebug()<<QString(666, 'F');


    data_tab_widget = new DataTabWidget("Data", this);
    main_tab_widget->addTab(data_tab_widget, data_tab_widget->GetName());
    qDebug()<<"DataTab Finished";
    icons_tab_widget = new IconsTabWidget("Icons", this);
    main_tab_widget->addTab(icons_tab_widget, icons_tab_widget->GetName());
    qDebug()<<"IconTab Finished";

    connect(main_tab_widget, &QTabWidget::currentChanged, [this](int index)
    {
       switch (index)
       {
       case 0:
           data_tab_widget->Update();
           break;
       case 1:
           icons_tab_widget->Update();
           break;
       }
    });

    qDebug()<<"Everything's Ready";
    main_tab_widget->show();

    qDebug()<<"End Ctor";
}

MainWindow::~MainWindow()
{
    delete ui;
    delete main_tab_widget;
}

bool MainWindow::configureDB()
{
    data_base.setHostName("127.0.0.1");
    data_base.setUserName("root");
    data_base.setPassword("root");
    data_base.setPort(3306);
    data_base.setDatabaseName("dota");

    return data_base.open();
}

