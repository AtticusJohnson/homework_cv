#-------------------------------------------------
#
# Project created by QtCreator 2020-05-29T20:36:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SiftHuster
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

CONFIG += c++11

SOURCES += \
        main.cpp \
#    imageviewer/qimageviewer.cpp \
#    mainwindow/mainwindow.cpp \
#    mainwindow/qabout.cpp \
#    sift/ImageProcess.cpp \
#    sift/mainprogram.cpp \
#    sift/SiftProcess.cpp \
#    sift/Visual.cpp

#HEADERS += \
#    imageviewer/qimageviewer.h \
#    mainwindow/mainwindow.h \
#    mainwindow/qabout.h \
#    sift/ImageProcess.h \
#    sift/MainProgram.h \
#    sift/SiftProcess.h \
#    sift/Visual.h \

RESOURCES += \
    images.qrc

INCLUDEPATH += mainwindow
include(mainwindow/mainwindow.pri)
include(imageviewer/imageviewer.pri)
include(sift/sift.pri)
FORMS += \
        mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH+=d:/opencv2413/build/include/opencv \
                    d:/opencv2413/build/include/opencv2 \
                   d:/opencv2413/build/include
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_ml2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_calib3d2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_contrib2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_core2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_features2d2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_flann2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_gpu2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_highgui2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_imgproc2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_legacy2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_objdetect2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_ts2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_video2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_nonfree2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_ocl2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_photo2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_stitching2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_superres2413d.lib
LIBS+=D:\opencv2413\build\x64\vc14\lib\opencv_videostab2413d.lib



