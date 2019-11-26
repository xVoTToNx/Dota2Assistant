#include "data_tab_widget.h"
#include "main_window.h"

DataTabWidget::DataTabWidget(QString&& name, QWidget *parent)
    : QWidget(parent)
    , name(name)
    , current_table("heroes")
    , table_view (new QTableView(this))
    , filter_model (new FilterProxyModel(this))
    , search_model (new SearchProxyModel(table_view, this))
    , test_model (new QSqlTableModel(this, MainWindow::data_base))
    , layout (new QHBoxLayout(this))
    , table_layout (new QVBoxLayout())
    , table_button_layout (new QHBoxLayout())
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
    table_combobox->setCurrentText("heroes");
    connect(table_combobox, &QComboBox::currentTextChanged, this, &DataTabWidget::changeTable);

    filter_model->setSourceModel(test_model);
    filter_model->setDynamicSortFilter(false);

    search_model->setSourceModel(filter_model);
    search_model->setDynamicSortFilter(false);

    table_view->setModel(search_model);
    table_view->setSortingEnabled(true);

    table_view->setItemDelegate(new DataItemDelegate(test_model, table_view));

    filter_tab_widget->setLayout(new QGridLayout());
    filter_search_tab_widget->addTab(filter_tab_widget, "Filter");
    search_tab_widget->setLayout(new QGridLayout());
    filter_search_tab_widget->addTab(search_tab_widget,"Search");

    changeTable(current_table);
    setLayout(layout);
}

