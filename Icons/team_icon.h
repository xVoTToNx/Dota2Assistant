#ifndef TEAMICON_H
#define TEAMICON_H

#include <QWidget>

class TeamIcon : public QWidget
{
    Q_OBJECT
public:
    explicit TeamIcon(QWidget *parent = nullptr);
    static TeamIcon* CreateMe();

signals:

public slots:
};

#endif // TEAMICON_H
