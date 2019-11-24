#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QDebug>

class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit SortFilterProxyModel(QObject* parent= nullptr);

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const  override;
    void ClearExpressions();


public slots:
    void setExpression(int column, const QString& exp);

private:
    QVector<QRegExp> expressions;
};

#endif // SORTFILTERPROXYMODEL_H
