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

    connect(exec_button, &QPushButton::clicked, this, &DataTabWidget::execQuery);
    connect(clr_button, &QPushButton::clicked, this, &DataTabWidget::clrQuery);
    connect(insert_button, &QPushButton::clicked, this, &DataTabWidget::insertRow);
    connect(remove_button, &QPushButton::clicked, this, &DataTabWidget::removeRow);
    connect(print_button, &QPushButton::clicked, this, &DataTabWidget::printTable);

    table_combobox->setSizePolicy(QSizePolicy::Expanding, table_combobox->sizePolicy().verticalPolicy());
    table_combobox->addItems(MainWindow::data_base.tables());
    table_combobox->setCurrentText("heroes");

    auto color_exec_func = std::bind(&QPushButton::setStyleSheet, exec_button, QString(""));
    connect(table_combobox, &QComboBox::currentTextChanged, this, &DataTabWidget::changeTable);
    connect(table_combobox, &QComboBox::currentTextChanged, this, color_exec_func);

    filter_model->setSourceModel(model);
    filter_model->setDynamicSortFilter(false);

    search_model->setSourceModel(filter_model);
    search_model->setDynamicSortFilter(false);

    table_view->setModel(search_model);
    table_view->setSortingEnabled(true);

    table_view->setItemDelegate(new DataItemDelegate(model, table_view, table_view));

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


    QVector<QWidget*> widgets;

    QPushButton* button = new QPushButton("Insert!");
    connect(button, &QPushButton::clicked, dialog, &QDialog::close);
    dialog->setLayout(layout);


    int i = 0;
    for(; i < model->columnCount(); ++i)
    {
        if(model->headerData(i, Qt::Orientation::Horizontal, Qt::UserRole + 3).toString() == "primary")
        {
            QLabel* label = new QLabel(model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
            layout->addWidget(label);

            if(model->headerData(i, Qt::Orientation::Horizontal, Qt::UserRole + 1).toString() == "combo")
            {
                QComboBox* box = new QComboBox();
                box->addItems(model->headerData(i, Qt::Orientation::Horizontal, Qt::UserRole + 2).value<QStringList>());
                layout->addWidget(box);
                widgets.push_back(box);
                continue;
            }

            QVariant::Type type = model->data(model->index(0, i)).type();
            switch(type)
            {
            case QVariant::Type::String:
            {
                QLineEdit* edit = new QLineEdit();
                layout->addWidget(edit);
                widgets.push_back(edit);
                break;
            }
            case QVariant::Type::Double:
            case QVariant::Type::Int:
            {
                // Value is bool type
                if(model->headerData(i, Qt::Orientation::Horizontal, Qt::UserRole + 1) == "boolean")
                {
                    QRadioButton* edit = new QRadioButton();
                    layout->addWidget(edit);
                    widgets.push_back(edit);
                    break;
                }

                // Value is int/double type
                QSpinBox* edit = new QSpinBox();
                layout->addWidget(edit);
                widgets.push_back(edit);
                break;
            }
            }
        }
    }

    connect(button, &QPushButton::clicked, [&widgets, &query_test, this, &was_closed]()
    {
        int widgets_i = 0;
        for(size_t i = 0; i < model->columnCount(); ++i)
        {
            QString value = "DEFAULT";
            if(model->headerData(i, Qt::Orientation::Horizontal, Qt::UserRole + 3).toString() == "primary")
            {
                QByteArray n = widgets[widgets_i]->metaObject()->userProperty().name();
                value = MainWindow::VariantToSql(widgets[widgets_i]->property(n));

                ++widgets_i;
            }
            query_test += value + ", ";
        }
        query_test.chop(2);
        query_test += ")";

        was_closed = false;
    });

    layout->addWidget(button);
    dialog->exec();
    if(was_closed)
        return;

    QSqlQuery qry;
    qry.prepare(query_test);

    if(!qry.exec())
        MainWindow::ThrowError(qry.lastError().text());

    changeTable(model->tableName());
}

