TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

QT += core gui printsupport

SOURCES += \
    facedetector.cpp \
    C:/Dev/dlib-19.7/dlib/all/source.cpp \
    face.cpp \
    utils.cpp \
    qcustomplot.cpp \
    _main.cpp \
    main.cpp \
    fatiguedetector.cpp \
    mainapplication.cpp \
    testwindow.cpp

win32:CONFIG(release, debug|release): LIBS += -LC:/Dev/OpenCV/opencv/build/x64/vc14/lib/ -lopencv_world320
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/Dev/OpenCV/opencv/build/x64/vc14/lib/ -lopencv_world320d

unix:LIBS += -lopencv_core \
             -lopencv_imgproc \
             -lopencv_highgui \
             -lopencv_ml \
             -lopencv_video \
             -lopencv_features2d \
             -lopencv_calib3d \
             -lopencv_objdetect \
             -lopencv_contrib \
             -lopencv_legacy \
             -lopencv_flann

win32:INCLUDEPATH += C:/Dev/OpenCV/opencv/build/include
win32:DEPENDPATH += C:/Dev/OpenCV/opencv/build/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += C:/Dev/OpenCV/opencv/build/x64/vc14/lib/libopencv_world320.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += C:/Dev/OpenCV/opencv/build/x64/vc14/lib/libopencv_world320d.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += C:/Dev/OpenCV/opencv/build/x64/vc14/lib/opencv_world320.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += C:/Dev/OpenCV/opencv/build/x64/vc14/lib/opencv_world320d.lib

INCLUDEPATH += C:/Dev/dlib-19.7

HEADERS += \
		facedetector.h \
	    face.h \
	    facelandmark68.h \
	    utils.h \
	    qcustomplot.h \
	    fatiguedetector.h \
	    mainapplication.h \
	    testwindow.h \
	    config.h

target.path = /home/pi/FatigueDetector
INSTALLS += target

QMAKE_CXXFLAGS += -O2