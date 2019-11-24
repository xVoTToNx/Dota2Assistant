#include "sort_filter_proxy_model.h"

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , expressions(10)
{
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool isAccepted = true;
    for(size_t i = 0; i < sourceModel()->columnCount(); ++i)
    {
        QModelIndex index = sourceModel()->index(sourceRow, i, sourceParent);
        QString str = sourceModel()->data(index).toString();

        if(expressions[i].pattern() != "")
            isAccepted = isAccepted && str.contains(expressions[i]);
    }

    return isAccepted;
}

void SortFilterProxyModel::ClearExpressions()
{
    for(auto i = expressions.begin(); i != expressions.end(); ++i)
        i->setPattern("");
}

void SortFilterProxyModel::setExpression(int column, const QString& exp)
{
    expressions[column].setPattern(exp);
    invalidateFilter();
}
