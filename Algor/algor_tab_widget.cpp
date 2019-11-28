#include "algor_tab_widget.h"
#include "main_window.h"

AlgorTabWidget::AlgorTabWidget(QString &&name, QWidget *parent)
    : QWidget(parent)
    , name (name)
    , current_team("")
    , layout (new QVBoxLayout())
    , team_label_layout (new QHBoxLayout)
    , team_label (new QLabel("Team: ", this))
    , team_name (new QComboBox(this))
    , heroes_layout (new QHBoxLayout())
    , heroes_icons_layout (new QGridLayout())
    , heroes_buttons_layout (new QVBoxLayout())
    , rand_button (new QPushButton("RAND", this))
    , clear_button (new QPushButton("CLR", this))
    , save_button (new QPushButton("SAVE", this))
    , heroes_list (new QListWidget(this))
    , stats (new QCustomPlot())
    , print_button (new QPushButton("PRINT", this))
{
    team_label_layout->addWidget(team_label);
    team_label_layout->addWidget(team_name);
    QSqlQuery qry;
    qry.prepare("select team_name from teams");
    qry.exec();
    while(qry.next())
        team_name->addItem(qry.value(0).toString());

    layout->addLayout(team_label_layout);

    heroes_buttons_layout->addWidget(rand_button);
    heroes_buttons_layout->addWidget(clear_button);
    heroes_buttons_layout->addWidget(save_button);
    heroes_buttons_layout->addWidget(print_button);
    heroes_layout->addLayout(heroes_icons_layout);
    heroes_layout->addLayout(heroes_buttons_layout);
    layout->addLayout(heroes_layout);

    layout->addWidget(heroes_list);

    layout->addWidget(stats);

    connect(rand_button, &QPushButton::clicked, this, &AlgorTabWidget::MAGIC);
    connect(clear_button, &QPushButton::clicked, this, &AlgorTabWidget::CLR);
    connect(save_button, &QPushButton::clicked, this, &AlgorTabWidget::SAVE);
    connect(print_button, &QPushButton::clicked, this, &AlgorTabWidget::PRINT);
    connect(team_name, &QComboBox::currentTextChanged, this, &AlgorTabWidget::updateTable);
    updateTable(team_name->itemText(0));

    setLayout(layout);
}

void AlgorTabWidget::configureHeroList(QString &hero_name, QString& picture_path)
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
    {
        pic->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));
    }
    else
    {
        pic->setText(hero_name);
    }

    list_item_layout->addWidget(pic, 0, 0, qry.size() / 4 + 1, 1);

    int i = 0;
    while(qry.next())
    {
        QString role_name = qry.value("role_name").toString();
        role_name[0] = role_name[0].toUpper();

        QLabel* role_label = new QLabel(role_name + ": " + qry.value("degree_of_affiliation").toString());
        list_item_layout->addWidget(role_label, i / 4, i % 4 + 1);

        ++i;
    }

    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint({400, 120});
    heroes_list->addItem(item);
    heroes_list->setItemWidget(item, list_widget);
}

void AlgorTabWidget::updateTable(QString team_name)
{
    if(team_name == "")
        return;
    current_team = team_name;
    heroes_list->clear();

    QLayoutItem* item;
    while ((item = heroes_icons_layout->takeAt(0)) != nullptr)
    {delete item->widget();delete item;}

    QSqlQuery qry;
    qry.prepare("select picture_path, heroes.hero_name from team_heroes, heroes where team_name = '" + team_name + "' and heroes.hero_name = team_heroes.hero_name");
    qry.exec();
    int i = 0;
    while(i < 5)
    {
        if(qry.next())
        {
            QString hero_name = qry.value("hero_name").toString();
            QString icon_path = qry.value("picture_path").toString();
            QToolButton* button = new QToolButton();
            button->setMaximumHeight(140);
            button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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

            connect(button, &QPushButton::clicked, [hero_name](){
                auto icon = HeroIcon::CreateMe(hero_name);
                icon->setAttribute(Qt::WA_DeleteOnClose);
                icon->show(); });

            heroes_icons_layout->addWidget(button, 0, i);

            configureHeroList(hero_name, icon_path);
        }
        else
        {
            QToolButton* empty_hero = new QToolButton(this);
            empty_hero->setText("Dummy");
            heroes_icons_layout->addWidget(empty_hero, 0, i);
        }
        ++i;
    }



    QSqlQuery stats_qry(MainWindow::data_base);
    stats_qry.prepare(QString("select hero_name from team_heroes where team_name = '" + team_name + "'"));
    stats_qry.exec();
    QMap<QString, double> role_degree_map;
    QVector<QString> labels;
    QVector<double> ticks;
    QVector<double> role_data;
    double max_value = 0;
    while(stats_qry.next())
    {
        QSqlQuery qry_role(MainWindow::data_base);
        qry_role.prepare(QString("select role_name, degree_of_affiliation from hero_roles where hero_name = '" + stats_qry.value(0).toString() + "'"));
        qry_role.exec();
        while(qry_role.next())
        {
            QString role = qry_role.value("role_name").toString();
            if(!role_degree_map.keys().contains(role))
                role_degree_map.insert(role, qry_role.value("degree_of_affiliation").toInt());
            else
            {
                role_degree_map[role] += qry_role.value("degree_of_affiliation").toInt();
            }
        }
    }

    for (auto i = role_degree_map.begin(); i != role_degree_map.end(); ++i)
    {
        max_value = max_value < i.value() ? i.value() : max_value;

        labels << i.key();
        ticks << labels.count() - 0.5;
        role_data << i.value();

    }
    stats->clearItems();
    stats->clearGraphs();
    stats->clearPlottables();
    stats->clearMask();
    stats->clearFocus();

    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    stats->setBackground(QBrush(gradient));

    // create empty bar chart objects:
    QCPBars *roles = new QCPBars(stats->xAxis, stats->yAxis);
    roles->setAntialiased(true); // gives more crisp, pixel aligned bar borders
    //roles->setStackingGap(1);
    // set names and colors:
    roles->setPen(QPen(QColor(0, 168, 140).lighter(130)));
    roles->setBrush(QColor(0, 168, 140));

    // prepare x axis with country labels:
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    stats->xAxis->setTicker(textTicker);
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

    // prepare y axis:
    stats->yAxis->setRange(0, max_value + 1);
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

    roles->setData(ticks, role_data);

    stats->replot();
    stats->setFixedHeight(225);
}

