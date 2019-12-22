#include "data_item_delegate.h"
#include "main_window.h"

DataItemDelegate::DataItemDelegate(QSqlTableModel *model, QTableView* view, QWidget *parent)
    : QStyledItemDelegate(parent)
    , sql_model(model)
    , table_view(view)
{

}

QWidget *DataItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QString data_type = getHeaderData(index.column(), Qt::UserRole + 1).toString();

    if (data_type == "combo")
    {
        QComboBox* box = new QComboBox(parent);
        box->addItems(getHeaderData(index.column(), Qt::UserRole + 2).value<QStringList>());
        box->setProperty("my_type", "combo");
        return box;
    }
    else if (data_type == "boolean")
    {
        QComboBox* box = new QComboBox(parent);
        box->addItems(getHeaderData(index.column(), Qt::UserRole + 2).value<QStringList>());
        box->setProperty("my_type", "boolean");
        return box;
    }
    else if (data_type == "const")
    {
        return nullptr;
    }
    else if (data_type == "picture")
    {
        QLineEdit* edit = new QLineEdit(parent);
        edit->setProperty("my_type", "picture");
        return edit;
    }
    else if (data_type == "description")
    {
        QLineEdit* edit = new QLineEdit(parent);
        edit->setProperty("my_type", "description");
        return edit;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void DataItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int proper_index = getProperIndex(index.row());
    if(editor->property("my_type").toString() == "combo")
    {
        QComboBox* box = static_cast<QComboBox*>(editor);
        int indexed_text = box->findText(getModelData(proper_index, index.column()).toString());
        box->setCurrentIndex(indexed_text);
        return;
    }
    else if(editor->property("my_type").toString() == "boolean")
    {
        QComboBox* box = static_cast<QComboBox*>(editor);
        bool table_value = getModelData(proper_index, index.column()).toInt() ? true : false;
        // In database 1 == true and 0 == false
        // But in ComboBox true has index(0) and false has index(1)
        int indexed_text = !table_value;
        box->setCurrentIndex(indexed_text);
        return;
    }
    else if(editor->property("my_type").toString() == "picture")
    {
        QLineEdit* edit = static_cast<QLineEdit*>(editor);
        QString fileName = QFileDialog::getOpenFileName(editor->parentWidget(),
            tr("Open Image"), "C:/", tr("Image Files (*.png *.jpg)"));

        if(fileName == "")
            fileName = getModelData(proper_index, index.column()).toString();

        edit->setText(fileName);
        editor->setProperty("my_type", "picture_used");
        return;
    }
    if(editor->property("my_type").toString() == "description")
    {
        QLineEdit* edit = static_cast<QLineEdit*>(editor);
        QString initial_text = getModelData(proper_index, index.column()).toString();
        QString text_desc = callTextBoxDialog(initial_text);

        edit->setText(text_desc);
        editor->setProperty("my_type", "description_used");
        return;
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

void DataItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    int proper_index = getProperIndex(index.row());
    QByteArray n = editor->metaObject()->userProperty().name();
    QVariant variant = editor->property(n);

    if(variant.toString() == getModelData(proper_index, index.column()).toString())
        return;

    if(HEADER(sql_model, index.column()) == "move_speed" && variant.toInt() < 100)
        return;
    if(HEADER(sql_model, index.column()) == "agility" && variant.toInt() < 0)
        return;
    if(HEADER(sql_model, index.column()) == "intelligence" && variant.toInt() < 0)
        return;
    if(HEADER(sql_model, index.column()) == "strength" && variant.toInt() < 101)
        return;
    if(HEADER(sql_model, index.column()) == "max_lvl" && variant.toInt() < 1)
        return;
    if(HEADER(sql_model, index.column()) == "price" && (variant.toInt() < 50 || variant.toInt() > 10000))
        return;
    if(HEADER(sql_model, index.column()) == "mark"  && (variant.toInt() < 0 || variant.toInt() > 10))
        return;

    if(editor->property("my_type").toString() == "boolean")
    {
        variant = MainWindow::VariantToSql(variant, QVariant::Type::Bool) == "1" ? QVariant(1)
                                                                                 : QVariant(0);
    }

    QString value = MainWindow::VariantToSql(variant);

    QSqlQuery qry;
    QString qry_text = QString("update %1 set %2 = %3 where ").arg(
                sql_model->tableName(), HEADER(sql_model,index.column()), value);

    for(int i = 0; i < sql_model->columnCount(); ++i)
    {
        QVariant vvalue = getModelData(proper_index, i);
        QString value_to_add = MainWindow::VariantToSql(vvalue);

        qry_text += HEADER(sql_model,i) + " = " + value_to_add + " and ";
    }
    qry_text.chop(4);
    qry.prepare(qry_text);
    if(qry.exec())
        model->setData(index, variant, Qt::EditRole);
    else
        MainWindow::ThrowError(qry.lastError().text());
}

void DataItemDelegate::paint( QPainter *painter,const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    if (index.isValid())
    {
        QVariant data = index.data();
        if(getHeaderData(index.column(), Qt::UserRole + 1) == "boolean")
            data = data.toInt() ? QVariant(true) : QVariant(false);

        painter->save();

        if(option.state & QStyle::State_Selected)
        {
            painter->setBrush(QColor(0, 0, 255, 125));
            painter->drawRect(option.rect);
        }
        painter->drawText(option.rect, Qt::AlignCenter, displayText(data, QLocale::system()));

        painter->restore();
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QVariant DataItemDelegate::getModelData(const int row, const int column) const
{
    return sql_model->data(sql_model->index(row, column));
}

int DataItemDelegate::getProperIndex(int const row) const
{
    return table_view->model()->headerData(row, Qt::Vertical, 0).toInt() - 1;

}

QVariant DataItemDelegate::getHeaderData(const int index, int const role) const
{
    return sql_model->headerData(index, Qt::Orientation::Horizontal, role);
}

QString DataItemDelegate::callTextBoxDialog(QString &initial_text) const
{
    QString result = initial_text;

    QDialog dialog;
    QGridLayout* dialog_layout = new QGridLayout();
    dialog.setLayout(dialog_layout);

    QTextEdit* text_edit = new QTextEdit(initial_text);

    QPushButton* ok_button = new QPushButton("OK");
    QPushButton* cancel_button = new QPushButton("Cancel");

    ok_button->setFixedHeight(30);
    cancel_button->setFixedHeight(30);

    text_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ok_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cancel_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    dialog_layout->addWidget(text_edit, 0, 0, 1, 2);
    dialog_layout->addWidget(ok_button, 1, 0);
    dialog_layout->addWidget(cancel_button, 1, 1);

    connect(ok_button, &QPushButton::clicked, [&dialog, text_edit, &result]()
    { result = text_edit->toPlainText(); dialog.close(); });

    connect(cancel_button, &QPushButton::clicked, [&dialog]()
    { dialog.close(); });

    dialog.exec();

    return result;
}
