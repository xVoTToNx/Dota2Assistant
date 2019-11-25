#include "icons_tab_widget.h"
#include "main_window.h"
#include <cmath>
#include <algorithm>

IconsTabWidget::IconsTabWidget(QString&& name, QWidget *parent)
    : QWidget(parent)
    , current_table(Icon::hero)
    , name(name)
    , model (new QSqlTableModel(this, MainWindow::data_base))
    , filter_model(new FilterProxyModel(this))
    , main_layout (new QHBoxLayout(this))
    , button_layout (new QVBoxLayout(this))
    , table_layout (new QVBoxLayout(this))
    , another_another_layout (new QHBoxLayout(this))
    , icons_layout (new QGridLayout(this))
    , clear_button (new QPushButton("CLR", this))
    , search_line_edit (new QLineEdit(this))
{
    connect(filter_model, &QSqlTableModel::dataChanged, [this](){this->updateTable(this->current_table);});

    filter_model->setSourceModel(model);
    filter_model->setDynamicSortFilter(false);

    QLabel* label = new QLabel("Search: ", this);
    label->setAlignment(Qt::AlignRight);


    auto func = std::bind(&FilterProxyModel::setExpression, filter_model, 0 ,std::placeholders::_1);
    connect(search_line_edit, &QLineEdit::textChanged, func);
    connect(search_line_edit, &QLineEdit::textChanged, [this](){this->updateTable(this->current_table);});
    connect(clear_button, &QPushButton::clicked, [this](){this->filter_model->setExpression(0, ""); this->search_line_edit->setText("");});

    another_another_layout->addWidget(label);
    another_another_layout->addWidget(search_line_edit);
    another_another_layout->addWidget(clear_button);

    table_layout->addLayout(another_another_layout);
    table_layout->addLayout(icons_layout);

    QPushButton* heroes_button = new QPushButton("Heroes");
    QPushButton* items_button = new QPushButton("Items");
    QPushButton* teams_button = new QPushButton("Teams");

    connect(heroes_button, &QPushButton::clicked, [this](){updateTable(Icon::hero);});
    connect(items_button, &QPushButton::clicked, [this](){updateTable(Icon::item);});
    connect(teams_button, &QPushButton::clicked, [this](){updateTable(Icon::team);});

    button_layout->addWidget(heroes_button);
    button_layout->addWidget(items_button);
    button_layout->addWidget(teams_button);

    main_layout->addLayout(button_layout);
    main_layout->addLayout(table_layout);


    updateTable(current_table);
    setLayout(main_layout);
}

void IconsTabWidget::updateTable(Icon icon)
{
    QString table_name;
    current_table = icon;
    switch(icon)
    {
    case Icon::hero:
        table_name = "test";
        break;
    case Icon::item:
        table_name = "test2";
        break;
    case Icon::team:
        table_name = "testc";
        break;
    }

    model->setTable(table_name);
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();

    QLayoutItem* item;
    while ((item = icons_layout->takeAt(0)) != nullptr)
    {delete item->widget();delete item;}

    int columns = std::min(7, std::max(3, static_cast<int>(std::sqrt(filter_model->rowCount()))));

    for(size_t i = 0; i < filter_model->rowCount(); ++i)
    {
        QVariant data = filter_model->data(filter_model->index(i,0));
        QPushButton* button = new QPushButton(data.toString());
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // Switch coonect to different creation window func
        // Use query
        switch(icon)
        {
        case Icon::hero:
            connect(button, &QPushButton::clicked, [](){
                auto icon = HeroIcon::CreateMe();
                icon->setAttribute(Qt::WA_DeleteOnClose);
                icon->show(); });
            break;
        case Icon::item:
            connect(button, &QPushButton::clicked, [](){
                auto icon = ItemIcon::CreateMe();
                icon->setAttribute(Qt::WA_DeleteOnClose);
                icon->show(); });
            break;
        case Icon::team:
            connect(button, &QPushButton::clicked, [](){
                auto icon = TeamIcon::CreateMe();
                icon->setAttribute(Qt::WA_DeleteOnClose);
                icon->show(); });
            break;
        }
        icons_layout->addWidget(button, 1 + i / columns, i % columns);
    }
}
