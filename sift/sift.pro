TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    ImageProcess.cpp \
    SiftProcess.cpp \
    Visual.cpp

HEADERS += \
    main.h \
    ImageProcess.h \
    SiftProcess.h \
    Visual.h

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
