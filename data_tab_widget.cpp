#include "data_tab_widget.h"
#include "main_window.h"

DataTabWidget::DataTabWidget(QString&& name, QWidget *parent)
    : QWidget(parent)
    , name(name)
    , sfp_model (new SortFilterProxyModel(this))
    , test_model (new QSqlTableModel(this, MainWindow::data_base))
    , layout (new QHBoxLayout(this))
    , table_layout (new QVBoxLayout(this))
    , table_button_layout (new QHBoxLayout(this))
    , table_label (new QLabel("Table:", this))
    , table_combobox (new QComboBox(this))
    , insert_button (new QPushButton("INSERT", this))
    , remove_button (new QPushButton("REMOVE", this))
    , table_view (new QTableView(this))
    , filter_search_tab_widget(new QTabWidget(this))
    , filter_tab_widget(new QWidget(this))
    , search_tab_widget(new QWidget(this))
{
    table_layout->addLayout(table_button_layout);
    table_button_layout->addWidget(table_label);
    table_button_layout->addWidget(table_combobox);
    table_button_layout->addWidget(insert_button);
    table_button_layout->addWidget(remove_button);

    table_layout->addWidget(table_view);

    layout->addLayout(table_layout);
    layout->addWidget(filter_search_tab_widget);

    connect(insert_button, &QPushButton::clicked, this, &DataTabWidget::insertRow);
    connect(remove_button, &QPushButton::clicked, this, &DataTabWidget::removeRow);
    table_combobox->setSizePolicy(QSizePolicy::Expanding, table_combobox->sizePolicy().verticalPolicy());
    table_combobox->addItems(MainWindow::data_base.tables());
    connect(table_combobox, &QComboBox::currentTextChanged, this, &DataTabWidget::changeTable);

    test_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    test_model->select();

    sfp_model->setSourceModel(test_model);
    sfp_model->setDynamicSortFilter(false);

    table_view->setModel(sfp_model);
    table_view->setSortingEnabled(true);

    filter_tab_widget->setLayout(new QGridLayout());
    filter_search_tab_widget->addTab(filter_tab_widget, "Filter");
    search_tab_widget->setLayout(new QGridLayout());
    filter_search_tab_widget->addTab(search_tab_widget,"Search");

    changeTable("Test");
    setLayout(layout);
}

void DataTabWidget::insertRow()
{
    QString query_test = "INSERT INTO " + test_model->tableName() + " VALUES (";
    for(size_t i = 0; i < test_model->columnCount() - 1; ++i)
    {
        query_test += " DEFAULT,";
    }
    query_test += " DEFAULT )";

    QSqlQuery qry;
    qry.prepare(query_test);

    if(!qry.exec())
        MainWindow::ThrowError("Something went wrong... Sooorry...<( _ _ )>");
    changeTable(test_model->tableName());
}

void DataTabWidget::removeRow()
{
    QModelIndex index = table_view->currentIndex();
    if(index.isValid())
        if(index.row() > - 1)
        {
            int answer = MainWindow::ThrowQuestion("Do you really want to remove this row?");

            if(answer == QMessageBox::Yes)
            {
                test_model->removeRow(index.row());
                changeTable(test_model->tableName());
            }
            return;
        }
    MainWindow::ThrowError("Invalid row. Sorry /(ㄒoㄒ)/~~");
}

void DataTabWidget::changeFilterSearchTabs()
{
    QGridLayout* filter_layout = static_cast<QGridLayout*>(filter_tab_widget->layout());
    QGridLayout* search_layout = static_cast<QGridLayout*>(search_tab_widget->layout());

    QLayoutItem* item;
    while ((item = filter_layout->takeAt(0)) != nullptr)
    {delete item->widget();delete item;}
    while ((item = search_layout->takeAt(0)) != nullptr)
    {delete item->widget();delete item;}
    sfp_model->ClearExpressions();

    auto record = test_model->record(0);
    if(!record.isEmpty())
        for( size_t i = 0; i < record.count(); ++i)
        {
            filter_layout->addWidget(new QLabel(test_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString()), i, 0);
            qDebug()<<(int)record.field(i).type();
            qDebug()<<(int)QVariant::Type::Char;
            qDebug()<<(record.field(i).type()==QVariant::Type::Char);
            switch (record.field(i).type())
            {
                case QVariant::Type::Int:
                {
                    QSpinBox* box = new QSpinBox();
                    filter_layout->addWidget(box, i, 1);
                    auto f1 = std::bind(&SortFilterProxyModel::setExpression, sfp_model, i,std::placeholders::_1);
                    connect(box, QOverload<const QString&>::of(&QSpinBox::valueChanged), f1);
                    break;
                }

                case QVariant::Type::String:
                {
                    QLineEdit* edit = new QLineEdit();
                    filter_layout->addWidget(edit, i, 1);
                    auto f1 = std::bind(&SortFilterProxyModel::setExpression, sfp_model, i,std::placeholders::_1);
                    connect(edit, &QLineEdit::textChanged, f1);
                    break;
                }

                case TY_QT_FOR_CHAR:
                {
                    QSpinBox* box = new QSpinBox();
                    filter_layout->addWidget(box, i, 1);
                    auto f1 = std::bind(&SortFilterProxyModel::setExpression, sfp_model, i,std::placeholders::_1);
                    connect(box, QOverload<const QString&>::of(&QSpinBox::valueChanged), f1);
                    break;
                }

                case QVariant::Type::Date:
                {
                    QDateEdit* date = new QDateEdit();
                    filter_layout->addWidget(date, i, 1);
                    connect(date, &QDateEdit::dateChanged, [this, i]( const QDate &date)
                    {
                        QString month_zero = date.month() > 9 ? "" : "0";
                        QString day_zero = date.day() > 9 ? "" : "0";
                        this->sfp_model->setExpression(i, QString::number(date.year()) + '-' +
                        month_zero + QString::number(date.month()) + '-' +
                        day_zero + QString::number(date.day()));
                    });
                    break;
                }
            }


            QPushButton* clearButton = new QPushButton("CLR");
            filter_layout->addWidget(clearButton, i, 2);
            connect(clearButton, &QPushButton::clicked, [this, i](){this->sfp_model->setExpression(i, "");});
        }
}

void DataTabWidget::changeTable(const QString& table_name)
{
    test_model->setTable(table_name);
    test_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    test_model->select();

    changeFilterSearchTabs();
}



