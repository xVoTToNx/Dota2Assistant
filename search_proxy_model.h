#ifndef SEARCHPROXYMODEL_H
#define SEARCHPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QTableView>
#include <QDebug>
#include <QHeaderView>

class SearchProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

private:
    QTableView* view;
    int* start_row;
    int* current_row;
    int *current_current_row;
    bool* go_next;
    QVector<QRegExp> expressions;


public:
    explicit SearchProxyModel(QTableView* view = nullptr, QObject* parent= nullptr);

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const  override;
    void ClearExpressions();
    void Search();


public slots:
    void setExpression(int column, const QString& exp);
};

#endif // SEARCHPROXYMODEL_H