void DataTabWidget::insertRow()
{
    bool was_closed = true;

    QString query_test = "INSERT INTO " + test_model->tableName() + " VALUES (";
    QDialog* dialog = new QDialog(this);
    QVBoxLayout* layout = new QVBoxLayout();

    QPushButton* button = new QPushButton("Insert!");
    connect(button, &QPushButton::clicked, dialog, &QDialog::close);
    dialog->setLayout(layout);

    if(current_table == "heroes")
    {
        QLineEdit* edit = new QLineEdit();
        layout->addWidget(new QLabel("Hero Name"));
        layout->addWidget(edit);
        connect(button, &QPushButton::clicked, [dialog, &query_test, this, edit, &was_closed]()
        {
            if(edit->text() == "") return;
            query_test += " '" + edit->text() + "',";
            for(size_t i = 1; i < test_model->columnCount() - 1; ++i)
                query_test += " DEFAULT,";
            query_test += " DEFAULT )";

            was_closed = false;
        });
    }
    else if(current_table == "skills")
    {
        QLineEdit* edit_skill = new QLineEdit();
        layout->addWidget(new QLabel("Skill Name"));
        layout->addWidget(edit_skill);
        QLineEdit* edit_hero = new QLineEdit();
        layout->addWidget(new QLabel("Hero Name"));
        layout->addWidget(edit_hero);
        connect(button, &QPushButton::clicked, [dialog, &query_test, this, edit_hero, edit_skill, &was_closed]()
        {
            if(edit_skill->text() == "") return;
            query_test += " '" + edit_skill->text() + "', DEFAULT, '" + edit_hero->text() + "',";
            for(size_t i = 3; i < test_model->columnCount() - 1; ++i)
                query_test += " DEFAULT,";
            query_test += " DEFAULT )";

            was_closed = false;
        });
    }
    else if(current_table == "items")
    {
        QLineEdit* edit_item = new QLineEdit();
        layout->addWidget(new QLabel("Item Name"));
        layout->addWidget(edit_item);
        connect(button, &QPushButton::clicked, [dialog, &query_test, this, edit_item, &was_closed]()
        {
            if(edit_item->text() == "") return;
            query_test += " '" + edit_item->text() + "',";
            for(size_t i = 1; i < test_model->columnCount() - 1; ++i)
                query_test += " DEFAULT,";
            query_test += " DEFAULT )";

            was_closed = false;
        });
    }
    else if(current_table == "recomendations")
    {
        QLineEdit* edit_hero = new QLineEdit();
        layout->addWidget(new QLabel("Hero Name"));
        layout->addWidget(edit_hero);
        QLineEdit* edit_item = new QLineEdit();
        layout->addWidget(new QLabel("Item Name"));
        layout->addWidget(edit_item);
        QRadioButton* radio_contr = new QRadioButton();
        layout->addWidget(new QLabel("Is Contr Item"));
        layout->addWidget(radio_contr);
        connect(button, &QPushButton::clicked, [&query_test, this, edit_hero, edit_item, radio_contr, &was_closed]()
        {
            query_test += " '" + edit_hero->text() + "', '" + edit_item->text() + "',";
            if (radio_contr->isChecked())
                query_test += "true, ";
            else
                query_test += "false, ";
            for(size_t i = 3; i < test_model->columnCount() - 1; ++i)
                query_test += " DEFAULT,";
            query_test += " DEFAULT )";

            was_closed = false;
        });
    }
    else if(current_table == "roles")
    {
        QLineEdit* edit_role = new QLineEdit();
        layout->addWidget(new QLabel("Role Name"));
        layout->addWidget(edit_role);
        connect(button, &QPushButton::clicked, [dialog, &query_test, this, edit_role, &was_closed]()
        {
            if(edit_role->text() == "") return;
            query_test += " '" + edit_role->text() + "',";
            for(size_t i = 1; i < test_model->columnCount() - 1; ++i)
                query_test += " DEFAULT,";
            query_test += " DEFAULT )";

            was_closed = false;
        });
    }
    else if(current_table == "hero_roles")
    {
        QLineEdit* edit_hero = new QLineEdit();
        layout->addWidget(new QLabel("Hero Name"));
        layout->addWidget(edit_hero);
        QLineEdit* edit_role = new QLineEdit();
        layout->addWidget(new QLabel("Role Name"));
        layout->addWidget(edit_role);
        connect(button, &QPushButton::clicked, [&query_test, this, edit_hero, edit_role, &was_closed]()
        {
            query_test += " '" + edit_hero->text() + "', '" + edit_role->text() + "',";
            for(size_t i = 2; i < test_model->columnCount() - 1; ++i)
                query_test += " DEFAULT,";
            query_test += " DEFAULT )";

            was_closed = false;
        });
    }
    else if(current_table == "teams")
    {
        QLineEdit* edit_team = new QLineEdit();
        layout->addWidget(new QLabel("Team Name"));
        layout->addWidget(edit_team);
        connect(button, &QPushButton::clicked, [dialog, &query_test, this, edit_team, &was_closed]()
        {
            if(edit_team->text() == "") return;
            query_test += " '" + edit_team->text() + "',";
            for(size_t i = 1; i < test_model->columnCount() - 1; ++i)
                query_test += " DEFAULT,";
            query_test += " DEFAULT )";

            was_closed = false;
        });
    }
    else if(current_table == "team_heroes")
    {
        QLineEdit* edit_hero = new QLineEdit();
        layout->addWidget(new QLabel("Hero Name"));
        layout->addWidget(edit_hero);
        QLineEdit* edit_team = new QLineEdit();
        layout->addWidget(new QLabel("Team Name"));
        layout->addWidget(edit_team);
        connect(button, &QPushButton::clicked, [&query_test, this, edit_hero, edit_team, &was_closed]()
        {
            query_test += " '" + edit_hero->text() + "', '" + edit_team->text() + "')";
            was_closed = false;
        });
    }

    layout->addWidget(button);
    dialog->exec();
    if(was_closed)
        return;

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
                QString query_text = "delete from " + test_model->tableName() + " where ";
                size_t i = 0;
                for(; i < test_model->columnCount() - 1; ++i)
                {
                    QVariant value = test_model->data(test_model->index(index.row(), i));
                    if(value.type() == QVariant::Type::DateTime)
                        continue;
                    QString str_value = value.type() == QVariant::Type::String ?
                                        "'" + value.toString() + "'" : value.toString();
                    query_text += HEADER(test_model, i) + " = " + str_value + " AND ";
                }
                query_text += HEADER(test_model, i) + " = " + test_model->data(test_model->index(index.row(), i)).toString();
                QSqlQuery qry;
                qry.prepare(query_text);
                qry.exec();

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
    test_model->setEditStrategy(QSqlTableModel::OnManualSubmit);;
    test_model->select();

    changeFilterSearchTabs();
}



