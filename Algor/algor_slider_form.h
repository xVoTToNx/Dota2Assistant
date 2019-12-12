#ifndef ALGORSLIDERFORM_H
#define ALGORSLIDERFORM_H

#include <QWidget>
#include <QVector>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

class AlgorTabWidget;

class AlgorSliderForm : public QWidget
{
    Q_OBJECT

    AlgorTabWidget* algor_tab;

    QGridLayout* form_layout;
    QGridLayout* layout;

    QVector<QLabel*> value_labels;
    QVector<QSlider*> sliders;

    QPushButton* avg_button;
    QPushButton* rand_button;


public:
    AlgorSliderForm(AlgorTabWidget* algor_tab, QWidget* parent = nullptr);
    ~AlgorSliderForm();

private slots:
    void updateSliderValue(int slider_index, int value);
    void AVG();
    void RAND();
};

#endif // ALGORSLIDERFORM_H
