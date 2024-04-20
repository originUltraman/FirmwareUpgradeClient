QT -= gui
QT += core
QT += network
QT += xml



LIBS += -L$$PWD/spdlog/build/ -lspdlog

INCLUDEPATH += $$PWD/spdlog/include
DEPENDPATH += $$PWD/spdlog/include
PRE_TARGETDEPS += $$PWD/spdlog/build/libspdlog.a

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
DEFINES += SPDLOG_COMPILED_LIB

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        crc/crc.cpp \
        dir.cpp \
        firmwareupgrade.cpp \
        main.cpp \
        mylog.cpp \
        protocal/utility.cpp \
        tftp/tftp.cpp \
        thread/findthread.cpp \
        thread/menuthread.cpp \
        thread/mythread.cpp \
        thread/odownloadthread.cpp \
        thread/statusfilercvthread.cpp \
        thread/uploadthread.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    crc/crc.h \
    device/device.h \
    dir.h \
    firmwareupgrade.h \
    global.h \
    globalDefine.h \
    inputListener.h \
    mylog.h \
    protocal/file_define.h \
    protocal/protocal.h \
    protocal/utility.h \
    singleton/singleton.h \
    tftp/tftp.h \
    tftp/tftprequest.h \
    thread/findthread.h \
    thread/menuthread.h \
    thread/mythread.h \
    thread/odownloadthread.h \
    thread/statusfilercvthread.h \
    thread/uploadthread.h
