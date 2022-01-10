QT = core gui widgets multimedia

CONFIG += c++latest
QMAKE_CXXFLAGS += -std=c++23

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

HEADERS += \
    codeeditorwidget.h \
    constantsmodel.h \
    dialogs/codeeditordialog.h \
    dialogs/createspritedialog.h \
    dialogs/fontpropertiesdialog.h \
    dialogs/imageeditordialog.h \
    dialogs/includedfilesdialog.h \
    dialogs/maskpropertiesdialog.h \
    dialogs/objectinformationdialog.h \
    dialogs/objectpropertiesdialog.h \
    dialogs/pathpropertiesdialog.h \
    dialogs/roompropertiesdialog.h \
    dialogs/scriptpropertiesdialog.h \
    dialogs/timelinepropertiesdialog.h \
    dialogs/triggerconditiondialog.h \
    dialogs/triggersdialog.h \
    dialogs/userdefinedconstantsdialog.h \
    drawingcanvaswidget.h \
    futurecpp.h \
    includedfilesmodel.h \
    jshighlighter.h \
    mainwindow.h \
    pathpointsmodel.h \
    pathpointswidget.h \
    projectcontainer.h \
    projecttreemodel.h \
    dialogs/backgroundpropertiesdialog.h \
    dialogs/editspritedialog.h \
    dialogs/extensionpackagesdialog.h \
    dialogs/gameinformationdialog.h \
    dialogs/globalgamesettingsdialog.h \
    dialogs/preferencesdialog.h \
    dialogs/soundpropertiesdialog.h \
    dialogs/spritepropertiesdialog.h \
    spritesmodel.h \
    triggersmodel.h

SOURCES += main.cpp \
    codeeditorwidget.cpp \
    constantsmodel.cpp \
    dialogs/codeeditordialog.cpp \
    dialogs/createspritedialog.cpp \
    dialogs/fontpropertiesdialog.cpp \
    dialogs/imageeditordialog.cpp \
    dialogs/includedfilesdialog.cpp \
    dialogs/maskpropertiesdialog.cpp \
    dialogs/objectinformationdialog.cpp \
    dialogs/objectpropertiesdialog.cpp \
    dialogs/pathpropertiesdialog.cpp \
    dialogs/roompropertiesdialog.cpp \
    dialogs/scriptpropertiesdialog.cpp \
    dialogs/timelinepropertiesdialog.cpp \
    dialogs/triggerconditiondialog.cpp \
    dialogs/triggersdialog.cpp \
    dialogs/userdefinedconstantsdialog.cpp \
    drawingcanvaswidget.cpp \
    includedfilesmodel.cpp \
    jshighlighter.cpp \
    mainwindow.cpp \
    pathpointsmodel.cpp \
    pathpointswidget.cpp \
    projectcontainer.cpp \
    projecttreemodel.cpp \
    dialogs/backgroundpropertiesdialog.cpp \
    dialogs/editspritedialog.cpp \
    dialogs/extensionpackagesdialog.cpp \
    dialogs/gameinformationdialog.cpp \
    dialogs/globalgamesettingsdialog.cpp \
    dialogs/preferencesdialog.cpp \
    dialogs/soundpropertiesdialog.cpp \
    dialogs/spritepropertiesdialog.cpp \
    spritesmodel.cpp \
    triggersmodel.cpp

FORMS += \
    dialogs/codeeditordialog.ui \
    dialogs/createspritedialog.ui \
    dialogs/fontpropertiesdialog.ui \
    dialogs/imageeditordialog.ui \
    dialogs/includedfilesdialog.ui \
    dialogs/maskpropertiesdialog.ui \
    dialogs/objectinformationdialog.ui \
    dialogs/objectpropertiesdialog.ui \
    dialogs/pathpropertiesdialog.ui \
    dialogs/roompropertiesdialog.ui \
    dialogs/timelinepropertiesdialog.ui \
    dialogs/triggersdialog.ui \
    dialogs/userdefinedconstantsdialog.ui \
    mainwindow.ui \
    dialogs/backgroundpropertiesdialog.ui \
    dialogs/editspritedialog.ui \
    dialogs/extensionpackagesdialog.ui \
    dialogs/gameinformationdialog.ui \
    dialogs/globalgamesettingsdialog.ui \
    dialogs/preferencesdialog.ui \
    dialogs/soundpropertiesdialog.ui \
    dialogs/spritepropertiesdialog.ui

RESOURCES += \
    resources.qrc
