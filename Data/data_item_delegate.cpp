#include "data_item_delegate.h"
#include "main_window.h"

DataItemDelegate::DataItemDelegate(QSqlTableModel *model, QWidget *parent)
    : QStyledItemDelegate(parent)
    , sql_model(model)
{

}

QWidget *DataItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(sql_model->headerData(index.column(), Qt::Orientation::Horizontal, Qt::UserRole + 1).toString() == "combo")
    {
        QComboBox* box = new QComboBox(parent);
        box->addItems(sql_model->headerData(index.column(), Qt::Orientation::Horizontal, Qt::UserRole + 2).value<QStringList>());
        box->setProperty("my_type", "combo");
        return box;
    }
    QStyledItemDelegate::createEditor(parent, option, index);
}

void DataItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(editor->property("my_type").toString() == "combo")
    {
        QComboBox* box = static_cast<QComboBox*>(editor);
        int indexed_text = box->findText(sql_model->data(index).toString());
        box->setCurrentIndex(indexed_text);
        return;
    }
    QStyledItemDelegate::setEditorData(editor, index);
}

void DataItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QByteArray n = editor->metaObject()->userProperty().name();
    QString id = model->data(model->index(index.row(), 0)).toString();
    QString value = editor->property(n).toString();

    if(editor->property(n).type() == QVariant::Type::String)
        value = "'" + value + "'";

    QSqlQuery qry;
    QString qry_text = QString("update %1 set %2 = %3 where ").arg(
                sql_model->tableName(), HEADER(sql_model,index.column()), value);

    for(int i = 0; i < sql_model->columnCount(); ++i)
    {
        QVariant vvalue = sql_model->data(sql_model->index(index.row(), i));
        QString value_to_add = vvalue.toString();
        if(vvalue.type() == QVariant::String)
            value_to_add = "'" + value_to_add + "'";

        qry_text += HEADER(sql_model,i) + " = " + value_to_add + " and ";
    }
    qry_text.chop(4);

    qry.prepare(qry_text);

    qDebug()<<qry.exec();
    qDebug()<<qry.lastQuery();
    model->setData(index, editor->property(n), Qt::EditRole);
}
