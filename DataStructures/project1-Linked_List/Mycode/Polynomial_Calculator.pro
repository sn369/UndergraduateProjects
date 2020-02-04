QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Polynomial Clculator
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    func.cpp

HEADERS  += \
    mainwindow.h

FORMS    += \
    mainwindow.ui
