#include "item_icon.h"

ItemIcon::ItemIcon(QWidget *parent) : QWidget(parent)
{
    setFixedSize({150, 150});
    setStyleSheet(QString("background-color: %1").arg(QColor(Qt::blue).name()));
}

ItemIcon* ItemIcon::CreateMe()
{
    return new ItemIcon();
}
