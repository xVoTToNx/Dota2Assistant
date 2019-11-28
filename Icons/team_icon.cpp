#include "team_icon.h"
#include "main_window.h"


TeamIcon::TeamIcon(QString team_name, QWidget *parent)
    : QWidget(parent)
    , main_layout (new QVBoxLayout(this))
    , pic_data_layout (new QHBoxLayout())
    , data_layout (new QGridLayout())
    , heroes_layout (new QGridLayout())
{
    QSqlQuery qry(MainWindow::data_base);
    qry.prepare(QString("select * from teams where team_name = '" + team_name + "'"));
    qry.exec(); qry.next();

    QLabel* pic = new QLabel(this);
    pic->setFixedSize(300,300);
    QPixmap pixmap(qry.value("picture_path").toString());
    if(!pixmap.isNull())
        pic->setPixmap(pixmap.scaled(300, 300, Qt::KeepAspectRatio));

    pic_data_layout->addWidget(pic);

    QGroupBox* data_group = new QGroupBox(this);

    QLabel* hero_name_label = MainWindow::CreateLabel(team_name);
    data_layout->addWidget(hero_name_label, 0, 0, 1, 2);
    QLabel* date_label = MainWindow::CreateLabel("Creation Date: " + qry.value("creation_date").toString());
    data_layout->addWidget(date_label, 1, 0, 1, 2);
    QLabel* focus_label = MainWindow::CreateLabel("Focus: " + qry.value("focus").toString());
    data_layout->addWidget(focus_label, 2, 0);
    QLabel* mark_label = MainWindow::CreateLabel("Mark: " + qry.value("mark").toString());
    data_layout->addWidget(mark_label, 2, 1);
    QLabel* desc_label = MainWindow::CreateLabel("Description: " + qry.value("description").toString());
    desc_label->setWordWrap(true);
    data_layout->addWidget(desc_label, 3, 0, 1, 2);
    QPushButton* stats_button = new QPushButton("STATS", this);
    data_layout->addWidget(stats_button, 4, 0, 1, 2);
    data_group->setLayout(data_layout);

    connect(stats_button, &QPushButton::clicked, this, std::bind(&TeamIcon::createTeamStats, this, team_name));

    data_layout->setAlignment(Qt::AlignCenter);
    pic_data_layout->addWidget(data_group);

    main_layout->addLayout(pic_data_layout);
    main_layout->addLayout(heroes_layout);

    fillHeroesList(team_name);
}

TeamIcon* TeamIcon::CreateMe(QString team_name)
{
    return new TeamIcon(team_name);
}

void TeamIcon::fillHeroesList(QString team_name)
{
    QSqlQuery qry(MainWindow::data_base);
    qry.prepare(QString("select * from team_heroes where team_name = '" + team_name + "'"));
    qry.exec();

    int columns = 5;

    int i = -1;
    while(qry.next())
    {
        ++i;

        QString data = qry.value("hero_name").toString();

        QSqlQuery qry_hero(MainWindow::data_base);
        qry_hero.prepare(QString("select picture_path from heroes where hero_name = '" + qry.value("hero_name").toString() + "'"));
        qry_hero.exec(); qry_hero.next();

        QString icon_path = qry_hero.value("picture_path").toString();
        QToolButton* button = new QToolButton();
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QVBoxLayout* button_icon_layout = new QVBoxLayout(button);
        QLabel* name = new QLabel(data, button);
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

        connect(button, &QPushButton::clicked, [data](){
            auto icon = HeroIcon::CreateMe(data);
            icon->setAttribute(Qt::WA_DeleteOnClose);
            icon->show(); });

        heroes_layout->addWidget(button, 1 + i / columns, i % columns);
    }
}

void TeamIcon::createTeamStats(QString team_name)
{
    QSqlQuery qry(MainWindow::data_base);
    qry.prepare(QString("select hero_name from team_heroes where team_name = '" + team_name + "'"));
    qry.exec();
    QMap<QString, double> role_degree_map;
    QVector<QString> labels;
    QVector<double> ticks;
    double max_value = 0;
    while(qry.next())
    {
        QSqlQuery qry_role(MainWindow::data_base);
        qry_role.prepare(QString("select role_name, degree_of_affiliation from hero_roles where hero_name = '" + qry.value(0).toString() + "'"));
        qry_role.exec();
        while(qry_role.next())
        {
            QString role = qry_role.value("role_name").toString();
            if(role_degree_map.keys().contains(role))
                role_degree_map.insert(role, qry_role.value("degree_of_affiliation").toInt());
            else
            {
                role_degree_map[role] += qry_role.value("degree_of_affiliation").toInt();
                max_value = max_value < role_degree_map[role] ? role_degree_map[role] : max_value;
            }

            if(!labels.contains(role)) { labels << role; ticks<<labels.count() - 0.5; }
        }
    }

    QCustomPlot* customPlot = new QCustomPlot(this);

    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    customPlot->setBackground(QBrush(gradient));

    // create empty bar chart objects:
    QCPBars *roles = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    roles->setAntialiased(false); // gives more crisp, pixel aligned bar borders
    roles->setStackingGap(1);
    // set names and colors:
    roles->setName("Regenerative");
    roles->setPen(QPen(QColor(0, 168, 140).lighter(130)));
    roles->setBrush(QColor(0, 168, 140));

    // prepare x axis with country labels:
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    customPlot->xAxis->setTicker(textTicker);
    customPlot->xAxis->setTickLabelRotation(60);
    customPlot->xAxis->setSubTicks(false);
    customPlot->xAxis->setTickLength(0, 10);
    customPlot->xAxis->setRange(0, role_degree_map.count());
    customPlot->xAxis->setBasePen(QPen(Qt::white));
    customPlot->xAxis->setTickPen(QPen(Qt::white));
    customPlot->xAxis->grid()->setVisible(true);
    customPlot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    customPlot->xAxis->setTickLabelColor(Qt::white);
    customPlot->xAxis->setLabelColor(Qt::white);

    // prepare y axis:
    customPlot->yAxis->setRange(0, max_value);
    customPlot->yAxis->setPadding(3); // a bit more space to the left border
    customPlot->yAxis->setLabel("Degree Of Affiliation");
    customPlot->yAxis->setBasePen(QPen(Qt::white));
    customPlot->yAxis->setTickPen(QPen(Qt::white));
    customPlot->yAxis->setSubTickPen(QPen(Qt::white));
    customPlot->yAxis->grid()->setSubGridVisible(true);
    customPlot->yAxis->setTickLabelColor(Qt::white);
    customPlot->yAxis->setLabelColor(Qt::white);
    customPlot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

    QVector<double> role_data = QVector<double>::fromList(role_degree_map.values());
    roles->setData(ticks, role_data);

    // setup legend:
    customPlot->legend->setVisible(true);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    customPlot->legend->setBrush(QColor(255, 255, 255, 100));
    customPlot->legend->setBorderPen(Qt::NoPen);
    QFont legendFont("Comic Sans MS");
    legendFont.setPointSize(10);
    customPlot->legend->setFont(legendFont);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    customPlot->replot();
    customPlot->show();
    QTextEdit* text = new QTextEdit();
    text->document()->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, new QCPDocumentObject());


    QTextCursor cur = text->textCursor();
    cur.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(customPlot, 1200, 400));

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
