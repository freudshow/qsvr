#-------------------------------------------------
#
# Project created by QtCreator 2018-01-07T10:39:29
#
#-------------------------------------------------

QT       += core gui network serialport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qServer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        lib698/lib698.c \
        lib698/fcs.c \
        libutil/basedef.c \
        lib698/request.c \
        lib698/apduType.c \
        lib698/baseDataType.c \
        lib698/linkManager.c \
        lib698/apduManager.c \
        qdlt698.cpp \
        libcom/libcom.cpp \
        ui/comconfigform.cpp \
        ui/netconfigform.cpp \
    libdb/libdb.cpp \
    libnet/libnet.cpp \
    ui/setlogicaddr.cpp \
    ui/setrs485.cpp

HEADERS += \
        mainwindow.h \
        lib698/lib698.h \
        libutil/basedef.h \
        lib698/fcs.h \
        lib698/request.h \
        lib698/apduType.h \
        lib698/baseDataType.h \
        lib698/linkManager.h \
        lib698/apduManager.h \
        qdlt698.h \
        libcom/libcom.h \
        ui/comconfigform.h \
        ui/netconfigform.h \
    libdb/libdb.h \
    libnet/libnet.h \
    ui/setlogicaddr.h \
    ui/setrs485.h

FORMS += \
        ui/mainwindow.ui \
        ui/comconfigform.ui \
    ui/netconfigform.ui \
    ui/setlogicaddr.ui \
    ui/setrs485.ui

DISTFILES += \
    lib698/dlt698.45.asn1 \
    config/config.ini
