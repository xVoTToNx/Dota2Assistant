#ifndef DATATABWIDGET_H
#define DATATABWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QSqlDatabase>
#include <QtSql>
#include <QTableView>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QDateEdit>
#include <QSlider>
#include <QGroupBox>

#include <functional>
#include "filter_proxy_model.h"
#include "search_proxy_model.h"
#include "data_item_delegate.h"

#define TY_QT_FOR_CHAR 34


class DataTabWidget : public QWidget
{
public:
    Q_OBJECT

private:
    QString name;
    QString current_table;

    QTableView* table_view;
    FilterProxyModel* filter_model;
    SearchProxyModel* search_model;
    QSqlTableModel* test_model;

    QHBoxLayout* layout;

        QVBoxLayout* table_layout;
            QHBoxLayout* table_button_layout;
                QLabel* table_label;
                QComboBox* table_combobox;
                QPushButton* insert_button;
                QPushButton* remove_button;


        QTabWidget* filter_search_tab_widget;
            QWidget* filter_tab_widget;
            QWidget* search_tab_widget;

public:
    explicit DataTabWidget(QString&& name = "", QWidget *parent = nullptr);

    void Update() {changeTable(current_table);}
    QString GetName() { return name; }

private:
    void insertRow();
    void removeRow();
    void changeFilterSearchTabs();

private slots:
    void changeTable(const QString& table_name);
};

#endif // DATATABWIDGET_H
