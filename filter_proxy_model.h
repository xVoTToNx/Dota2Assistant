#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QTableView>
#include <QDebug>

class FilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

private:
    QVector<QRegExp> expressions;


public:
    explicit FilterProxyModel(QObject* parent= nullptr);

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const  override;
    void ClearExpressions();


public slots:
    void setExpression(int column, const QString& exp);
};

#endif // SORTFILTERPROXYMODEL_H
