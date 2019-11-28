#ifndef ICONSTABWIDGET_H
#define ICONSTABWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTabWidget>
#include <QSqlTableModel>
#include <QPicture>
#include <QToolButton>



#include "filter_proxy_model.h"
#include "Algor/algor_tab_widget.h"
#include "hero_icon.h"
#include "item_icon.h"
#include "team_icon.h"


class IconsTabWidget : public QWidget
{
public:
    Q_OBJECT

private:
    enum Icon
    {
        hero,
        item,
        team
    };

    Icon current_table;

    QString name;
    QSqlTableModel* model;
    FilterProxyModel* filter_model;

    QHBoxLayout* main_layout;
    QVBoxLayout* button_layout;
    QVBoxLayout* table_layout;
    QHBoxLayout* another_another_layout;
    QGridLayout* icons_layout;
    QPushButton* clear_button;
    QLineEdit* search_line_edit;


public:
    explicit IconsTabWidget(QString&& name = "", QWidget *parent = nullptr);

    void Update() {updateTable(current_table);}
    QString GetName() { return name; }

private slots:
    void updateTable(Icon icon);
};


#endif // ICONSTABWIDGET_H
