#include "filter_proxy_model.h"

FilterProxyModel::FilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , expressions(15)
{
}

bool FilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool isAccepted = true;
    for(size_t i = 0; i < sourceModel()->columnCount(); ++i)
    {
        QModelIndex index = sourceModel()->index(sourceRow, i, sourceParent);
        QString str = sourceModel()->data(index).toString();

        if(expressions[i].pattern() != "")
            isAccepted = isAccepted && str.toLower().contains(expressions[i]);
    }
    return isAccepted;
}

void FilterProxyModel::ClearExpressions()
{
    for(auto i = expressions.begin(); i != expressions.end(); ++i)
        i->setPattern("");
}

void FilterProxyModel::setExpression(int column, const QString& exp)
{
    expressions[column].setPattern(exp.toLower());
    invalidateFilter();
}
