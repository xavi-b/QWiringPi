TEMPLATE        = lib
CONFIG         += c++17
DEFINES        += QT_DEPRECATED_WARNINGS
QT              = core
TARGET          = qwiringpi

unix {
target.path = /usr/lib/
INSTALLS += target
}

LIBS += -lwiringPi

SUBDIRS += \
    src/

include(src/src.pri)
