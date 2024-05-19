QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    decrypt.cpp \
    encrypt.cpp \
    main.cpp \
    mainwindow.cpp \
    sha512.cpp

HEADERS += \
    decrypt.h \
    encrypt.h \
    mainwindow.h \
    sha512.h

FORMS += \
    decrypt.ui \
    encrypt.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SUBDIRS += \
    ../lab3/lab3.pro
