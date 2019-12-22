#ifndef DATAITEMDELEGATE_H
#define DATAITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QDebug>
#include <QMetaProperty>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTableView>

class DataItemDelegate : public QStyledItemDelegate
{
    QSqlTableModel* sql_model;
    QTableView* table_view;

public:
    DataItemDelegate(QSqlTableModel* model, QTableView* view, QWidget* parent = nullptr);
    QWidget* createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void paint( QPainter *painter,const QStyleOptionViewItem &option, const QModelIndex &index ) const override;

private:
    QVariant getModelData(int const row, int const column) const;
    int getProperIndex(int const row) const;
    QVariant getHeaderData(const int index, int const role) const;
    QString callTextBoxDialog(QString& initial_text) const;
};

#endif // DATAITEMDELEGATE_H
