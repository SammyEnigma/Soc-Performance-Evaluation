TARGET = xlsxwidget
QT += widgets

QT       += core gui sql xlsx

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
    soc/transmissions \
    soc/modules \
    soc/interfaces \
    soc/flowcontrol \
    soc/tools \
    animation \
    utils/xlsx \
    animation/interactive_buttons \
    animation/facile_menu \

SOURCES += \
    animation/numberanimation.cpp \
    dialogs/lookuptabledialog.cpp \
    dialogs/moduledatadialog.cpp \
    dialogs/portdatadialog.cpp \
    dialogs/portpositiondialog.cpp \
    dialogs/routingtabledialog.cpp \
    dialogs/shapedatadialog.cpp \
    dialogs/shapepropertydialog.cpp \
    global/globalvars.cpp \
    global/usettings.cpp \
    graphic/graphicarea.cpp \
    main.cpp \
    mainwindow.cpp \
    shapes/base/cablebase.cpp \
    shapes/base/customdatatype.cpp \
    shapes/base/portbase.cpp \
    shapes/base/selectedge.cpp \
    shapes/base/shapebase.cpp \
    graphic/shapelistwidget.cpp \
    shapes/circleshape.cpp \
    shapes/ellipseshape.cpp \
    shapes/hexagonshape.cpp \
    shapes/modulepanel.cpp \
    shapes/rectangleshape.cpp \
    shapes/squareshape.cpp \
    soc/flowcontrol/flowcontrol_master1_slave1.cpp \
    soc/flowcontrol/flowcontrol_master2_switch_slave2.cpp \
    soc/flowcontrol/flowcontrolautomatic.cpp \
    soc/flowcontrol/flowcontrolbase.cpp \
    soc/flowcontrol/timeframe.cpp \
    soc/flowcontrol/timeframecontroller.cpp \
    soc/interfaces/masterslaveinterface.cpp \
    soc/modules/drammodule.cpp \
    soc/modules/ipmodule.cpp \
    soc/modules/mastermodule.cpp \
    soc/modules/masterslave.cpp \
    soc/modules/modulecable.cpp \
    soc/modules/moduleport.cpp \
    soc/modules/slavemodule.cpp \
    soc/modules/switchmodule.cpp \
    soc/modules/switchpicker.cpp \
    soc/modules/watchmodule.cpp \
    soc/tools/convexhull.cpp \
    soc/tools/watchwidget.cpp \
    soc/transmissions/csvtool.cpp \
    soc/transmissions/datapacket.cpp \
    soc/transmissions/datapacketview.cpp \
    soc/transmissions/sheetdata.cpp \
    soc/transmissions/xlsx_edit.cpp \
    soc/transmissions/xlsxsheetmodel.cpp


HEADERS += \
    animation/numberanimation.h \
    dialogs/lookuptabledialog.h \
    dialogs/moduledatadialog.h \
    dialogs/portdatadialog.h \
    dialogs/portpositiondialog.h \
    dialogs/routingtabledialog.h \
    dialogs/shapedatadialog.h \
    dialogs/shapepropertydialog.h \
    global/Fraction.h \
    global/defines.h \
    global/settings.h \
    global/usettings.h \
    graphic/graphicarea.h \
    shapes/base/cablebase.h \
    shapes/base/customdatatype.h \
    shapes/base/portbase.h \
    shapes/base/selectedge.h \
    graphic/shapelistwidget.h \
    shapes/circleshape.h \
    shapes/ellipseshape.h \
    shapes/hexagonshape.h \
    shapes/modulepanel.h \
    shapes/rectangleshape.h \
    shapes/squareshape.h \
    soc/flowcontrol/flowcontrol_master2_switch_slave2.h \
    soc/flowcontrol/flowcontrolautomatic.h \
    soc/flowcontrol/flowcontrolbase.h \
    soc/flowcontrol/flowcontrol_master1_slave1.h \
    soc/flowcontrol/timeframe.h \
    soc/flowcontrol/timeframecontroller.h \
    soc/interfaces/masterslaveinterface.h \
    soc/interfaces/modulebase.h \
    soc/interfaces/moduleinterface.h \
    soc/modules/drammodule.h \
    soc/modules/ipmodule.h \
    soc/modules/mastermodule.h \
    soc/modules/masterslave.h \
    soc/modules/modulecable.h \
    soc/modules/moduleport.h \
    soc/modules/slavemodule.h \
    soc/modules/switchmodule.h \
    soc/modules/switchpicker.h \
    soc/modules/watchmodule.h \
    soc/tools/convexhull.h \
    soc/tools/watchwidget.h \
    soc/transmissions/csvtool.h \
    soc/transmissions/datapacket.h \
    soc/transmissions/datapacketview.h \
    soc/transmissions/sheetdata.h \
    soc/transmissions/xlsx_edit.h \
    soc/transmissions/xlsxsheetmodel.h \
    soc/transmissions/xlsxsheetmodel_p.h \
    utils/bytearrayutil.h \
    utils/fileutil.h \
    global/globalvars.h \
    global/runtimeinfo.h \
    mainwindow.h \
    shapes/base/shapebase.h \
    utils/stringutil.h

FORMS += \
    dialogs/lookuptabledialog.ui \
    dialogs/moduledatadialog.ui \
    dialogs/mysqldialog.ui \
    dialogs/portdatadialog.ui \
    dialogs/routingtabledialog.ui \
    mainwindow.ui \
    shapedatadialog.ui \
    shapepropertydialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    README.md \
    android/AndroidManifest.xml \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew \
    android/gradlew.bat \
    android/gradlew.bat \
    android/res/values/libs.xml \
    android/res/values/libs.xml \
    flow_control_logic.md

contains(ANDROID_TARGET_ARCH,x86) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
# 该路径是mysql的头文件路径
   # INCLUDEPATH +="C:/Program Files/MySQL/MySQL Server 8.0/include"

#install
    #target.path = $$[QT_INSTALL_EXAMPLES]/xlsx/xlsxwidget
    #INSTALLS += target

}
