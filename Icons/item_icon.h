#ifndef ITEMICON_H
#define ITEMICON_H

#include <QWidget>

class ItemIcon : public QWidget
{
    Q_OBJECT
public:
    explicit ItemIcon(QWidget *parent = nullptr);
    static ItemIcon* CreateMe();

signals:

public slots:
};

#endif // ITEMICON_H
