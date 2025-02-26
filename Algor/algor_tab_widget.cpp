#include "algor_tab_widget.h"
#include "main_window.h"

AlgorTabWidget::AlgorTabWidget(QString &&name, MainWindow* main_window, QWidget *parent)
    : QWidget(parent)
    , main_window (main_window)
    , tab_name (name)
    , current_team_name("")
    , current_hero_index(-1)
    , layout (new QVBoxLayout())
    , team_label_layout (new QHBoxLayout)
    , choose_hero_button (new QToolButton(this))
    , algor_hero_button (new QPushButton("ALG", this))
    , clear_hero_button (new QPushButton("CLR", this))
    , info_hero_button (new QPushButton("INFO", this))
    , team_label (new QLabel("Team: ", this))
    , team_name (new QComboBox(this))
    , heroes_layout (new QHBoxLayout())
    , heroes_icons_layout (new QGridLayout())
    , heroes_buttons_layout (new QVBoxLayout())
    , rand_button (new QPushButton("RAND", this))
    , clear_button (new QPushButton("CLR", this))
    , roles_button (new QToolButton(this))
    , hero_roles_list (new QListWidget(this))
    , stats (new QCustomPlot())
    , bar_group (new QCPBarsGroup(stats))
    , current_team_bar (new QCPBars(stats->xAxis, stats->yAxis))
    , desired_team_bar (new QCPBars(stats->xAxis, stats->yAxis))
    , print_button (new QPushButton("PRINT", this))
{
    choose_hero_button->setText("PICK");
    choose_hero_button->setCheckable(true);

    for(int i = 0; i < 5; ++i)
        team_heroes.push_back(nullptr);

    team_label_layout->addWidget(choose_hero_button);
    team_label_layout->addWidget(algor_hero_button);
    team_label_layout->addWidget(clear_hero_button);
    team_label_layout->addWidget(info_hero_button);
    team_label_layout->addWidget(team_label);
    team_label_layout->addWidget(team_name);

    choose_hero_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    choose_hero_button->setFixedHeight(30);
    algor_hero_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    algor_hero_button->setFixedHeight(30);
    clear_hero_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    clear_hero_button->setFixedHeight(30);
    info_hero_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    info_hero_button->setFixedHeight(30);
    team_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    team_label->setFixedWidth(50);
    team_label->setAlignment(Qt::AlignRight);
    team_name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    team_name->setFixedHeight(30);

    QSqlQuery qry;
    qry.prepare("select team_name from teams");
    qry.exec();
    while(qry.next())
        team_name->addItem(qry.value(0).toString());

    heroes_buttons_layout->addWidget(rand_button);
    heroes_buttons_layout->addWidget(clear_button);
    heroes_buttons_layout->addWidget(roles_button);
    heroes_buttons_layout->addWidget(print_button);
    heroes_layout->addLayout(heroes_icons_layout);
    heroes_layout->addLayout(heroes_buttons_layout);
    layout->addLayout(team_label_layout);
    layout->addLayout(heroes_layout);
    layout->addWidget(hero_roles_list);
    layout->addWidget(stats);

    rand_button->setFixedSize(150, 30);
    clear_button->setFixedSize(150, 30);
    print_button->setFixedSize(150, 30);
    roles_button->setFixedSize(150, 30);
    roles_button->setText("ROLES");
    roles_button->setCheckable(true);
    roles_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    connect(choose_hero_button, &QPushButton::clicked, this, &AlgorTabWidget::pickHero);
    connect(algor_hero_button, &QPushButton::clicked, this, &AlgorTabWidget::randHero);
    connect(clear_hero_button, &QPushButton::clicked, this, &AlgorTabWidget::clrHero);
    connect(info_hero_button, &QPushButton::clicked, this, &AlgorTabWidget::infoHero);

    connect(rand_button, &QPushButton::clicked, this, &AlgorTabWidget::MAGIC);
    connect(clear_button, &QPushButton::clicked, this, &AlgorTabWidget::CLR);
    connect(roles_button, &QToolButton::clicked, this, &AlgorTabWidget::openRolesForm);
    connect(print_button, &QPushButton::clicked, this, &AlgorTabWidget::PRINT);
    connect(team_name, &QComboBox::currentTextChanged, this, &AlgorTabWidget::updateTable);

    connect(main_window, &MainWindow::ModeChanged, this, &AlgorTabWidget::changeMode);

    current_team_name = team_name->itemText(0);
    configureStatsPlot();

    updateTable(team_name->itemText(0));
    setLayout(layout);
}

