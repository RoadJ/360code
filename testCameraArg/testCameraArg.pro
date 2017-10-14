#-------------------------------------------------
#
# Project created by QtCreator 2017-03-20T11:52:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


windows{

message("windows");

INCLUDEPATH += D:\opencv\opencv\mingw492\install\include
LIBS+=-LD:\opencv\opencv\mingw492\install\x86\mingw\bin
LIBS += -lopencv_calib3d310 -lopencv_features2d310 -lopencv_flann310 -lopencv_objdetect310 -lopencv_photo310 -lopencv_shape310  -lopencv_core310 -lopencv_highgui310 -lopencv_imgproc310 -lopencv_imgcodecs310 -lopencv_videoio310 -lopencv_imgcodecs310

}


#android{

#message("android");

##QT += androidextras
##QT += multimedia multimediawidgets

##CONFIG += mobility

##MOBILITY =

#ANDROID_OPENCV = /home/zsh/android_env/OpenCV-android-sdk/sdk/native

#INCLUDEPATH += \
#    $$ANDROID_OPENCV/jni/include/opencv    \
#    $$ANDROID_OPENCV/jni/include/opencv2    \
#    $$ANDROID_OPENCV/jni/include

#LIBS +=\
#     #$$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_contrib.a \
#     #$$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_legacy.a \
#    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_ml.a \
#    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_objdetect.a \
#    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_calib3d.a \
#    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_video.a \
#    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_features2d.a \
#    $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_highgui.a \
#     #$$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_androidcamera.a \
#     $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_flann.a \
#     $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_imgproc.a \
#     $$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_core.a     \
#     $$ANDROID_OPENCV/3rdparty/libs/armeabi-v7a/liblibjpeg.a \
#     $$ANDROID_OPENCV/3rdparty/libs/armeabi-v7a/liblibpng.a \
#     $$ANDROID_OPENCV/3rdparty/libs/armeabi-v7a/liblibtiff.a \
#     $$ANDROID_OPENCV/3rdparty/libs/armeabi-v7a/liblibjasper.a \
#     $$ANDROID_OPENCV/3rdparty/libs/armeabi-v7a/libtbb.a

# LIBS +=$$ANDROID_OPENCV/libs/armeabi-v7a/libopencv_java3.so

# LIBS += $$PWD/lib4_in_read.so

# LIBS += $$PWD/libg2d.so

#HEADERS  += g2d.h \
#    g2d_yuyv_bgra.h \
#    4_in_read.h \
#    v4l2_work_fourin.h

#SOURCES += g2d_yuyv_bgra.cpp \
#    v4l2_work_fourin.cpp
#}

#contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
#    ANDROID_EXTRA_LIBS = \
#        /home/zsh/android_env/OpenCV-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_java3.so \
#        $$PWD/lib4_in_read.so \
#        $$PWD/libg2d.so
#}

TARGET = testCameraArg
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sourcepicture.cpp \
    descpicture.cpp

HEADERS  += mainwindow.h \
    sourcepicture.h \
    descpicture.h \
    public.h

#CONFIG += mobility
#MOBILITY =

#DISTFILES += \
#    android/AndroidManifest.xml \
#    android/gradle/wrapper/gradle-wrapper.jar \
#    android/gradlew \
#    android/res/values/libs.xml \
#    android/build.gradle \
#    android/gradle/wrapper/gradle-wrapper.properties \
#    android/gradlew.bat

#ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

FORMS += \
    mainwindow.ui \
    sourcepicture.ui \
    descpicture.ui

