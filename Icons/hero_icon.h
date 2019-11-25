#ifndef HEROICON_H
#define HEROICON_H

#include <QWidget>

class HeroIcon : public QWidget
{
    Q_OBJECT
public:
    explicit HeroIcon(QWidget *parent = nullptr);
    static HeroIcon* CreateMe();

signals:

public slots:
};

#endif // HEROICON_H