void AlgorTabWidget::CLR()
{
    QSqlQuery qry;
    qry.prepare("delete from team_heroes where team_name = '" + current_team + "'");
    qry.exec();

    Update();
}

void AlgorTabWidget::MAGIC()
{
    std::mt19937 gen(554534435345345);
    std::uniform_int_distribution<> uid(0, 10);

    int heroes = 0;

    QSqlQuery qry;
    qry.prepare("delete from team_heroes where team_name = '" + current_team + "'");
    qry.exec();

    // TANK
    qry.prepare("select hero_name from hero_roles "
                "where (role_name = 'tank' and degree_of_affiliation between 3 and 5) "
                "or (role_name = 'initiator' and degree_of_affiliation between 3 and 5) ORDER BY RAND() LIMIT 1");
    qry.exec();
    if(qry.size() > 0)
    {
        qry.next();
        qry.prepare("insert into team_heroes values('" + qry.value(0).toString() +"', '" + current_team + "')" );
        if(qry.exec())
            ++heroes;
    }


    // MID
    qry.prepare("select hero_name from hero_roles where (role_name = 'midder'"
                " and degree_of_affiliation between 3 and 5)ORDER BY RAND() LIMIT 1");
    qry.exec();
    if(qry.size() > 0)
    {
        qry.next();
        qry.prepare("insert into team_heroes values('" + qry.value(0).toString() +"', '" + current_team + "')" );
        if(qry.exec())
            ++heroes;
    }

    // CORE
    qry.prepare("select hero_name from hero_roles"
                " where (role_name = 'core' and degree_of_affiliation between 3 and 5)"
                " or (role_name = 'stealth' and degree_of_affiliation between 3 and 5) ORDER BY RAND() LIMIT 1");
    qry.exec();
    if(qry.size() > 0)
    {
        qry.next();
        qry.prepare("insert into team_heroes values('" + qry.value(0).toString() +"', '" + current_team + "')" );
        if(qry.exec())
            ++heroes;
    }

    // CORE 2
    qry.prepare("select hero_name from hero_roles "
                "where (role_name = 'killer' and degree_of_affiliation between 2 and 5) or "
                "(role_name = 'core' and degree_of_affiliation between 3 and 5) "
                "or (role_name = 'nuker' and degree_of_affiliation between 2 and 5) ORDER BY RAND() LIMIT 1");
    qry.exec();
    if(qry.size() > 0)
    {
        qry.next();
        qry.prepare("insert into team_heroes values('" + qry.value(0).toString() +"', '" + current_team + "')" );
        if(qry.exec())
            ++heroes;
    }


    // SUPP
    qry.prepare("select hero_name from hero_roles "
                "where (role_name = 'support' and degree_of_affiliation between 3 and 5) or "
                "(role_name = 'babysitter' and degree_of_affiliation between 3 and 5) ORDER BY RAND() LIMIT 1");
    qry.exec();
    if(qry.size() > 0)
    {
        qry.next();
        qry.prepare("insert into team_heroes values('" + qry.value(0).toString() +"', '" + current_team + "')" );
        if(qry.exec())
            ++heroes;
    }

    QSqlQuery heroes_qry;
    heroes_qry.prepare("select hero_name from heroes");
    heroes_qry.exec();
    int j = 0;
    for(int i = heroes; i < 5; ++j)
    {
        if(!heroes_qry.next())
            break;

        qry.prepare("insert into team_heroes values('" + heroes_qry.value(0).toString() +"', '" + current_team + "')" );
        if(qry.exec())
        {
            ++i;
        }

        if(j > qry.exec() * 3)
            break;
    }

    Update();
}

void AlgorTabWidget::SAVE()
{

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
