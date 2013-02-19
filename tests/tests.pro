QUUNRAR_PATH = ..
include($$QUUNRAR_PATH/QuunRar.pri)

### General config ###

TARGET          = quunrar_tests
VERSION         = 0.1.0
TEMPLATE        = app
QT              += widgets
CONFIG          += static c++11
QMAKE_CXXFLAGS  += $$QUUNRAR_COMMON_QMAKE_CXXFLAGS


### QUUNRAR ###

INCLUDEPATH     += $$QUUNRAR_INCLUDEPATH
LIBS            += $$QUUNRAR_LIBS


### Files ###

SOURCES += \
    main.cpp \
    extractionwidget.cpp

HEADERS += \
    extractionwidget.h

FORMS += \
    extractionwidget.ui


# Copies the given files to the destination directory
defineTest(copyToDestdir) {
    files = $$1

    for(FILE, files) {
        DDIR = $$OUT_PWD

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

TESTFILES = \
$$PWD/testData/testData.password.rar \
$$PWD/testData/testData.password.split.part1.rar \
$$PWD/testData/testData.password.split.part2.rar \
$$PWD/testData/testData.password.split.part3.rar \
$$PWD/testData/testData.password.split.part4.rar \
$$PWD/testData/testData.rar \
$$PWD/testData/testData.split.part1.rar \
$$PWD/testData/testData.split.part2.rar \
$$PWD/testData/testData.split.part3.rar \
$$PWD/testData/testData.split.part4.rar \
$$PWD/testData/testData/binaryData \
$$PWD/testData/testData/testfile.txt

copyToDestdir($$TESTFILES)
