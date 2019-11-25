#include "data_tab_widget.h"
#include "main_window.h"

DataTabWidget::DataTabWidget(QString&& name, QWidget *parent)
    : QWidget(parent)
    , name(name)
    , current_table("test")
    , table_view (new QTableView(this))
    , filter_model (new FilterProxyModel(this))
    , search_model (new SearchProxyModel(table_view, this))
    , test_model (new QSqlTableModel(this, MainWindow::data_base))
    , layout (new QHBoxLayout(this))
    , table_layout (new QVBoxLayout(this))
    , table_button_layout (new QHBoxLayout(this))
    , table_label (new QLabel("Table:", this))
    , table_combobox (new QComboBox(this))
    , insert_button (new QPushButton("INSERT", this))
    , remove_button (new QPushButton("REMOVE", this))
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

    QGroupBox* table_group = new QGroupBox(this);
    table_group->setLayout(table_layout);
    QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spLeft.setHorizontalStretch(2);
    table_group->setSizePolicy(spLeft);
    layout->addWidget(table_group);

    QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spRight.setHorizontalStretch(1);
    filter_search_tab_widget->setSizePolicy(spRight);
    layout->addWidget(filter_search_tab_widget);

    connect(insert_button, &QPushButton::clicked, this, &DataTabWidget::insertRow);
    connect(remove_button, &QPushButton::clicked, this, &DataTabWidget::removeRow);
    table_combobox->setSizePolicy(QSizePolicy::Expanding, table_combobox->sizePolicy().verticalPolicy());
    table_combobox->addItems(MainWindow::data_base.tables());
    connect(table_combobox, &QComboBox::currentTextChanged, this, &DataTabWidget::changeTable);

    filter_model->setSourceModel(test_model);
    filter_model->setDynamicSortFilter(false);

    search_model->setSourceModel(filter_model);
    search_model->setDynamicSortFilter(false);

    table_view->setModel(search_model);
    table_view->setSortingEnabled(true);

    filter_tab_widget->setLayout(new QGridLayout());
    filter_search_tab_widget->addTab(filter_tab_widget, "Filter");
    search_tab_widget->setLayout(new QGridLayout());
    filter_search_tab_widget->addTab(search_tab_widget,"Search");

    changeTable(current_table);
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
    filter_model->ClearExpressions();
    search_model->ClearExpressions();

    auto record = test_model->record(0);
    if(!record.isEmpty())
        for( size_t i = 0; i < record.count(); ++i)
        {
            filter_layout->addWidget(new QLabel(test_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString()), i, 0);
            QWidget* widget = nullptr;
            auto func = std::bind(&FilterProxyModel::setExpression, filter_model, i,std::placeholders::_1);

            switch (record.field(i).type())
            {
                case QVariant::Type::Int:
                {
                    widget = new QSpinBox();
                    connect(static_cast<QSpinBox*>(widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), func);
                    break;
                }

                case QVariant::Type::String:
                {
                    widget = new QLineEdit();
                    connect(static_cast<QLineEdit*>(widget), &QLineEdit::textChanged, func);
                    break;
                }

                case TY_QT_FOR_CHAR:
                {
                    widget = new QSpinBox();
                    connect(static_cast<QSpinBox*>(widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), func);
                    break;
                }

                case QVariant::Type::Date:
                {
                    widget = new QDateEdit();
                    connect(static_cast<QDateEdit*>(widget), &QDateEdit::dateChanged, [this, i]( const QDate &date)
                    {
                        QString month_zero = date.month() > 9 ? "" : "0";
                        QString day_zero = date.day() > 9 ? "" : "0";
                        this->filter_model->setExpression(i, QString::number(date.year()) + '-' +
                        month_zero + QString::number(date.month()) + '-' +
                        day_zero + QString::number(date.day()));
                    });
                    break;
                }
            }

            if(widget != nullptr)
            {
                filter_layout->addWidget(widget, i, 1);

                QPushButton* clearButton = new QPushButton("CLR");
                filter_layout->addWidget(clearButton, i, 2);
                connect(clearButton, &QPushButton::clicked, [this, i](){this->filter_model->setExpression(i, "");});
            }
        }

    if(!record.isEmpty())
    {
        size_t i = 0;
        for(; i < record.count(); ++i)
        {
            search_layout->addWidget(new QLabel(test_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString()), i, 0);
            QWidget* widget = nullptr;
            auto func = std::bind(&SearchProxyModel::setExpression, search_model, i,std::placeholders::_1);

            switch (record.field(i).type())
            {
                case QVariant::Type::Int:
                {
                    widget = new QSpinBox();
                    connect(static_cast<QSpinBox*>(widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), func);
                    break;
                }

                case QVariant::Type::String:
                {
                    widget = new QLineEdit();
                    connect(static_cast<QLineEdit*>(widget), &QLineEdit::textChanged, func);
                    break;
                }

                case TY_QT_FOR_CHAR:
                {
                    widget = new QSpinBox();
                    connect(static_cast<QSpinBox*>(widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), func);
                    break;
                }

                case QVariant::Type::Date:
                {
                    widget = new QDateEdit();
                    connect(static_cast<QDateEdit*>(widget), &QDateEdit::dateChanged, [this, i]( const QDate &date)
                    {
                        QString month_zero = date.month() > 9 ? "" : "0";
                        QString day_zero = date.day() > 9 ? "" : "0";
                        this->search_model->setExpression(i, QString::number(date.year()) + '-' +
                        month_zero + QString::number(date.month()) + '-' +
                        day_zero + QString::number(date.day()));
                    });
                    break;
                }
            }

            if(widget != nullptr)
            {
                search_layout->addWidget(widget, i, 1);

                QPushButton* clearButton = new QPushButton("CLR");
                search_layout->addWidget(clearButton, i, 2);
                connect(clearButton, &QPushButton::clicked, [this, i](){this->search_model->setExpression(i, "");});
            }

        }

        QPushButton* searchButton = new QPushButton("CLR");
        search_layout->addWidget(searchButton, i, 0, 1, 3);
        connect(searchButton, &QPushButton::clicked, [this](){this->search_model->Search();});
    }
}

void DataTabWidget::changeTable(const QString& table_name)
{
    current_table = table_name;
    test_model->setTable(table_name);
    test_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    test_model->select();

    changeFilterSearchTabs();
}



