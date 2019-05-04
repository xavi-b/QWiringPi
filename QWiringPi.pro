TEMPLATE        = lib
CONFIG         += c++17
DEFINES        += QT_DEPRECATED_WARNINGS
QT              = core
TARGET          = qwiringpi

LIBS += -lwiringPi

SUBDIRS += \
    src/

include(src/src.pri)
