QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    shapes \
    global \
    utils \
    shapes \
    shapes/base \
    graphic \
    dialogs \
    soc \
    soc/transmission \

SOURCES += \
    dialogs/portpositiondialog.cpp \
    dialogs/shapepropertydialog.cpp \
    global/globalvars.cpp \
    global/usettings.cpp \
    graphic/graphicarea.cpp \
    main.cpp \
    mainwindow.cpp \
    shapes/base/cablebase.cpp \
    shapes/base/portbase.cpp \
    shapes/base/selectedge.cpp \
    shapes/base/shapebase.cpp \
    shapes/base/shapelistwidget.cpp \
    shapes/circleshape.cpp \
    shapes/ellipseshape.cpp \
    shapes/hexagonshape.cpp \
    soc/transmission/datapacket.cpp

HEADERS += \
    dialogs/portpositiondialog.h \
    dialogs/shapepropertydialog.h \
    global/settings.h \
    global/usettings.h \
    graphic/graphicarea.h \
    shapes/base/cablebase.h \
    shapes/base/portbase.h \
    shapes/base/selectedge.h \
    shapes/base/shapelistwidget.h \
    shapes/circleshape.h \
    shapes/ellipseshape.h \
    shapes/hexagonshape.h \
    soc/transmission/datapacket.h \
    utils/bytearrayutil.h \
    utils/fileutil.h \
    global/globalvars.h \
    global/runtimeinfo.h \
    mainwindow.h \
    shapes/base/shapebase.h \
    utils/stringutil.h

FORMS += \
    mainwindow.ui \
    shapepropertydialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
