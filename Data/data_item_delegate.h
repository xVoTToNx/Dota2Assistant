#ifndef DATAITEMDELEGATE_H
#define DATAITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QDebug>
#include <QMetaProperty>
#include <QSqlQuery>
#include <QSqlTableModel>

class DataItemDelegate : public QStyledItemDelegate
{
    QSqlTableModel* sql_model;
public:
    DataItemDelegate(QSqlTableModel* model, QWidget* parent = nullptr);
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
};

#endif // DATAITEMDELEGATE_H
