TARGET   = printservice
TEMPLATE = app
CONFIG   += console qt
QT += printsupport network pdf

HEADERS += \
           controller/healthcontroller.h \
           controller/logincontroller.h \
           controller/printersetupcontroller.h \
           controller/printpzcontroller.h \
           global.h \
           iniutil.h \
           requestmapper.h \
           controller/dumpcontroller.h \
           controller/templatecontroller.h \
           controller/formcontroller.h \
           controller/fileuploadcontroller.h \
           controller/sessioncontroller.h \
           retjson.h

SOURCES += main.cpp \
           controller/healthcontroller.cpp \
           controller/logincontroller.cpp \
           controller/printersetupcontroller.cpp \
           controller/printpzcontroller.cpp \
           global.cpp \
           iniutil.cpp \
           requestmapper.cpp \
           controller/dumpcontroller.cpp \
           controller/templatecontroller.cpp \
           controller/formcontroller.cpp \
           controller/fileuploadcontroller.cpp \
           controller/sessioncontroller.cpp \
           retjson.cpp

OTHER_FILES += ../etc/* ../etc/docroot/* ../etc/templates/* ../etc/ssl/* ../logs/*

include(../qtservice/src/qtservice.pri)
include(../http/logging/logging.pri)
include(../http/httpserver/httpserver.pri)
include(../http/templateengine/templateengine.pri)

