QT = core gui widgets multimedia

CONFIG += c++latest
QMAKE_CXXFLAGS += -std=c++23

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

HEADERS += \
    codeeditorwidget.h \
    dialogs/createspritedialog.h \
    dialogs/fontpropertiesdialog.h \
    dialogs/imageeditordialog.h \
    dialogs/objectinformationdialog.h \
    dialogs/pathpropertiesdialog.h \
    dialogs/scriptpropertiesdialog.h \
    futurecpp.h \
    jshighlighter.h \
    mainwindow.h \
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
    spritesmodel.h

SOURCES += main.cpp \
    codeeditorwidget.cpp \
    dialogs/createspritedialog.cpp \
    dialogs/fontpropertiesdialog.cpp \
    dialogs/imageeditordialog.cpp \
    dialogs/objectinformationdialog.cpp \
    dialogs/pathpropertiesdialog.cpp \
    dialogs/scriptpropertiesdialog.cpp \
    jshighlighter.cpp \
    mainwindow.cpp \
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
    spritesmodel.cpp

FORMS += \
    dialogs/createspritedialog.ui \
    dialogs/fontpropertiesdialog.ui \
    dialogs/imageeditordialog.ui \
    dialogs/objectinformationdialog.ui \
    dialogs/pathpropertiesdialog.ui \
    dialogs/scriptpropertiesdialog.ui \
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
