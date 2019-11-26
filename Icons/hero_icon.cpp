#include "hero_icon.h"
#include "main_window.h"


HeroIcon::HeroIcon(QString hero_name, QWidget *parent)
    : QWidget(parent)
    , main_layout (new QVBoxLayout(this))
    , pic_data_layout (new QHBoxLayout())
    , data_layout (new QGridLayout())
    , skills_list (new QListWidget())
{
    //setFixedSize({700, 800});

    QSqlQuery qry(MainWindow::data_base);
    qry.prepare(QString("select * from heroes where hero_name = '" + hero_name + "'"));
    qry.exec(); qry.next();

    QLabel* pic = new QLabel(this);
    pic->setFixedSize(300,300);
    QPixmap pixmap(qry.value("picture_path").toString());
    if(!pixmap.isNull())
        pic->setPixmap(pixmap.scaled(300, 300, Qt::KeepAspectRatio));

    pic_data_layout->addWidget(pic);


    QLabel* hero_name_label = MainWindow::CreateLabel(hero_name);
    data_layout->addWidget(hero_name_label, 0, 0, 1, 2);
    QLabel* main_attribute_label = MainWindow::CreateLabel("Main Attribute: " + qry.value("main_attribute").toString());
    data_layout->addWidget(main_attribute_label, 1, 0, 1, 2);
    QLabel* strength_label = MainWindow::CreateLabel("Strength: " + qry.value("strength").toString());
    QLabel* inteligence_label = MainWindow::CreateLabel("Intelligence: " + qry.value("intelligence").toString());
    QLabel* agility_label = MainWindow::CreateLabel("Agility: " + qry.value("agility").toString());
    data_layout->addWidget(strength_label, 2, 0);
    data_layout->addWidget(inteligence_label, 3, 0);
    data_layout->addWidget(agility_label, 4, 0);
    QLabel* health_label = MainWindow::CreateLabel("Health: " + qry.value("health").toString());
    QLabel* mana_label = MainWindow::CreateLabel("Mana: " + qry.value("mana").toString());
    QLabel* armor_label = MainWindow::CreateLabel("Armor: " + qry.value("armor").toString());
    data_layout->addWidget(health_label, 2, 1);
    data_layout->addWidget(mana_label, 3, 1);
    data_layout->addWidget(armor_label, 4, 1);
    QLabel* attack_speed_label = MainWindow::CreateLabel("Attack Speed: " + qry.value("attack_speed").toString());
    QLabel* move_speed_label = MainWindow::CreateLabel("Move Speed: " + qry.value("move_speed").toString());
    data_layout->addWidget(attack_speed_label, 5, 0);
    data_layout->addWidget(move_speed_label, 5, 1);

    data_layout->setAlignment(Qt::AlignCenter);
    pic_data_layout->addLayout(data_layout);

    main_layout->addLayout(pic_data_layout);
    main_layout->addWidget(skills_list);
    fillSkillsList(hero_name);
}


HeroIcon* HeroIcon::CreateMe(QString hero_name)
{
    return new HeroIcon(hero_name);
}

void HeroIcon::fillSkillsList(QString hero_name)
{
    QSqlQuery qry(MainWindow::data_base);
    qry.prepare(QString("select * from skills where hero_name = '" + hero_name + "'"));
    qry.exec();
    while(qry.next())
    {
        QWidget* skill = new QWidget();
        QGridLayout* skill_layout = new QGridLayout(skill);

        QLabel* pic = new QLabel(this);
        pic->setFixedSize(100,100);
        QPixmap pixmap(qry.value("picture_path").toString());
        if(!pixmap.isNull())
            pic->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));

        skill_layout->addWidget(pic, 0, 0, 2, 2);

        QLabel* name_label = MainWindow::CreateLabel("Skill: " + qry.value("skill_name").toString());
        skill_layout->addWidget(name_label, 0, 2, 1, 2);

        QLabel* lvl_label = MainWindow::CreateLabel("Max Lvl: " + qry.value("max_lvl").toString());
        skill_layout->addWidget(lvl_label, 0, 4, 1, 2);

        QLabel* act_label = MainWindow::CreateLabel("Active: " + qry.value("is_active").toString());
        skill_layout->addWidget(act_label, 1, 2);
        QLabel* aura_label = MainWindow::CreateLabel("Aura: " + qry.value("is_aura").toString());
        skill_layout->addWidget(aura_label, 1, 3);
        QLabel* bff_label = MainWindow::CreateLabel("Buff: " + qry.value("is_buff").toString());
        skill_layout->addWidget(bff_label, 1, 4);
        QLabel* debff_label = MainWindow::CreateLabel("DeBuff: " + qry.value("is_debuff").toString());
        skill_layout->addWidget(debff_label, 1, 5);

        QLabel* desc_label = MainWindow::CreateLabel("Description: " + qry.value("description").toString());
        desc_label->setWordWrap(true);
        skill_layout->addWidget(desc_label, 2, 2, 1, 4);

        QListWidgetItem* item = new QListWidgetItem();
        item->setSizeHint({400, 120});
        //skill->setFixedSize({skills_list->width(), 100});
        skills_list->addItem(item);
        skills_list->setItemWidget(item, skill);
    }
}

