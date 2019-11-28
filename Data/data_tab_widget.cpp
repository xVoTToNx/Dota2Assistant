#include "data_tab_widget.h"
#include "main_window.h"

DataTabWidget::DataTabWidget(QString&& name, QWidget *parent)
    : QWidget(parent)
    , name(name)
    , current_table("heroes")
    , is_custom_query(false)
    , table_view (new QTableView(this))
    , filter_model (new FilterProxyModel(this))
    , search_model (new SearchProxyModel(table_view, this))
    , model (new QSqlTableModel(this, MainWindow::data_base))
    , query_model (new QSqlQueryModel(this))
    , layout (new QHBoxLayout(this))
    , table_layout (new QVBoxLayout())
    , table_button_layout (new QHBoxLayout())
    , table_label (new QLabel("Table:", this))
    , table_combobox (new QComboBox(this))
    , insert_button (new QPushButton("INSERT", this))
    , remove_button (new QPushButton("REMOVE", this))
    , print_button (new QPushButton("PRINT", this))
    , query_layout (new QGridLayout())
    , query_edit (new QTextEdit(this))
    , exec_button (new QPushButton("EXEC", this))
    , clr_button (new QPushButton("CLR", this))
    , filter_search_tab_widget(new QTabWidget(this))
    , filter_tab_widget(new QWidget(this))
    , search_tab_widget(new QWidget(this))
{
    table_layout->addLayout(table_button_layout);
    table_button_layout->addWidget(table_label);
    table_button_layout->addWidget(table_combobox);
    table_button_layout->addWidget(insert_button);
    table_button_layout->addWidget(remove_button);
    table_button_layout->addWidget(print_button);

    table_layout->addWidget(table_view);
    table_layout->addLayout(query_layout);

    query_layout->addWidget(query_edit, 0, 0, 2, 1);
    query_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    query_edit->setWordWrapMode(QTextOption::WrapMode::WordWrap);
    query_layout->addWidget(exec_button, 0, 1);
    query_layout->addWidget(clr_button, 1, 1);

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

    connect(exec_button, &QPushButton::clicked, [this](){
        QSqlQuery qry(MainWindow::data_base);
        qry.prepare(query_edit->toPlainText());
        if(!qry.exec())
        {
            MainWindow::ThrowError("Your query is... is.. is wrong! ＞︿＜");
            return;
        }
        query_model->setQuery(QSqlQuery(qry));
        filter_model->setSourceModel(query_model);
        changeFilterSearchTabs();
        is_custom_query = true;
    });
    connect(clr_button, &QPushButton::clicked, [this](){
        query_edit->setText(""); changeTable(current_table);
        filter_model->setSourceModel(model);
        changeFilterSearchTabs();
        is_custom_query = false;
    });
    connect(insert_button, &QPushButton::clicked, this, &DataTabWidget::insertRow);
    connect(remove_button, &QPushButton::clicked, this, &DataTabWidget::removeRow);
    connect(print_button, &QPushButton::clicked, this, &DataTabWidget::printTable);
    table_combobox->setSizePolicy(QSizePolicy::Expanding, table_combobox->sizePolicy().verticalPolicy());
    table_combobox->addItems(MainWindow::data_base.tables());
    table_combobox->setCurrentText("heroes");
    connect(table_combobox, &QComboBox::currentTextChanged, this, &DataTabWidget::changeTable);

    filter_model->setSourceModel(model);
    filter_model->setDynamicSortFilter(false);

    search_model->setSourceModel(filter_model);
    search_model->setDynamicSortFilter(false);

    table_view->setModel(search_model);
    table_view->setSortingEnabled(true);

    table_view->setItemDelegate(new DataItemDelegate(model, table_view));

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

    QString query_test = "INSERT INTO " + model->tableName() + " VALUES (";
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
            for(size_t i = 1; i < model->columnCount() - 1; ++i)
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
            for(size_t i = 3; i < model->columnCount() - 1; ++i)
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
            for(size_t i = 1; i < model->columnCount() - 1; ++i)
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
            for(size_t i = 3; i < model->columnCount() - 1; ++i)
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
            for(size_t i = 1; i < model->columnCount() - 1; ++i)
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
            for(size_t i = 2; i < model->columnCount() - 1; ++i)
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
            for(size_t i = 1; i < model->columnCount() - 1; ++i)
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
    changeTable(model->tableName());
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
                QString query_text = "delete from " + model->tableName() + " where ";
                size_t i = 0;
                for(; i < model->columnCount(); ++i)
                {
                    QVariant value = model->data(model->index(index.row(), i));
                    if(value.type() == QVariant::Type::DateTime)
                        continue;
                    QString str_value = value.type() == QVariant::Type::String ?
                                        "'" + value.toString() + "'" : value.toString();
                    query_text += HEADER(model, i) + " = " + str_value + " AND ";
                }
                QSqlQuery qry;
                query_text.chop(4);
                qry.prepare(query_text);
                qry.exec();

                changeTable(model->tableName());
            }
            return;
        }
    MainWindow::ThrowError("Invalid row. Sorry /(ㄒoㄒ)/~~");
}