void DataTabWidget::removeRow()
{
    QModelIndex index = table_view->currentIndex();
    int right_index = table_view->model()->headerData(index.row(), Qt::Vertical, 0).toInt() - 1;
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
                    QVariant value = model->data(model->index(right_index, i));
                    QString str_value = MainWindow::VariantToSql(value);
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

void DataTabWidget::execQuery()
{
    QSqlQuery qry(MainWindow::data_base);
    qry.prepare(query_edit->toPlainText());
    if(!qry.exec())
    {
        MainWindow::ThrowError(qry.lastError().text());
        return;
    }

    QRegExp reg_exp("\\* ?(from) ([^, ]*),");

    reg_exp.indexIn(query_edit->toPlainText());
    QStringList list = reg_exp.capturedTexts();

    // If user enters "select * from heroes, skills" program will crush, so do not let him do it
    auto iter = list.cbegin();
    while(iter != list.cend())
    {
        if (*iter != QString(""))
        {
            MainWindow::ThrowError("Do not use '*' from two or more tables, please. (￣m￣）");
            return;
        }
        ++iter;
    }

    query_model->setQuery(QSqlQuery(qry));

    exec_button->setStyleSheet("background-color: lime");

    is_custom_query = true;
    changeTable(current_table);
}

void DataTabWidget::clrQuery()
{
    exec_button->setStyleSheet("");

    is_custom_query = false;
    changeTable(current_table);
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
        QLabel* filter_label = new QLabel(current_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
        filter_label->setMinimumHeight(40);
        filter_label->setAlignment(Qt::AlignCenter);
        filter_layout->addWidget(filter_label, i, 0);

        QLabel* search_label = new QLabel(current_model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString());
        search_label->setMinimumHeight(40);
        search_label->setAlignment(Qt::AlignCenter);
        search_layout->addWidget(search_label, i, 0);

        QWidget* filter_widget = nullptr;
        QWidget* search_widget = nullptr;
        auto filter_func = std::bind(&FilterProxyModel::setExpression, filter_model, i,std::placeholders::_1);
        auto search_func = std::bind(&SearchProxyModel::setExpression, search_model, i,std::placeholders::_1);
        auto color_filter_func = std::bind(&QLabel::setStyleSheet, filter_label, QString("background-color: lime"));
        auto color_search_func = std::bind(&QLabel::setStyleSheet, search_label, QString("background-color: lime"));

        switch (current_model->data(current_model->index(0, i)).type())
        {
            case QVariant::Type::Int:
            case QVariant::Type::Double: {
                // Value is bool type
                if(model->headerData(i, Qt::Orientation::Horizontal, Qt::UserRole + 1) == "boolean")
                {
                    filter_widget = new QRadioButton();
                    connect(static_cast<QRadioButton*>(filter_widget), &QRadioButton::toggled, [this, i]( const bool &exp)
                    { this->filter_model->setExpression(i, exp ? "1" : "0"); });
                    connect(static_cast<QRadioButton*>(filter_widget), &QRadioButton::toggled, color_filter_func);
                    search_widget = new QRadioButton();
                    connect(static_cast<QRadioButton*>(search_widget), &QRadioButton::toggled, [this, i]( const bool &exp)
                    { this->search_model->setExpression(i, exp ? "1" : "0"); });
                    connect(static_cast<QRadioButton*>(search_widget), &QRadioButton::toggled, color_search_func);
                    break;
                }

                //Value is int / double type
                filter_widget = new QSpinBox();
                connect(static_cast<QSpinBox*>(filter_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), filter_func);
                connect(static_cast<QSpinBox*>(filter_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), color_filter_func);
                search_widget = new QSpinBox();
                connect(static_cast<QSpinBox*>(search_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), search_func);
                connect(static_cast<QSpinBox*>(search_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), color_search_func);
                break;
            }
            case QVariant::Type::String: {
                filter_widget = new QLineEdit();
                connect(static_cast<QLineEdit*>(filter_widget), &QLineEdit::textChanged, filter_func);
                connect(static_cast<QLineEdit*>(filter_widget), &QLineEdit::textChanged, color_filter_func);
                search_widget = new QLineEdit();
                connect(static_cast<QLineEdit*>(search_widget), &QLineEdit::textChanged, search_func);
                connect(static_cast<QLineEdit*>(search_widget), &QLineEdit::textChanged, color_search_func);
                break;
            }
            case TY_QT_FOR_CHAR: {
                filter_widget = new QSpinBox();
                connect(static_cast<QSpinBox*>(filter_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), filter_func);
                connect(static_cast<QSpinBox*>(filter_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), color_filter_func);
                search_widget = new QSpinBox();
                connect(static_cast<QSpinBox*>(search_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), search_func);
                connect(static_cast<QSpinBox*>(search_widget), QOverload<const QString&>::of(&QSpinBox::valueChanged), color_search_func);
                break;
            }
            case QVariant::Type::Date: {
                filter_widget = new QDateEdit();
                connect(static_cast<QDateEdit*>(filter_widget), &QDateEdit::dateChanged, [this, i]( const QDate &date)
                { this->filter_model->setExpression(i, date.toString("(yyyy-)|(-MM-)|(-dd)"));});
                connect(static_cast<QDateEdit*>(filter_widget), &QDateEdit::dateChanged, color_filter_func);
                search_widget = new QDateEdit();
                connect(static_cast<QDateEdit*>(search_widget), &QDateEdit::dateChanged, [this, i]( const QDate &date)
                { this->search_model->setExpression(i, date.toString("yyyy-MM-dd")); });
                connect(static_cast<QDateEdit*>(search_widget), &QDateEdit::dateChanged, color_search_func);
                break;
            }
        }

        if(filter_widget != nullptr && search_widget != nullptr)
        {
            filter_layout->addWidget(filter_widget, i, 1);
            QPushButton* filter_clearButton = new QPushButton("CLR");
            filter_layout->addWidget(filter_clearButton, i, 2);
            connect(filter_clearButton, &QPushButton::clicked, [this, i,filter_label]()
            {
                filter_label->setStyleSheet("");
                this->filter_model->setExpression(i, "");
            });

            search_layout->addWidget(search_widget, i, 1);
            QPushButton* search_clr_button = new QPushButton("CLR");
            search_layout->addWidget(search_clr_button, i, 2);
            connect(search_clr_button, &QPushButton::clicked, [this, i, search_label]()
            {
                search_label->setStyleSheet("");
                this->search_model->setExpression(i, "");
            });
        }
    }

    QWidget* filter_dummy_widget = new QWidget();
    filter_dummy_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget* search_dummy_widget = new QWidget();
    search_dummy_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    filter_layout->addWidget(filter_dummy_widget, i, 0, 1, 2);
    search_layout->addWidget(search_dummy_widget, i, 0, 1, 3);

    QPushButton* searchButton = new QPushButton("CLR");
    search_layout->addWidget(searchButton, i + 1, 0, 1, 3);
    connect(searchButton, &QPushButton::clicked, [this](){this->search_model->Search();});
}

void DataTabWidget::addHeaderData(QString& source, QRegExp reg_exp, void (*data_handler)(QStringList&, int i,QSqlTableModel*))
{
    int pos = reg_exp.indexIn(source);
    while(pos != -1)
    {
        QStringList list = reg_exp.capturedTexts();

        int i = 0;
        for(; i < model->columnCount(); ++i)
        {
            if(HEADER(model, i) == list[1])
                break;
        }
        data_handler(list, i, model);
        pos = reg_exp.indexIn(source, pos + 1);
    }
}

void DataTabWidget::fillHeaderData()
{
    // ADDING DATA TO HEADER FROM DB TO VIEW
    QSqlQuery qry;
    qry.exec("SHOW CREATE TABLE " + current_table);
    qry.next();
    QString show_create_table = qry.value(1).toString().toLower();
    qDebug()<<show_create_table;

    // PRIMARY KEYS
    QRegExp primary_reg_exp("primary key \\(([^\\)]*)\\)");
    addHeaderData(show_create_table, primary_reg_exp, [](QStringList& list, int i, QSqlTableModel* model)
    {
        list[1].remove('\`');
        QStringList primary_list = list[1].split(',');

        for(int i = 0; i < primary_list.size(); ++i)
        {
            int j = 0;
            for(; j < model->columnCount(); ++j)
            {
                if(HEADER(model, j) == primary_list[i])
                    break;
            }
            model->setHeaderData(j, Qt::Orientation::Horizontal, "primary", Qt::UserRole + 3);
        }
    });

    // FOREIGN KEYS
    QRegExp foreign_reg_exp("foreign key \\(`([^`]*)`\\) references `([^`]*)` \\(`([^`]*)`\\)");
    addHeaderData(show_create_table, foreign_reg_exp, [](QStringList& list, int i, QSqlTableModel* model)
    {
        model->setHeaderData(i, Qt::Orientation::Horizontal, "combo", Qt::UserRole + 1);
        QSqlQuery select_query;
        select_query.exec("select " + list[3] + " from " + list[2]);
        QStringList combo_list;
        while(select_query.next())
            combo_list.append(select_query.value(0).toString());

        model->setHeaderData(i, Qt::Orientation::Horizontal, combo_list, Qt::UserRole + 2);
    });

    // ENUMS
    QRegExp enums_reg_exp("`([^`]*)` enum\\(([^\\)]*)\\)");
    addHeaderData(show_create_table, enums_reg_exp, [](QStringList& list, int i, QSqlTableModel* model)
    {
        model->setHeaderData(i, Qt::Orientation::Horizontal, "combo", Qt::UserRole + 1);
        list[2].remove('\'');
        QStringList combo_list = list[2].split(',');
        model->setHeaderData(i, Qt::Orientation::Horizontal, combo_list, Qt::UserRole + 2);
    });

   // TINYINTS
    QRegExp tinyints_reg_exp("`([^`]*)` tinyint\\(1\\)");
    addHeaderData(show_create_table, tinyints_reg_exp, [](QStringList& list, int i, QSqlTableModel* model)
    {
        model->setHeaderData(i, Qt::Orientation::Horizontal, "boolean", Qt::UserRole + 1);
        QStringList combo_list {"true", "false"};
        model->setHeaderData(i, Qt::Orientation::Horizontal, combo_list, Qt::UserRole + 2);
    });

    // BOOLEANS
     QRegExp booleans_reg_exp("`([^`]*)` tinyint\\(1\\)");
     addHeaderData(show_create_table, booleans_reg_exp, [](QStringList& list, int i, QSqlTableModel* model)
     {
         model->setHeaderData(i, Qt::Orientation::Horizontal, "boolean", Qt::UserRole + 1);
         QStringList combo_list {"true", "false"};
         model->setHeaderData(i, Qt::Orientation::Horizontal, combo_list, Qt::UserRole + 2);
     });

    // GENERATED
    QRegExp genereate_reg_exp("`([^`]*)` ([^ ]*) generated");
    addHeaderData(show_create_table, genereate_reg_exp, [](QStringList& list, int i, QSqlTableModel* model)
    {
        model->setHeaderData(i, Qt::Orientation::Horizontal, "const", Qt::UserRole + 1);
    });

    // PICTURES
    QRegExp picture_reg_exp("(picture_path)");
    addHeaderData(show_create_table, picture_reg_exp, [](QStringList& list, int i, QSqlTableModel* model)
    {
        model->setHeaderData(i, Qt::Orientation::Horizontal, "picture", Qt::UserRole + 1);
    });
}

void DataTabWidget::changeTable(const QString& table_name)
{
    current_table = table_name;
    model->setTable(table_name);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    is_custom_query ? filter_model->setSourceModel(query_model)
                    : filter_model->setSourceModel(model);


    fillHeaderData();
    changeFilterSearchTabs();

    is_custom_query = false;
}

