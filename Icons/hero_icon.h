#ifndef HEROICON_H
#define HEROICON_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QDebug>
#include <QListWidget>

class HeroIcon : public QWidget
{
    Q_OBJECT

    QVBoxLayout* main_layout;
    QHBoxLayout* pic_data_layout;
    QGridLayout* data_layout;
    QListWidget* skills_list;

public:
    explicit HeroIcon(QString hero_name, QWidget *parent = nullptr);
    static HeroIcon* CreateMe(QString hero_name);

private:
    void fillSkillsList(QString hero_name);
};

#endif // HEROICON_H
