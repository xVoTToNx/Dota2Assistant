#ifndef ICONSTABWIDGET_H
#define ICONSTABWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTabWidget>
#include <QSqlTableModel>


#include "filter_proxy_model.h"


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

    QString GetName() { return name; }

private:

private slots:
    void updateTable(Icon icon);
};


#endif // ICONSTABWIDGET_H
