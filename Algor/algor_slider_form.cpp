#include "algor_slider_form.h"
#include "algor_tab_widget.h"

AlgorSliderForm::AlgorSliderForm(AlgorTabWidget* algor_tab, QWidget* parent)
    : QWidget(parent)
    , algor_tab (algor_tab)
    , form_layout (new QGridLayout(this))
    , layout (new QGridLayout())
    , avg_button (new QPushButton("AVG"))
    , rand_button (new QPushButton("RND"))
{
    form_layout->addLayout(layout, 0, 0, 1, 2);

    layout->setColumnStretch(0, 10000000);
    layout->setColumnMinimumWidth(1, 30);
    layout->setColumnMinimumWidth(2, 30);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(0, 2);

    QSqlQuery qry;
    qry.prepare("select role_name from roles order by role_name");
    qry.exec();

    int i = 0;
    while(qry.next())
    {
        QLabel* label = new QLabel(qry.value(0).toString());
        label->setAlignment(Qt::AlignHCenter);

        layout->addWidget(label, i++, 0, 1, 2);

        QSlider* slider = new QSlider(Qt::Horizontal);
        slider->setMaximum(25);
        slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
        sliders << slider;
        layout->addWidget(slider, i, 0);

        QLabel* value = new QLabel();
        value_labels << value;
        layout->addWidget(value, i, 1);

        QPushButton* checkButton = new QPushButton();
        checkButton->setFixedSize(30,30);
        check_buttons << checkButton;
        layout->addWidget(checkButton, i++, 2);


        int data_index = i / 2 - 1;

        int current_value = algor_tab->getDesiredRoleData(data_index);
        slider->setValue(current_value);
        value->setText(QString::number(current_value));

        connect(slider, &QSlider::valueChanged, [this, data_index](int val)
        { updateSliderValue(data_index, val); });

        connect(checkButton, &QPushButton::clicked, [this, data_index]()
        { updateAttributesCheckBoxes(data_index); });
    }

    connect (avg_button, &QPushButton::clicked, this, &AlgorSliderForm::AVG);
    connect (rand_button, &QPushButton::clicked, this, &AlgorSliderForm::RAND);

    form_layout->addWidget(avg_button, 1, 0);
    form_layout->addWidget(rand_button, 1, 1);

    setAttribute(Qt::WA_DeleteOnClose);
}

AlgorSliderForm::~AlgorSliderForm()
{}

void AlgorSliderForm::updateSliderValue(int slider_index, int value)
{
    sliders[slider_index]->setValue(value);
    value_labels[slider_index]->setText(QString::number(value));
    algor_tab->setDesiredRoleData(slider_index, value);
    algor_tab->replotStatsPlot(2);
}

void AlgorSliderForm::updateAttributesCheckBoxes(int new_index)
{
    int primary_attribute_index = algor_tab->getPrimaryAttributeIndex();
    int secondary_attribute_index = algor_tab->getSecondaryAttributeIndex();

    if(primary_attribute_index == -1 && secondary_attribute_index != new_index)
    {
        primary_attribute_index = new_index;
        check_buttons[primary_attribute_index]->setStyleSheet("background-color: lime");
        check_buttons[primary_attribute_index]->setText("Prim");
    }
    else if(secondary_attribute_index == -1 && primary_attribute_index != new_index)
    {
        secondary_attribute_index = new_index;
        check_buttons[secondary_attribute_index]->setStyleSheet("background-color: cyan");
        check_buttons[secondary_attribute_index]->setText("Sec");
    }
    else if(new_index == primary_attribute_index)
    {
        check_buttons[primary_attribute_index]->setStyleSheet("");
        check_buttons[primary_attribute_index]->setText("");
        primary_attribute_index = -1;
    }
    else if(new_index == secondary_attribute_index)
    {
        check_buttons[secondary_attribute_index]->setStyleSheet("");
        check_buttons[secondary_attribute_index]->setText("");
        secondary_attribute_index = -1;
    }
    else
    {
        check_buttons[secondary_attribute_index]->setStyleSheet("");
        check_buttons[secondary_attribute_index]->setText("");

        secondary_attribute_index = new_index;
        check_buttons[secondary_attribute_index]->setStyleSheet("background-color: cyan");
        check_buttons[secondary_attribute_index]->setText("Sec");
    }

    algor_tab->setPrimaryAttributeIndex(primary_attribute_index);
    algor_tab->setSecondaryAttributeIndex(secondary_attribute_index);
}

void AlgorSliderForm::AVG()
{
    for(int i = 0; i < algor_tab->sizeDesiredToleData(); ++i)
    {
        int value = 12;
        updateSliderValue(i, value);
    }

    algor_tab->replotStatsPlot(2);
}

void AlgorSliderForm::RAND()
{
    std::mt19937 gen(time(0));
    std::uniform_int_distribution<> uid(0, 25);

    for(int i = 0; i < algor_tab->sizeDesiredToleData(); ++i)
    {
        int value = uid(gen);
        updateSliderValue(i, value);
    }

    algor_tab->replotStatsPlot(2);
}

