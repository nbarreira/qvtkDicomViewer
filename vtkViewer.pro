#-------------------------------------------------
#
# Project created by QtCreator 2014-04-08T21:36:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vtkViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    surfacereconstruction.cpp

HEADERS  += mainwindow.h \
    surfacereconstruction.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    CMakeLists.txt

win32:CONFIG(release, debug|release): LIBS  += -lvtkCommonCore-6
else:win32:CONFIG(debug, debug|release): LIBS +=  -lvtkCommon \
        -lvtkRendering \
        -lvtkVolumeRendering \
        -lQVTK \
        -lvtkIO \
        -lvtkFiltering \
        -lvtkgdcm \
        -lvtkGraphics \
        -lvtkImaging \
        -lvtksys \
        -lgdcmMSFF \
        -lgdcmDSED
else:unix: LIBS +=   -L/usr/local/lib/ -lvtkCommon \
        -lvtkRendering \
        -lvtkVolumeRendering \
        -lQVTK \
        -lvtkIO \
        -lvtkFiltering \
        -lvtkgdcm \
        -lvtkGraphics \
        -lvtkImaging \
        -lvtksys \
        -lgdcmMSFF \
        -lgdcmDSED
INCLUDEPATH += /usr/include/vtk-5.8/
DEPENDPATH += /usr/include/vtk-5.8/

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../usr/lib/release/ -lgdcmCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../usr/lib/debug/ -lgdcmCommon
else:unix: LIBS += -L$$PWD/../../../../../../../usr/lib/ -lgdcmCommon

INCLUDEPATH += $$PWD/../../../../../../../usr/include/gdcm-2.4
DEPENDPATH += $$PWD/../../../../../../../usr/include/gdcm-2.4

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../../usr/lib/release/ -lvtkgdcm
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../../usr/lib/debug/ -lvtkgdcm
else:unix: LIBS += -L$$PWD/../../../../../../../usr/lib/ -lvtkgdcm