AlgorTabWidget::~AlgorTabWidget()
{
    for(int i = 0; i < 5; ++i)
        delete team_heroes[i];
}

void AlgorTabWidget::replotStatsPlot(int is_data_changed)
{
    if(is_data_changed & 0x0001)
        current_team_bar->setData(ticks, current_role_data);
    if(is_data_changed & 0x0002)
        desired_team_bar->setData(ticks, desired_role_data);
    if(is_data_changed & 0x0004)
    {
        QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
        textTicker->addTicks(ticks, labels);
        stats->xAxis->setTicker(textTicker);
    }

    stats->rescaleAxes();
    stats->replot();
}

void AlgorTabWidget::setRolePriorityIndex(int priority, int index)
{
    int count = role_priority_indexes.count();
    if(priority >= count)
    {
        for(int i = 0; i < priority - count + 1; ++i)
        {
            role_priority_indexes.push_back(-1);
        }
    }

    // Change labels to see priorities on the gistogram
    int labels_index = role_priority_indexes[priority];

    if(index == -1 && labels_index != -1)
        labels[labels_index] = labels[labels_index].split(' ').count() > 1 ? labels[labels_index].split(' ')[1] : labels[labels_index];
    else if (index != -1)
    {
        labels[index] = QString::number(priority + 1) + ' ' + labels[index];
    }

    replotStatsPlot(4);

    role_priority_indexes[priority] = index;
}

void AlgorTabWidget::updateHeroesList()
{
    hero_roles_list->clear();
    QLayoutItem* item;
    while ((item = heroes_icons_layout->takeAt(0)) != nullptr)
    {}

    for(int i = 0; i < 5; ++i)
    {
        heroes_icons_layout->addWidget(team_heroes[i], 0, i);
        if(!team_heroes[i]->property("is_dummy").toBool())
        {
            QString hero_name = team_heroes[i]->property("hero_name").toString();
            QString icon_path = team_heroes[i]->property("picture_path").toString();
            updateHeroRolesList(hero_name, icon_path);
        }
    }
}

void AlgorTabWidget::configureHeroesList()
{
    QSqlQuery qry;
    qry.prepare("select hero_name from team_heroes where team_name = '" + current_team_name + "' ");
    qry.exec();
    int i = 0;
    while(i < 5)
    {
        QToolButton* button = nullptr;

        if(qry.next())
        {
            QString hero_name = qry.value("hero_name").toString();
            button = createHeroToolButton(hero_name, i);
        }
        else
        {
            button = createDummyToolButton(i);
        }

        if(team_heroes[i])
            delete team_heroes[i];

        team_heroes[i] = button;

        ++i;
    }

    updateHeroesList();
}

void AlgorTabWidget::changeCurrentHero(int new_hero_index)
{
    if(current_hero_index != -1)
        team_heroes[current_hero_index]->setStyleSheet(team_heroes[current_hero_index]->property("style").toString());

    current_hero_index = new_hero_index;
    team_heroes[current_hero_index]->setStyleSheet("border: 4px inset lime");
}

void AlgorTabWidget::updateHeroRolesList(QString &hero_name, QString& picture_path)
{
    QSqlQuery qry;
    qry.prepare("select role_name, degree_of_affiliation from hero_roles where hero_name = '" + hero_name + "' order by role_name");
    qry.exec();

    QWidget* list_widget = new QWidget(this);
    QGridLayout* list_item_layout = new QGridLayout();
    list_widget->setLayout(list_item_layout);

    QLabel* pic = new QLabel();
    pic->setFixedSize(100,100);
    QPixmap pixmap(picture_path);
    if(!pixmap.isNull())
        pic->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));
    else
        pic->setText(hero_name);

    list_item_layout->addWidget(pic, 0, 0, qry.size() / 4 + 1, 1);

    int i = 0;
    while(qry.next())
    {
        QString role_name = qry.value("role_name").toString();
        role_name[0] = role_name[0].toUpper();

        QLabel* role_label = new QLabel(role_name + ": " + qry.value("degree_of_affiliation").toString());
        role_label->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
        list_item_layout->addWidget(role_label, i / 4, i % 4 + 1);

        ++i;
    }

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint({400, 120});
    hero_roles_list->addItem(item);
    hero_roles_list->setItemWidget(item, list_widget);
}

