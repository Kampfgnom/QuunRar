isEmpty(QUUNRAR_PATH) {
    error(You have to set QUUNRAR_PATH to the path of QUUNRAR)
}

QUUNRAR_TARGET           = quunrar
QUUNRAR_VERSION          = 0.1.0
QUUNRAR_INCLUDEPATH      = $$PWD/include
QUUNRAR_LIBS             = -L$$QUUNRAR_PATH/src -l$$QUUNRAR_TARGET

QUUNRAR_QMAKE_CXXFLAGS = -Wall -ansi -pedantic \ # Gives us more compiler warnings
                         -Wno-long-long  # When using -pedantic, the qglobal.h does not
                                         # compile because illegal usage of long long
