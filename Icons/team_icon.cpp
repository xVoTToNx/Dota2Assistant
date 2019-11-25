#include "team_icon.h"

TeamIcon::TeamIcon(QWidget *parent) : QWidget(parent)
{
    setFixedSize({150, 150});
    setStyleSheet(QString("background-color: %1").arg(QColor(Qt::green).name()));
}

TeamIcon* TeamIcon::CreateMe()
{
    return new TeamIcon();
}
