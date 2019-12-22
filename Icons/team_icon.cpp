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
    QPushButton* push = new QPushButton("V Print");
    data_layout->addWidget(push, 4, 0);
    connect(push, &QPushButton::clicked, [this]()
    {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOrientation(QPrinter::Orientation::Portrait);
        QString fileName = QFileDialog::getSaveFileName(nullptr, "Export PDF", QString(), "*.pdf");
        if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }
        printer.setOutputFileName(fileName);

        QPainter painter;

        if (! painter.begin(&printer)) { // failed to open file
            qWarning("failed to open file, is it writable?");
        }

        if(this->width() > 0 && this->height() >0 ){

            qreal xscale = 0.9*printer.pageRect().width()/qreal(this->width());
            qreal yscale = 0.9*printer.pageRect().height()/qreal(this->height());

            qreal scale = qMin(xscale, yscale);

            painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                              printer.paperRect().y() + printer.pageRect().height()/2);

            painter.scale(scale, scale);
            painter.translate(-width()/2, -height()/2);
            this->render(&painter);
            painter.resetTransform();
        }
        painter.end();
    });
    QPushButton* pushV = new QPushButton("H Print");
    data_layout->addWidget(pushV, 4, 1);
    connect(pushV, &QPushButton::clicked, [this]()
    {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOrientation(QPrinter::Orientation::Landscape);
        QString fileName = QFileDialog::getSaveFileName(nullptr, "Export PDF", QString(), "*.pdf");
        if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }
        printer.setOutputFileName(fileName);

        QPainter painter;

        if (! painter.begin(&printer)) { // failed to open file
            qWarning("failed to open file, is it writable?");
        }

        if(this->width() > 0 && this->height() >0 ){

            qreal xscale = 0.9*printer.pageRect().width()/qreal(this->width());
            qreal yscale = 0.9*printer.pageRect().height()/qreal(this->height());

            qreal scale = qMin(xscale, yscale);

            painter.translate(printer.paperRect().x() + printer.pageRect().width()/2,
                              printer.paperRect().y() + printer.pageRect().height()/2);

            painter.scale(scale, scale);
            painter.translate(-width()/2, -height()/2);
            this->render(&painter);
            painter.resetTransform();
        }
        painter.end();
    });


    data_group->setLayout(data_layout);


    data_layout->setAlignment(Qt::AlignCenter);
    pic_data_layout->addWidget(data_group);

    main_layout->addLayout(pic_data_layout);
    main_layout->addLayout(heroes_layout);

    fillHeroesList(team_name);

    setAttribute(Qt::WA_DeleteOnClose);
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
