#include "hero_icon.h"

HeroIcon::HeroIcon(QWidget *parent) : QWidget(parent)
{
    setFixedSize({150, 150});
    setStyleSheet(QString("background-color: %1").arg(QColor(Qt::red).name()));
}

HeroIcon* HeroIcon::CreateMe()
{
    return new HeroIcon();
}
