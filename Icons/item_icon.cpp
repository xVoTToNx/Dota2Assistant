#include "item_icon.h"
#include "main_window.h"

ItemIcon::ItemIcon(QString item_name, QWidget *parent)
  : QWidget(parent)
  , main_layout (new QVBoxLayout(this))
  , pic_data_layout (new QHBoxLayout())
  , data_layout (new QGridLayout())
  , recomendations_list (new QListWidget())
{
  QSqlQuery qry(MainWindow::data_base);
  qry.prepare(QString("select * from items where item_name = '" + item_name + "'"));
  qry.exec(); qry.next();

  QLabel* pic = new QLabel(this);
  pic->setFixedSize(300,300);
  QPixmap pixmap(qry.value("picture_path").toString());
  if(!pixmap.isNull())
      pic->setPixmap(pixmap.scaled(300, 300, Qt::KeepAspectRatio));

  pic_data_layout->addWidget(pic);

  QGroupBox* data_group = new QGroupBox(this);

  QLabel* hero_name_label = MainWindow::CreateLabel(item_name);
  data_layout->addWidget(hero_name_label, 0, 0);
  QLabel* price_label = MainWindow::CreateLabel("Price: " + qry.value("price").toString());
  data_layout->addWidget(price_label, 1, 0);
  QLabel* accessibility_label = MainWindow::CreateLabel("Accessibility: " + qry.value("accessibility").toString());
  data_layout->addWidget(accessibility_label, 2, 0);
  QLabel* desc_label = MainWindow::CreateLabel("Description: " + qry.value("description").toString());
  desc_label->setWordWrap(true);
  data_layout->addWidget(desc_label, 3, 0);
  data_group->setLayout(data_layout);


  data_layout->setAlignment(Qt::AlignCenter);
  pic_data_layout->addWidget(data_group);

  main_layout->addLayout(pic_data_layout);
  main_layout->addWidget(recomendations_list);

  connect(recomendations_list, &QListWidget::itemClicked,
          [](QListWidgetItem * item){auto icon = HeroIcon::CreateMe(item->data(DATA_ROLE).toString());
          icon->setAttribute(Qt::WA_DeleteOnClose); icon->show(); });
  fillRecomendationsList(item_name);
}

ItemIcon* ItemIcon::CreateMe(QString item_name)
{
    return new ItemIcon(item_name);
}

void ItemIcon::fillRecomendationsList(QString item_name)
{
    QSqlQuery qry(MainWindow::data_base);
    qry.prepare(QString("select heroes.hero_name, is_contr, description, picture_path from recomendations , heroes "
                        "where item_name = '" + item_name + "' and heroes.hero_name = recomendations.hero_name order by hero_name, is_contr"));
    qry.exec();
    while(qry.next())
    {
        QWidget* recomendation = new QWidget();
        QGridLayout* recomendation_layout = new QGridLayout(recomendation);

        QLabel* pic = new QLabel();
        pic->setFixedSize(100,100);
        QLabel* refl_pic = new QLabel();
        pic->setFixedSize(100,100);

        QPixmap pixmap(qry.value("picture_path").toString());
        if(!pixmap.isNull())
        {
            pic->setPixmap(pixmap.scaled(100, 100, Qt::KeepAspectRatio));
            pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio);
            refl_pic->setPixmap(pixmap.transformed(QTransform().scale(-1,1)));
        }

        int offset = 0;
        if(qry.value("is_contr").toInt() == 0)
        {
            recomendation_layout->addWidget(pic, 0, 0, 2, 2);
            QLabel* hero_label = MainWindow::CreateLabel(qry.value("hero_name").toString());
            recomendation_layout->addWidget(hero_label, 0, 2);
            QLabel* desc_label = MainWindow::CreateLabel(qry.value("description").toString());
            desc_label->setWordWrap(true);
            recomendation_layout->addWidget(desc_label, 1, 2);

            QString hero_name = qry.value("hero_name").toString();
            if(qry.next())
            {
                if(hero_name == qry.value("hero_name").toString())
                    offset += 3;
                else
                    qry.previous();
            }
        }
        if(qry.value("is_contr").toInt() == 1)
        {
            QLabel* hero_label = MainWindow::CreateLabel(qry.value("hero_name").toString());
            recomendation_layout->addWidget(hero_label, 0, 0 + offset);
            QLabel* desc_label = MainWindow::CreateLabel(qry.value("description").toString());
            desc_label->setWordWrap(true);
            recomendation_layout->addWidget(desc_label, 1, 0 + offset);
            recomendation_layout->addWidget(refl_pic, 0, 1 + offset, 2, 2);
        }

        QListWidgetItem* item = new QListWidgetItem();
        item->setData(DATA_ROLE, qry.value("hero_name"));
        item->setSizeHint({400, 120});
        recomendations_list->addItem(item);
        recomendations_list->setItemWidget(item, recomendation);
    }
}
