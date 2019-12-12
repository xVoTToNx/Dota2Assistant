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
#include "algor_slider_form.h"
#include <random>

class AlgorTabWidget : public QWidget
{
public:
    Q_OBJECT

private:
    QString name;
    QString current_team;
    int current_hero;
    QVector<QToolButton*> team_heroes;

    QVBoxLayout* layout;
        QHBoxLayout* team_label_layout;
            QToolButton* choose_hero_button;
            QPushButton* algor_hero_button;
            QPushButton* clear_hero_button;
            QPushButton* info_hero_button;
            QLabel* team_label;
            QComboBox* team_name;
        QHBoxLayout* heroes_layout;
            QGridLayout* heroes_icons_layout;
            QVBoxLayout* heroes_buttons_layout;
                QPushButton* rand_button;
                QPushButton* clear_button;
                QToolButton* roles_button;

        QListWidget* hero_roles_list;
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
    ~AlgorTabWidget();
    void Update() {updateTable(current_team);}
    QString GetName() { return name; }

    // If current_role_data and/or desired_role_data has been changed and we need to show it on the plot
    // 1 - update current_role_data bar values
    // 2 - desired_role_data bar values
    // 3 - both
    void replotStatsPlot(int is_data_changed);

    void setDesiredRoleData(int index, int value) { desired_role_data[index] = value; }
    int getDesiredRoleData(int index) { return desired_role_data[index]; }
    int sizeDesiredToleData() { return desired_role_data.size(); }

private:
    // If heroes of current team have been changed and we need to show it on icon list at the top
    // Calls updateHeroRolesList
    void updateHeroesList();

    void configureHeroesList();
    void changeCurrentHero(int new_hero_index);

    // Used in updateHeroesList
    void updateHeroRolesList(QString& hero_name, QString& picture_path);

    // If we need to recalculate current_role_data and/or desired_role_data
    void updateStatsPlot(bool update_desired = false);

    // Init the plot. Calls updateStatsPlot(true)
    void configureStatsPlot();

private slots:
    // Global tab update
    // Calls  updateHeroesList() and updateStatsPlot()
    void updateTable(QString team_name);
    void MAGIC();
    void CLR();
    void openRolesForm();
    void PRINT();
};

#endif // ALGORTABWIDGET_H
