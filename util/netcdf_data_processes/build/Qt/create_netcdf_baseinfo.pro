QT += core
QT -= gui

CONFIG += c++11

TARGET = create_netcdf_baseinfo
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    ../../createbaseinfo_netcdf.cpp

LIBS += -L/usr/lib -lnetcdf -lgomp -lpthread -fopenmp

HEADERS += \
    ../../pubtools.h \
    ../../esrigridclass.h
