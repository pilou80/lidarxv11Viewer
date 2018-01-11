#-------------------------------------------------
#
# Project created by QtCreator 2016-06-16T10:42:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

TARGET = lidarxv11viewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    draw360.cpp \
    lidarxv11.cpp \
    serialclass.cpp

HEADERS  += mainwidget.h \
    draw360.h \
    lidarxv11.h \
    serialclass.h

FORMS    += mainwidget.ui \
    serialclass.ui
