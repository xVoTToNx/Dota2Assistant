QT       += core gui
QT       += sql
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Algor/algor_slider_form.cpp \
    Algor/algor_tab_widget.cpp \
    Data/data_item_delegate.cpp \
    Data/data_tab_widget.cpp \
    Icons/hero_icon.cpp \
    Icons/icons_tab_widget.cpp \
    Icons/item_icon.cpp \
    Icons/team_icon.cpp \
    filter_proxy_model.cpp \
    main.cpp \
    main_window.cpp \
    qcpdocumentobject.cpp \
    qcustomplot.cpp \
    search_proxy_model.cpp

HEADERS += \
    Algor/algor_slider_form.h \
    Algor/algor_tab_widget.h \
    Data/data_item_delegate.h \
    Data/data_tab_widget.h \
    Icons/hero_icon.h \
    Icons/icons_tab_widget.h \
    Icons/item_icon.h \
    Icons/team_icon.h \
    filter_proxy_model.h \
    main_window.h \
    qcpdocumentobject.h \
    qcustomplot.h \
    search_proxy_model.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