void AlgorTabWidget::updateStatsPlot(bool update_desired)
{
    double max_value = 0;

    role_degree_map.clear();
    ticks.clear();
    labels.clear();
    current_role_data.clear();

    if(update_desired)
        desired_role_data.clear();

    QSqlQuery qry_role(MainWindow::data_base);
    qry_role.prepare(QString
    (
    "select roles.role_name as any_role, degree from "
            "( "
                "select role_name as role, degree_of_affiliation as degree from hero_roles where hero_name in "
                "( "
                    "select hero_name from team_heroes where team_name = '" + current_team_name + "' "
                ") "
            ") "
           "as rroles right join roles on role = role_name order by any_role; "
    ));

    qry_role.exec();
    while(qry_role.next())
    {
        QString role = qry_role.value("any_role").toString();

        if(!role_degree_map.keys().contains(role))
            role_degree_map.insert(role, qry_role.value("degree").toInt());
        else
            role_degree_map[role] += qry_role.value("degree").toInt();
    }

    for (auto i = role_degree_map.begin(); i != role_degree_map.end(); ++i)
    {
        max_value = max_value < i.value() ? i.value() : max_value;

        labels << i.key();
        ticks << labels.count() - 0.5;
        current_role_data << i.value();

        if(update_desired)
            desired_role_data << i.value();
    }

    for(int i = 0; i < role_priority_indexes.count(); ++i)
    {
        int labels_index = role_priority_indexes[i];
        if(labels_index != -1)
        {
            labels[labels_index] = QString::number(i + 1) + ' ' + labels[labels_index];
        }
    }

    current_team_bar->setData(ticks, current_role_data);

    if(update_desired)
        desired_team_bar->setData(ticks, desired_role_data);

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    stats->xAxis->setTicker(textTicker);
    stats->yAxis->setRange(0, max_value + 1);

    int update = update_desired ? 3 : 1;
    replotStatsPlot(update);
}

void AlgorTabWidget::configureStatsPlot()
{
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    stats->setBackground(QBrush(gradient));

    current_team_bar->setBrush(QColor(0, 0, 255, 255));
    current_team_bar->setPen(QColor(0, 0, 255));
    current_team_bar->setWidth(0.3);
    current_team_bar->setBarsGroup(bar_group);
    current_team_bar->setName("Current team roles");

    desired_team_bar->setBrush(QColor(180, 00, 120, 255));
    desired_team_bar->setPen(QColor(180, 00, 120));
    desired_team_bar->setWidth(0.3);
    desired_team_bar->setBarsGroup(bar_group);
    desired_team_bar->setName("Desired team roles");

    stats->xAxis->setTickLabelRotation(30);
    stats->xAxis->setSubTicks(false);
    stats->xAxis->setTickLength(0, 10);
    stats->xAxis->setRange(0, role_degree_map.count());
    stats->xAxis->setBasePen(QPen(Qt::white));
    stats->xAxis->setTickPen(QPen(Qt::white));
    stats->xAxis->grid()->setVisible(true);
    stats->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    stats->xAxis->setTickLabelColor(Qt::white);
    stats->xAxis->setLabelColor(Qt::white);

    stats->yAxis->setPadding(3); // a bit more space to the left border
    stats->yAxis->setLabel("Degree Of Affiliation");
    stats->yAxis->setBasePen(QPen(Qt::white));
    stats->yAxis->setTickPen(QPen(Qt::white));
    stats->yAxis->setSubTickPen(QPen(Qt::white));
    stats->yAxis->grid()->setSubGridVisible(true);
    stats->yAxis->setTickLabelColor(Qt::white);
    stats->yAxis->setLabelColor(Qt::white);
    stats->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    stats->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));


    stats->legend->setVisible(true);
    stats->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    stats->legend->setBrush(QColor(255, 255, 255, 100));
    stats->legend->setBorderPen(Qt::NoPen);
    stats->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignTop);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    stats->legend->setFont(legendFont);
    stats->xAxis->setTickLabelFont(legendFont);
    stats->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    stats->setFixedHeight(225);

    updateStatsPlot(true);
}

