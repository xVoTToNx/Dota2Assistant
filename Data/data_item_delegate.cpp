#include "data_item_delegate.h"
#include "main_window.h"

DataItemDelegate::DataItemDelegate(QSqlTableModel *model, QWidget *parent)
    : QStyledItemDelegate(parent)
    , sql_model(model)
{

}

void DataItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QByteArray n = editor->metaObject()->userProperty().name();
    QString id = model->data(model->index(index.row(), 0)).toString();
    QString value = editor->property(n).toString();

    if(editor->property(n).type() == QVariant::Type::String)
        value = "'" + value + "'";

    if(model->data(model->index(index.row(), 0)).type() == QVariant::Type::String)
        id = "'" + id + "'";
    QSqlQuery qry;
    qry.prepare(QString("update %1 set %2 = %3 where %4 = %5").arg(
                    sql_model->tableName(),
                    HEADER(sql_model,index.column()),
                    value, HEADER(sql_model,0), id));

    qry.exec();
    sql_model->setTable(sql_model->tableName());
    sql_model->select();

    //QStyledItemDelegate::setModelData(editor, model, index);
}
