#ifndef ALGORTABWIDGET_H
#define ALGORTABWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QSqlDatabase>
#include <QtSql>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QListWidget>

#include <QFileDialog>
#include <QPrinter>

#include "qcustomplot.h"
#include "qcpdocumentobject.h"
#include <random>

class AlgorTabWidget : public QWidget
{
public:
    Q_OBJECT

private:
    QString name;
    QString current_team;

    QVBoxLayout* layout;
        QHBoxLayout* team_label_layout;
            QLabel* team_label;
            QComboBox* team_name;
        QHBoxLayout* heroes_layout;
            QGridLayout* heroes_icons_layout;
            QVBoxLayout* heroes_buttons_layout;
                QPushButton* rand_button;
                QPushButton* clear_button;
                QPushButton* save_button;

        QListWidget* heroes_list;
        QCustomPlot* stats;
        QPushButton* print_button;


public:
    explicit AlgorTabWidget(QString&& name = "", QWidget *parent = nullptr);
    void Update() {updateTable(current_team);}
    QString GetName() { return name; }

private:
    void configureHeroList(QString& hero_name, QString& picture_path);

private slots:
    void updateTable(QString team_name);
    void MAGIC();
    void CLR();
    void SAVE();
    void PRINT();
};

#endif // ALGORTABWIDGET_H
