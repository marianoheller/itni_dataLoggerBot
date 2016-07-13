#-------------------------------------------------
#
# Project created by QtCreator 2016-06-08T12:54:43
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dataLoggerBot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    httprequestworker.cpp

HEADERS  += mainwindow.h \
    httprequestworker.h

FORMS    += mainwindow.ui
