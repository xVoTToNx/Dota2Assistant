#ifndef ITEMICON_H
#define ITEMICON_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSqlQuery>
#include <QDebug>
#include <QListWidget>


class ItemIcon : public QWidget
{
    Q_OBJECT

    QVBoxLayout* main_layout;
    QHBoxLayout* pic_data_layout;
    QGridLayout* data_layout;
    QListWidget* recomendations_list;

public:
    explicit ItemIcon(QString item_name, QWidget *parent = nullptr);
    static ItemIcon* CreateMe(QString item_name);

private:
    void fillRecomendationsList(QString hero_name);
};

#endif // ITEMICON_H
