#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T21:36:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qvtkDicomViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    surfacereconstruction.cpp

HEADERS  += mainwindow.h \
    surfacereconstruction.h

FORMS    += mainwindow.ui

OTHER_FILES +=

QMAKE_CXXFLAGS += -Wno-deprecated

LIBS += -lvtkCommon \
        -lvtkRendering \
        -lvtkVolumeRendering \
        -lQVTK \
        -lvtkIO \
        -lvtkFiltering \
        -lvtkgdcm \
        -lvtkGraphics \
        -lvtkImaging \
        -lvtksys \
        -lgdcmCommon \
        -lvtkgdcm \
        -lgdcmMSFF \
        -lgdcmDSED
INCLUDEPATH += /usr/include/vtk-5.8/
DEPENDPATH += /usr/include/vtk-5.8/

INCLUDEPATH += /usr/include/gdcm-2.4
DEPENDPATH += /usr/include/gdcm-2.4

