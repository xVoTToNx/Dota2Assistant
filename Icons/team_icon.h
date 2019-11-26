#ifndef TEAMICON_H
#define TEAMICON_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QDebug>
#include <QListWidget>

class TeamIcon : public QWidget
{
    Q_OBJECT

    QVBoxLayout* main_layout;
    QHBoxLayout* pic_data_layout;
    QGridLayout* data_layout;
    QGridLayout* heroes_layout;

public:
    explicit TeamIcon(QString team_name, QWidget *parent = nullptr);
    static TeamIcon* CreateMe(QString team_name);

private:
    void fillHeroesList(QString team_name);
};

#endif // TEAMICON_H
