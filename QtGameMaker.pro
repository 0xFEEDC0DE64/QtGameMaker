QT = core gui widgets multimedia

CONFIG += c++latest
QMAKE_CXXFLAGS += \
    -std=c++23 \
    -Wno-missing-field-initializers \
    -Wno-sign-compare

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

HEADERS += \
    dialogs/deletemomentsdialog.h \
    models/actionscontainermodel.h \
    widgets/actionscontainerwidget.h \
    widgets/codeeditorwidget.h \
    models/constantsmodel.h \
    dialogs/addeventdialog.h \
    dialogs/codeactiondialog.h \
    dialogs/codeeditordialog.h \
    dialogs/createspritedialog.h \
    dialogs/fontpropertiesdialog.h \
    dialogs/imageeditordialog.h \
    dialogs/includedfilesdialog.h \
    dialogs/installextensiondialog.h \
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
    widgets/drawingcanvaswidget.h \
    futurecpp.h \
    models/includedfilesmodel.h \
    jshighlighter.h \
    mainwindow.h \
    models/objecteventsmodel.h \
    models/pathpointsmodel.h \
    widgets/pathpointswidget.h \
    projectcontainer.h \
    models/projecttreemodel.h \
    dialogs/backgroundpropertiesdialog.h \
    dialogs/editspritedialog.h \
    dialogs/extensionpackagesdialog.h \
    dialogs/gameinformationdialog.h \
    dialogs/globalgamesettingsdialog.h \
    dialogs/preferencesdialog.h \
    dialogs/soundpropertiesdialog.h \
    dialogs/spritepropertiesdialog.h \
    models/spritesmodel.h \
    models/timelinemomentsmodel.h \
    models/triggersmodel.h

SOURCES += main.cpp \
    dialogs/deletemomentsdialog.cpp \
    models/actionscontainermodel.cpp \
    widgets/actionscontainerwidget.cpp \
    widgets/codeeditorwidget.cpp \
    models/constantsmodel.cpp \
    dialogs/addeventdialog.cpp \
    dialogs/codeactiondialog.cpp \
    dialogs/codeeditordialog.cpp \
    dialogs/createspritedialog.cpp \
    dialogs/fontpropertiesdialog.cpp \
    dialogs/imageeditordialog.cpp \
    dialogs/includedfilesdialog.cpp \
    dialogs/installextensiondialog.cpp \
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
    widgets/drawingcanvaswidget.cpp \
    models/includedfilesmodel.cpp \
    jshighlighter.cpp \
    mainwindow.cpp \
    models/objecteventsmodel.cpp \
    models/pathpointsmodel.cpp \
    widgets/pathpointswidget.cpp \
    projectcontainer.cpp \
    models/projecttreemodel.cpp \
    dialogs/backgroundpropertiesdialog.cpp \
    dialogs/editspritedialog.cpp \
    dialogs/extensionpackagesdialog.cpp \
    dialogs/gameinformationdialog.cpp \
    dialogs/globalgamesettingsdialog.cpp \
    dialogs/preferencesdialog.cpp \
    dialogs/soundpropertiesdialog.cpp \
    dialogs/spritepropertiesdialog.cpp \
    models/spritesmodel.cpp \
    models/timelinemomentsmodel.cpp \
    models/triggersmodel.cpp

FORMS += \
    dialogs/addeventdialog.ui \
    dialogs/codeeditordialog.ui \
    dialogs/createspritedialog.ui \
    dialogs/deletemomentsdialog.ui \
    dialogs/fontpropertiesdialog.ui \
    dialogs/imageeditordialog.ui \
    dialogs/includedfilesdialog.ui \
    dialogs/installextensiondialog.ui \
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
    dialogs/spritepropertiesdialog.ui \
    widgets/actionscontainerwidget.ui

RESOURCES += \
    resources.qrc