void DataTabWidget::printTable()
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Export PDF", QString(), "*.pdf");
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::NativeFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Orientation::Landscape);
    printer.setOutputFileName(fileName);

    QString title = is_custom_query ? "data" : current_table;
    title[0] = title[0].toUpper();

    QString str;
    QTextStream out(&str);
    const int rowCount = table_view->model()->rowCount();
    const int columnCount = table_view->model()->columnCount();
    out <<  "<html>\n<head>\n <meta Content=\"Text/html; charset=Windows-936\">\n"
        <<  QString("<title>%1</title>\n").arg("Dota 2 Assistant : " + title)
        <<  "</head>\n"
            "<body bgcolor=#ffffff link=#5000A0>\n"
            "<table border=1 cellspacing=0 cellpadding=2>\n";
    // headers
    out << "<center><h1>Dota 2 Assistance</h1>"
        << "<p><h4>" + title + "</h4></p><p>&nbsp;</p>";
    out << "<tr bgcolor=#f0f0f0>";
    for (int column = 0; column < columnCount; column++)
        if (!table_view->isColumnHidden(column))
            out << QString("<th>%1</th>").arg(table_view->model()->headerData(column, Qt::Horizontal).toString());
    out << "</tr>\n";
    // data table
    for (int row = 0; row < rowCount; row++)
    {
        out << "<tr>";
        for (int column = 0; column < columnCount; column++)
        {
            if (!table_view->isColumnHidden(column))
            {
                QString data = table_view->model()->data(table_view->model()->index(row, column)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString(" "));
            }
        }out << "</tr>\n";
    }out <<  "</table></center>";
    out <<"<p>&nbsp;</p><p>&nbsp;</p><div style=\"position: absolute; bottom: 5px; align-content: right; \">"
        <<"Creation date: " << QDate::currentDate().toString("dd.MM.yyyy")
        <<"</div>"
        <<"</body>\n</html>\n";
    QTextDocument doc;
    doc.setHtml(str);
    doc.setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number
    doc.print(&printer);
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

    QSqlQueryModel* current_model = static_cast<QSqlQueryModel*>(table_view->model());

    size_t i = 0;
    for(; i < current_model->columnCount(); ++i)
    {
        filter_layout->addWidget(new QLabel(current_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString()), i, 0);
        search_layout->addWidget(new QLabel(current_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString()), i, 0);
        QWidget* filter_widget = nullptr;
        QWidget* search_widget = nullptr;
        auto filter_func = std::bind(&FilterProxyModel::setExpression, filter_model, i,std::placeholders::_1);
        auto search_func = std::bind(&SearchProxyModel::setExpression, search_model, i,std::placeholders::_1);

        switch (current_model->data(current_model->index(0, i)).type())
        {
            case QVariant::Type::Int:
            case QVariant::Type::Double: {
                filter_widget = new QSpinBox();
                connect(static_cast<QSpinBox*>(filter_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), filter_func);
                search_widget = new QSpinBox();
                connect(static_cast<QSpinBox*>(search_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), search_func);
                break;
            }
            case QVariant::Type::String: {
                filter_widget = new QLineEdit();
                connect(static_cast<QLineEdit*>(filter_widget), &QLineEdit::textChanged, filter_func);
                search_widget = new QLineEdit();
                connect(static_cast<QLineEdit*>(search_widget), &QLineEdit::textChanged, search_func);
                break;
            }
            case TY_QT_FOR_CHAR: {
                filter_widget = new QSpinBox();
                connect(static_cast<QSpinBox*>(filter_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), filter_func);
                search_widget = new QSpinBox();
                connect(static_cast<QSpinBox*>(search_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), search_func);
                break;
            }
            case QVariant::Type::Date: {
                filter_widget = new QDateEdit();
                connect(static_cast<QDateEdit*>(filter_widget), &QDateEdit::dateChanged, [this, i]( const QDate &date)
                {
                    QString month_zero = date.month() > 9 ? "" : "0";
                    QString day_zero = date.day() > 9 ? "" : "0";
                    this->filter_model->setExpression(i, QString::number(date.year()) + '-' +
                    month_zero + QString::number(date.month()) + '-' +
                    day_zero + QString::number(date.day()));
                });
                search_widget = new QDateEdit();
                connect(static_cast<QDateEdit*>(search_widget), &QDateEdit::dateChanged, [this, i]( const QDate &date)
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

        if(filter_widget != nullptr && search_widget != nullptr)
        {
            filter_layout->addWidget(filter_widget, i, 1);
            QPushButton* filter_clearButton = new QPushButton("CLR");
            filter_layout->addWidget(filter_clearButton, i, 2);
            connect(filter_clearButton, &QPushButton::clicked, [this, i](){this->filter_model->setExpression(i, "");});

            search_layout->addWidget(search_widget, i, 1);
            QPushButton* search_clr_button = new QPushButton("CLR");
            search_layout->addWidget(search_clr_button, i, 2);
            connect(search_clr_button, &QPushButton::clicked, [this, i](){this->search_model->setExpression(i, "");});
        }
    }

    QPushButton* searchButton = new QPushButton("CLR");
    search_layout->addWidget(searchButton, i, 0, 1, 3);
    connect(searchButton, &QPushButton::clicked, [this](){this->search_model->Search();});
}

void DataTabWidget::changeTable(const QString& table_name)
{
    current_table = table_name;
    model->setTable(table_name);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);;
    model->select();

    changeFilterSearchTabs();
}

