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
    , button_layout (new QVBoxLayout())
    , table_layout (new QVBoxLayout())
    , another_another_layout (new QHBoxLayout())
    , icons_layout (new QGridLayout())
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
    heroes_button->setMinimumSize(150, 60);
    QPushButton* items_button = new QPushButton("Items");
    items_button->setMinimumSize(150, 60);
    QPushButton* teams_button = new QPushButton("Teams");
    teams_button->setMinimumSize(150, 60);
    QWidget* dummy_widget = new QWidget();
    dummy_widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    connect(heroes_button, &QPushButton::clicked, [this](){updateTable(Icon::hero);});
    connect(items_button, &QPushButton::clicked, [this](){updateTable(Icon::item);});
    connect(teams_button, &QPushButton::clicked, [this](){updateTable(Icon::team);});

    button_layout->addWidget(heroes_button);
    button_layout->addWidget(items_button);
    button_layout->addWidget(teams_button);
    button_layout->addWidget(dummy_widget);

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
        table_name = "heroes";
        break;
    case Icon::item:
        table_name = "items";
        break;
    case Icon::team:
        table_name = "teams";
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
        QString data = filter_model->data(filter_model->index(i,0)).toString();
        QString icon_path = filter_model->data(filter_model->index(i, 1)).toString();
        QToolButton* button = new QToolButton();

        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QVBoxLayout* button_icon_layout = new QVBoxLayout(button);
        QLabel* name = new QLabel(data, button);
        name->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
        name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        QIcon icon_icon(icon_path);

        if(!icon_icon.isNull())
        {
            button->setIcon(icon_icon);
            button->setIconSize({150,150});
        }

        button_icon_layout->addWidget(name);
        button_icon_layout->setAlignment(Qt::AlignHCenter);

        switch(icon)
        {
        case Icon::hero:
            connect(button, &QPushButton::clicked, [data](){
                auto icon = HeroIcon::CreateMe(data);
                icon->setAttribute(Qt::WA_DeleteOnClose);
                icon->show(); });
            break;
        case Icon::item:
            connect(button, &QPushButton::clicked, [data](){
                auto icon = ItemIcon::CreateMe(data);
                icon->setAttribute(Qt::WA_DeleteOnClose);
                icon->show(); });
            break;
        case Icon::team:
            connect(button, &QPushButton::clicked, [data](){
                auto icon = TeamIcon::CreateMe(data);
                icon->setAttribute(Qt::WA_DeleteOnClose);
                icon->show(); });
            break;
        }
        icons_layout->addWidget(button, 1 + i / columns, i % columns);
    }
}
