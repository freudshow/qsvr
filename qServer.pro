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
        libproto/lib698/lib698.c \
        libutil/basedef.c \
        libproto/lib698/request.c \
        libproto/lib698/apduType.c \
        libproto/lib698/baseDataType.c \
        libproto/lib698/linkManager.c \
        libproto/lib698/apduManager.c \
        libproto/lib3761/lib3761.c \
        libproto/lib3762/lib3762.c \
        libproto/libcj188/libcj188.c \
        libproto/libdlt645/libdlt645.c \
        libcom/libcom.cpp \
        ui/comconfigform.cpp \
        ui/netconfigform.cpp \
        libdb/libdb.cpp \
        libnet/libnet.cpp \
        ui/setlogicaddr.cpp \
        ui/setrs485.cpp \
    libproto/protocolobj.cpp \
    libproto/libstate/libstate.c \
    ui/toolsdialog.cpp \
    libproto/lib698/lib698obj.cpp \
    libproto/lib698/oiclass.c \
    libproto/lib698/objectidentify.c

HEADERS += \
        mainwindow.h \
        libproto/lib698/lib698.h \
        libutil/basedef.h \
        libproto/lib698/request.h \
        libproto/lib698/apduType.h \
        libproto/lib698/baseDataType.h \
        libproto/lib698/linkManager.h \
        libproto/lib698/apduManager.h \
        libproto/lib3761/lib3761.h \
        libproto/lib3762/lib3762.h \
        libproto/libcj188/libcj188.h \
        libproto/libdlt645/libdlt645.h \
        libcom/libcom.h \
        ui/comconfigform.h \
        ui/netconfigform.h \
        libdb/libdb.h \
        libnet/libnet.h \
        ui/setlogicaddr.h \
        ui/setrs485.h \
    libproto/protocolobj.h \
    libproto/libstate/libstate.h \
    ui/toolsdialog.h \
    libproto/lib698/lib698obj.h \
    libproto/lib698/interfaceclass.h \
    libproto/lib698/objectidentify.h

FORMS += \
        ui/mainwindow.ui \
        ui/comconfigform.ui \
    ui/netconfigform.ui \
    ui/setlogicaddr.ui \
    ui/setrs485.ui \
    ui/toolsdialog.ui

DISTFILES += \
    libproto/lib698/dlt698.45.asn1 \
    config/config.ini