QToolButton* AlgorTabWidget::createHeroToolButton(QString &hero_name, int index)
{
    QSqlQuery qry;
    qry.prepare("select picture_path from heroes where hero_name = '" + hero_name + "'");
    qry.exec(); qry.next();

    QToolButton* button = new QToolButton();

    QString icon_path = qry.value("picture_path").toString();

    button->setProperty("hero_name", hero_name);
    button->setProperty("picture_path", icon_path);
    button->setProperty("is_dummy", false);
    button->setProperty("style", "");

    QVBoxLayout* button_icon_layout = new QVBoxLayout(button);
    QLabel* name = new QLabel(hero_name, button);
    name->setAlignment(Qt::AlignCenter | Qt::AlignBottom);
    name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QIcon icon_icon(icon_path);

    if(!icon_icon.isNull())
    {
        button->setIcon(icon_icon);
        button->setIconSize({100,100});
    }

    button_icon_layout->addWidget(name);
    button_icon_layout->setAlignment(Qt::AlignHCenter);

    button->setMaximumHeight(140);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(button, &QToolButton::clicked, [this, index]()
    { changeCurrentHero(index); });

    return button;
}

QToolButton* AlgorTabWidget::createDummyToolButton(int index)
{
    QToolButton* button = new QToolButton();

    button->setText("Dummy");

    button->setStyleSheet("background-color: gray");

    button->setProperty("is_dummy", true);
    button->setProperty("style", "background-color: gray");

    button->setMaximumHeight(140);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(button, &QToolButton::clicked, [this, index]()
    { changeCurrentHero(index); });

    return button;
}

void AlgorTabWidget::updateTable(QString team_name)
{
    if(team_name == "")
        return;
    current_team_name = team_name;

    configureHeroesList();
    updateStatsPlot();
}

void AlgorTabWidget::algorCurrentHero()
{
    for(int i = 0; i < role_priority_indexes.count(); ++i)
    {
        if(role_priority_indexes[i] == -1)
            continue;

        if(algorHeroByRole(role_priority_indexes[i]))
            return;
    }

    for(int i = 0; i < current_role_data.count(); ++i)
    {
        if(algorHeroByRole(i))
            return;
    }

    MainWindow::ThrowError("Sorry, but I couldn't pick you a hero. \nPlease, change preferences or pick him by youself. (⊙_⊙;)");
}

bool AlgorTabWidget::algorHeroByRole(int role_index)
{
    // If this role is already satisfied we cannot pick a hero using this one.
    if(current_role_data[role_index] >= desired_role_data[role_index])
        return false;

    // Double check the result
    int role_difference = desired_role_data[role_index] - current_role_data[role_index];
    if(role_difference == 0)
        return false;

    auto label_list = labels[role_index].split(' ');
    QString label_string = label_list.count() > 1 ? label_list[1] : label_list[0];

    QSqlQuery qry;
    QString qry_text = "select distinct hero_roles.hero_name, role_name, degree_of_affiliation from hero_roles, team_heroes "
                       "where hero_roles.hero_name = team_heroes.hero_name and hero_roles.hero_name not in "
                       "(select hero_name from team_heroes where team_name = '"+ current_team_name + "') "
                       "and hero_roles.hero_name in "
                       "(select hero_name from hero_roles where role_name = '" + label_string + "' "
                       "and degree_of_affiliation = ";

    // Look from the best to the worst heroes judging their degree of affiliation of current role
    for(int i = role_difference; i > 0; --i)
    {
        qry.exec(qry_text + QString::number(i) + ") order by hero_roles.hero_name");
        if(qry.size() > 0)
            break;
    }

    // No heroes were found
    if(qry.size() < 1)
        return false;

    QString hero_name = "";
    bool is_good_for_team = true;
    QMap<QString, int> average_hero_deviation;

    while(qry.next())
    {
        if(hero_name != qry.value("hero_name").toString())
        {
            if(is_good_for_team && hero_name != "")
            {
                qry.exec("insert into team_heroes values ('" + hero_name + "', '" + current_team_name + "')");
                return true;
            }
            else
            {
                hero_name = qry.value("hero_name").toString();
                is_good_for_team = true;
            }
        }
        else if (!is_good_for_team)
            continue;

        int current_role_degree = qry.value("degree_of_affiliation").toInt();
        QString current_role_name = qry.value("role_name").toString();

        int current_role_index = 0;

        for(; current_role_index < labels.count(); ++current_role_index)
        {
            auto label_list = labels[current_role_index].split(' ');
            QString test_string = label_list.count() > 1 ? label_list[1] : label_list[0];
            if(test_string == current_role_name)
                break;
        }

        int team_role_degree = current_role_data[current_role_index];
        int desired_role_degree = desired_role_data[current_role_index];


        // A hero is not suitable for the team.
        if(team_role_degree + current_role_degree > desired_role_degree)
            is_good_for_team = false;

        if(std::find(role_priority_indexes.cbegin(), role_priority_indexes.cend(), current_role_index) != role_priority_indexes.cend())
        {
            average_hero_deviation[hero_name] = 0;
            continue;
        }

        if(average_hero_deviation.contains(hero_name))
            average_hero_deviation[hero_name] += std::pow((team_role_degree + current_role_degree - desired_role_degree), 2);
        else
            average_hero_deviation[hero_name] = std::pow((team_role_degree + current_role_degree - desired_role_degree), 2);
    }

    if(is_good_for_team)
    {
        qry.exec("insert into team_heroes values ('" + hero_name + "', '" + current_team_name + "')");
        return true;
    }
    else
    {
        QString best_from_worst_hero = "";
        int best_from_worst_deviation = INT_MAX;

        for(auto i = average_hero_deviation.cbegin(); i != average_hero_deviation.cend(); ++i)
            if(best_from_worst_deviation > i.value() && i.value() != 0)
            {
                best_from_worst_deviation = i.value();
                best_from_worst_hero = i.key();
            }

        if(best_from_worst_hero != "")
        {
            qry.exec("insert into team_heroes values ('" + hero_name + "', '" + current_team_name + "')");
            return true;
        }
        else
            return false;
    }
}

