#include "main_window.h"
#include "ui_mainwindow.h"

QSqlDatabase MainWindow::data_base = QSqlDatabase::addDatabase("QMYSQL");

QLabel* MainWindow::CreateLabel(QString text)
{
    QLabel* label = new QLabel(text);
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    return label;
}

QString MainWindow::VariantToSql(QVariant value, QVariant::Type type)
{
    if(type == QVariant::UserType)
        type = value.type();

    switch(type)
    {
    case QVariant::Type::String:
        return "'" + value.toString() + "'";

    case QVariant::Type::Int:
    case QVariant::Type::UInt:
    case QVariant::Type::LongLong:
    case QVariant::Type::ULongLong:
    case QVariant::Type::Double:
        return value.toString();

    case QVariant::Type::Date:
        return "'" + value.toDate().toString("yyyy-MM-dd") + "'";

    case QVariant::Type::DateTime:
        return "'" + value.toDateTime().toString("yyyy-MM-dd hh:mm:ss") + "'";

    case QVariant::Type::Bool:
        return value.toString() == "true" ? "1" : "0";

    default:
        return "";
    }
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
    icons_tab_widget = new IconsTabWidget("Icons", this, this);
    main_tab_widget->addTab(icons_tab_widget, icons_tab_widget->GetName());
    qDebug()<<"IconTab Finished";
    algor_tab_widget = new AlgorTabWidget("Algor", this, this);
    main_tab_widget->addTab(algor_tab_widget, algor_tab_widget->GetName());
    qDebug()<<"AlgorTab Finished";

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
       case 2:
           algor_tab_widget->Update();
           break;
       }
    });

    qDebug()<<"Everything's Ready";
    main_tab_widget->show();
    main_tab_widget->setGeometry(main_tab_widget->x(), main_tab_widget->y(), 1000, 700);
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
    data_base.setPort(54000);
    data_base.setDatabaseName("dota");

    if(!data_base.open())
        data_base.setPort(3306);

    return data_base.open();
}

