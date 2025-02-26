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
#include <cmath>

class MainWindow;

class AlgorTabWidget : public QWidget
{
public:
    Q_OBJECT

private:
    MainWindow* main_window;

    QString tab_name;
    QString current_team_name;
    int current_hero_index;
    QVector<int> role_priority_indexes;

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
    explicit AlgorTabWidget(QString&& tab_name = "", MainWindow* main_window = nullptr, QWidget *parent = nullptr);
    ~AlgorTabWidget();
    void Update() {updateTable(current_team_name);}
    QString GetName() { return tab_name; }
    QString GetCurrentTeamName() { return current_team_name; }

    void DeleteCurrentHero() { clrHero(); }

    // If current_role_data and/or desired_role_data has been changed and we need to show it on the plot
    // 1 - update current_role_data bar values
    // 2 - desired_role_data bar values
    // 3 - both
    // 4 - update labels
    void replotStatsPlot(int is_data_changed);

    void setDesiredRoleData(int index, int value) { desired_role_data[index] = value; }
    int getDesiredRoleData(int index) { return desired_role_data[index]; }
    int sizeDesiredToleData() { return desired_role_data.size(); }

    void setRolePriorityIndex(int priority, int index);
    int getRolePriorityIndex(int index) const {return role_priority_indexes[index]; }
    int getRolePrioritySize() const {return role_priority_indexes.count(); }

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

    QToolButton* createHeroToolButton(QString& hero_name, int index);
    QToolButton* createDummyToolButton(int index);

private slots:
    // Global tab update
    // Calls  updateHeroesList() and updateStatsPlot()
    void updateTable(QString team_name);
    void algorCurrentHero();
    bool algorHeroByRole(int role_index);
    void MAGIC();
    void CLR();
    void openRolesForm();
    void PRINT();

    void changeMode(int mode);
    void pickHero();
    void randHero();
    void clrHero();
    void infoHero();
};

#endif // ALGORTABWIDGET_H
