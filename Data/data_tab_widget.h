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
#include <QRadioButton>
#include <QTextEdit>

#include <QFileDialog>
#include <QPrinter>

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
    bool is_custom_query;

    QTableView* table_view;
    FilterProxyModel* filter_model;
    SearchProxyModel* search_model;
    QSqlTableModel* model;
    QSqlQueryModel* query_model;

    QHBoxLayout* layout;

        QVBoxLayout* table_layout;
            QHBoxLayout* table_button_layout;
                QLabel* table_label;
                QComboBox* table_combobox;
                QPushButton* insert_button;
                QPushButton* remove_button;
                QPushButton* print_button;
            QGridLayout* query_layout;
                QTextEdit* query_edit;
                QPushButton* exec_button;
                QPushButton* clr_button;


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
    void printTable();
    void changeFilterSearchTabs();
    void addHeaderData(QString& source, QRegExp reg_exp, void (*data_handler)(QStringList& list, int i, QSqlTableModel* model));

private slots:
    void changeTable(const QString& table_name);
};

#endif // DATATABWIDGET_H
