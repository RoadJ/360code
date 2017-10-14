#-------------------------------------------------
#
# Project created by QtCreator 2017-07-27T19:15:01
#
#-------------------------------------------------
INCLUDEPATH += D:\opencv\opencv\mingw492\install\include
LIBS+=-LD:\opencv\opencv\mingw492\install\x86\mingw\bin
LIBS += -lopencv_calib3d310 -lopencv_features2d310 -lopencv_flann310 -lopencv_objdetect310 -lopencv_photo310 -lopencv_shape310  -lopencv_core310 -lopencv_highgui310 -lopencv_imgproc310 -lopencv_imgcodecs310 -lopencv_videoio310 -lopencv_imgcodecs310

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ui360
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mainscene.cpp \
    scene360.cpp \
    myitem.cpp

HEADERS  += mainwindow.h \
    mainscene.h \
    scene360.h \
    myitem.h

FORMS    += mainwindow.ui \
    mainwindow360.ui

RESOURCES += \
    imge.qrc
