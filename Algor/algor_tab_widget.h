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
                QPushButton* roles_button;

        QListWidget* heroes_list;
        QCustomPlot* stats;
            QCPBarsGroup* bar_group;
            QCPBars* current_team_bar;
            QCPBars* desired_team_bar;

            QMap<QString, double> role_degree_map;
            QVector<QString> labels;
            QVector<double> ticks;
            QVector<double> current_role_data;
            QVector<double> desired_role_data;
        QPushButton* print_button;


public:
    explicit AlgorTabWidget(QString&& name = "", QWidget *parent = nullptr);
    void Update() {updateTable(current_team);}
    QString GetName() { return name; }

private:
    void updateHeroesList();
    void updateHeroRolesList(QString& hero_name, QString& picture_path);
    void updateStatsPlot();
    void configureStatsPlot();

private slots:
    void updateTable(QString team_name);
    void MAGIC();
    void CLR();
    void openRolesForm();
    void PRINT();
};

#endif // ALGORTABWIDGET_H