void AlgorTabWidget::CLR()
{
    QSqlQuery qry;
    qry.prepare("delete from team_heroes where team_name = '" + current_team_name + "'");
    qry.exec();

    Update();
}

void AlgorTabWidget::MAGIC()
{
    for(int i = 0; i < 5; ++i)
    {
        current_hero_index = i;
        clrHero();
    }

    for(int i = 0; i < 5; ++i)
    {
        current_hero_index = i;
        algorCurrentHero();
        updateStatsPlot();
    }

    configureHeroesList();
}

void AlgorTabWidget::openRolesForm()
{
    if(!roles_button->isChecked())
    {
        roles_button->setChecked(true);
        return;
    }

    AlgorSliderForm* slider_form = new AlgorSliderForm(this);
    connect(slider_form, &QObject::destroyed, roles_button, &QToolButton::toggle);
    slider_form->show();
    slider_form->setGeometry(slider_form->x(), slider_form->y(), 350, 650);
}

void AlgorTabWidget::PRINT()
{
    QTextEdit* text = new QTextEdit();
    text->document()->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, new QCPDocumentObject());
    QTextCursor cur = text->textCursor();
    cur.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(stats, 1200, 400));

    QString fileName = QFileDialog::getSaveFileName(nullptr, "Export PDF", QString(), "*.pdf");
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::NativeFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Orientation::Landscape);
    printer.setOutputFileName(fileName);
    text->document()->setPageSize(printer.pageRect().size());
    text->print(&printer);
}

void AlgorTabWidget::changeMode(int mode)
{
    if(mode == MainWindow::normal)
        choose_hero_button->setChecked(false);
}

void AlgorTabWidget::pickHero()
{
    if(choose_hero_button->isChecked())
    {
        main_window->ChangeMode(MainWindow::select_hero);
        main_window->ChangeTab(MainWindow::icons);
    }
    else
    {
        main_window->ChangeMode(MainWindow::normal);
    }
}

void AlgorTabWidget::randHero()
{
    clrHero();
    algorCurrentHero();
    configureHeroesList();
    updateStatsPlot();
}

void AlgorTabWidget::clrHero()
{
    if(current_hero_index == -1)
        return;

    QString hero_name = team_heroes[current_hero_index]->property("hero_name").toString();
    QSqlQuery("delete from team_heroes where team_name = '" + current_team_name + "' and hero_name = '" + hero_name + "'");

    delete team_heroes[current_hero_index];
    team_heroes[current_hero_index] = createDummyToolButton(current_hero_index);
    changeCurrentHero(current_hero_index);
    updateHeroesList();
    updateStatsPlot();
}

void AlgorTabWidget::infoHero()
{
    if(current_hero_index == -1)
        return;

    QString hero_name = team_heroes[current_hero_index]->property("hero_name").toString();
    auto hero_icon = HeroIcon::CreateMe(hero_name);
    hero_icon->show();
}
