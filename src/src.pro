QUUNRAR_PATH = ..
include($$QUUNRAR_PATH/QuunRar.pri)

### General config ###

TARGET          = $$QUUNRAR_TARGET
VERSION         = $$QUUNRAR_VERSION
TEMPLATE        = lib
QT              +=
QT              -= gui
CONFIG          += static c++11
QMAKE_CXXFLAGS  += $$QUUNRAR_COMMON_QMAKE_CXXFLAGS
INCLUDEPATH     += $$QUUNRAR_INCLUDEPATH

# Remove comment to build QuunRar in its own namespace
# QUUNRAR_NAMESPACE = QuunRar


### Unrar DLL ###

include(../lib/unrar/unrar.pri)
INCLUDEPATH += $$UNRAR_INCLUDEPATH


### Files ###

HEADERS += \
    quunrar.h \
    quunrarjob.h \
    quunrarfileinfo.h \
    quunrar_p.h

SOURCES += \
    quunrar.cpp \
    quunrarjob.cpp \
    quunrarfileinfo.cpp
