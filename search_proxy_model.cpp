#include "search_proxy_model.h"

SearchProxyModel::SearchProxyModel(QTableView* view, QObject *parent)
    : QSortFilterProxyModel(parent)
    , view (view)
    , start_row (new int(-1))
    , current_row (new int(-1))
    , go_next(new bool(false))
    , expressions(15)
{
}

bool SearchProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool isAccepted = true;

    for(size_t i = 0; i < sourceModel()->columnCount(); ++i)
    {
        QModelIndex index = sourceModel()->index(sourceRow, i, sourceParent);
        QString str = sourceModel()->data(index).toString();

        if(expressions[i].pattern() != "")
            isAccepted = isAccepted && str.toLower().contains(expressions[i]);
    }

    if(isAccepted && *start_row == -1)
        *start_row = sourceRow;

    if(isAccepted && *go_next)
        if(sourceRow > *current_row)
        {
            view->selectRow(sourceRow);
            *current_row = sourceRow;
            *go_next = false;
            return true;
        }

    if(sourceRow == sourceModel()->rowCount() - 1 && *go_next)
    {
        view->selectRow(*start_row);
        *current_row = *start_row;
        *go_next = false;
    }

    return true;
}

void SearchProxyModel::ClearExpressions()
{
    for(auto i = expressions.begin(); i != expressions.end(); ++i)
        i->setPattern("");
}

void SearchProxyModel::setExpression(int column, const QString& exp)
{
    expressions[column].setPattern(exp.toLower());
}

void SearchProxyModel::Search()
{
    *go_next = true;
    *start_row = -1;
    invalidateFilter();
}
