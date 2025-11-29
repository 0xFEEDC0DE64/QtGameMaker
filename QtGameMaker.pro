QT += \
    core \
    gui \
    widgets \
    multimedia \
    opengl

CONFIG += c++latest
QMAKE_CXXFLAGS += \
    -std=c++23 \
    -Wno-missing-field-initializers \
    -Wno-unused-variable \
    -Wall \
    -Wpedantic \
    -Werror

!greaterThan(QT_MAJOR_VERSION, 5) {
    QMAKE_CXXFLAGS += -Wno-deprecated-enum-enum-conversion
}

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

INCLUDEPATH += \
    $$PWD/src \
    $$PWD/src/editor

HEADERS += \
    src/closeeventfilter.h \
    src/editor/dialogs/actions/setgravitydialog.h \
    src/editor/dialogs/actions/speedhorizontaldialog.h \
    src/editor/dialogs/actions/movefreedialog.h \
    src/editor/dialogs/actions/movetowardsdialog.h \
    src/editor/dialogs/actions/executecodedialog.h \
    src/editor/dialogs/actions/speedverticaldialog.h \
    src/editor/dialogs/genericcodeeditordialog.h \
    src/editor/dialogs/includedfilepropertiesdialog.h \
    src/editor/dialogs/transparentbackgroundsettingsdialog.h \
    src/editor/editorguiutils.h \
    src/editor/roomscene.h \
    src/editor/widgets/actiondragwidget.h \
    src/editor/widgets/coarsecolorwidget.h \
    src/editor/widgets/colorwidget.h \
    src/editor/widgets/draggabletreeview.h \
    src/editor/widgets/finecolorwidget.h \
    src/editor/widgets/objectparentselectorwidget.h \
    src/editor/widgets/objectselectorwidget.h \
    src/editor/widgets/qlineeditwithmenu.h \
    src/editor/widgets/qscrollareawithmenu.h \
    src/editor/widgets/roomeditwidget.h \
    src/editor/widgets/zoomablescrollarea.h \
    src/engine/gameengine.h \
    src/engine/glgamewindow.h \
    src/engine/vulkangamerenderer.h \
    src/engine/vulkangamewindow.h \
    src/futurecpp.h \
    src/projectcontainer.h \
    src/editor/jshighlighter.h \
    src/editor/mainwindow.h \
    src/editor/dialogs/addeventdialog.h \
    src/editor/dialogs/backgroundpropertiesdialog.h \
    src/editor/dialogs/codeeditordialog.h \
    src/editor/dialogs/createspritedialog.h \
    src/editor/dialogs/deletemomentsdialog.h \
    src/editor/dialogs/editspritedialog.h \
    src/editor/dialogs/extensionpackagesdialog.h \
    src/editor/dialogs/fontpropertiesdialog.h \
    src/editor/dialogs/gameinformationdialog.h \
    src/editor/dialogs/globalgamesettingsdialog.h \
    src/editor/dialogs/imageeditordialog.h \
    src/editor/dialogs/includedfilesdialog.h \
    src/editor/dialogs/installextensiondialog.h \
    src/editor/dialogs/maskpropertiesdialog.h \
    src/editor/dialogs/objectinformationdialog.h \
    src/editor/dialogs/objectpropertiesdialog.h \
    src/editor/dialogs/pathpropertiesdialog.h \
    src/editor/dialogs/preferencesdialog.h \
    src/editor/dialogs/roompropertiesdialog.h \
    src/editor/dialogs/scriptpropertiesdialog.h \
    src/editor/dialogs/soundpropertiesdialog.h \
    src/editor/dialogs/spritepropertiesdialog.h \
    src/editor/dialogs/timelinepropertiesdialog.h \
    src/editor/dialogs/triggersdialog.h \
    src/editor/dialogs/userdefinedconstantsdialog.h \
    src/editor/dialogs/actions/movefixeddialog.h \
    src/editor/models/actionscontainermodel.h \
    src/editor/models/constantsmodel.h \
    src/editor/models/includedfilesmodel.h \
    src/editor/models/objecteventsmodel.h \
    src/editor/models/pathpointsmodel.h \
    src/editor/models/projecttreemodel.h \
    src/editor/models/spritesmodel.h \
    src/editor/models/timelinemomentsmodel.h \
    src/editor/models/triggersmodel.h \
    src/editor/widgets/actionscontainerwidget.h \
    src/editor/widgets/codeeditorwidget.h \
    src/editor/widgets/drawingcanvaswidget.h \
    src/editor/widgets/pathpointswidget.h \
    src/projectserialization.h \
    src/stdserialization.h

SOURCES += \
    src/closeeventfilter.cpp \
    src/editor/dialogs/actions/executecodedialog.cpp \
    src/editor/dialogs/actions/setgravitydialog.cpp \
    src/editor/dialogs/actions/speedhorizontaldialog.cpp \
    src/editor/dialogs/actions/movefreedialog.cpp \
    src/editor/dialogs/actions/movetowardsdialog.cpp \
    src/editor/dialogs/actions/speedverticaldialog.cpp \
    src/editor/dialogs/genericcodeeditordialog.cpp \
    src/editor/dialogs/includedfilepropertiesdialog.cpp \
    src/editor/dialogs/transparentbackgroundsettingsdialog.cpp \
    src/editor/editorguiutils.cpp \
    src/editor/roomscene.cpp \
    src/editor/widgets/actiondragwidget.cpp \
    src/editor/widgets/coarsecolorwidget.cpp \
    src/editor/widgets/colorwidget.cpp \
    src/editor/widgets/draggabletreeview.cpp \
    src/editor/widgets/finecolorwidget.cpp \
    src/editor/widgets/objectparentselectorwidget.cpp \
    src/editor/widgets/objectselectorwidget.cpp \
    src/editor/widgets/qlineeditwithmenu.cpp \
    src/editor/widgets/qscrollareawithmenu.cpp \
    src/editor/widgets/roomeditwidget.cpp \
    src/editor/widgets/zoomablescrollarea.cpp \
    src/engine/gameengine.cpp \
    src/engine/glgamewindow.cpp \
    src/engine/vulkangamerenderer.cpp \
    src/engine/vulkangamewindow.cpp \
    src/main.cpp \
    src/projectcontainer.cpp \
    src/editor/jshighlighter.cpp \
    src/editor/mainwindow.cpp \
    src/editor/dialogs/addeventdialog.cpp \
    src/editor/dialogs/backgroundpropertiesdialog.cpp \
    src/editor/dialogs/codeeditordialog.cpp \
    src/editor/dialogs/createspritedialog.cpp \
    src/editor/dialogs/deletemomentsdialog.cpp \
    src/editor/dialogs/editspritedialog.cpp \
    src/editor/dialogs/extensionpackagesdialog.cpp \
    src/editor/dialogs/fontpropertiesdialog.cpp \
    src/editor/dialogs/gameinformationdialog.cpp \
    src/editor/dialogs/globalgamesettingsdialog.cpp \
    src/editor/dialogs/imageeditordialog.cpp \
    src/editor/dialogs/includedfilesdialog.cpp \
    src/editor/dialogs/installextensiondialog.cpp \
    src/editor/dialogs/maskpropertiesdialog.cpp \
    src/editor/dialogs/objectinformationdialog.cpp \
    src/editor/dialogs/objectpropertiesdialog.cpp \
    src/editor/dialogs/pathpropertiesdialog.cpp \
    src/editor/dialogs/preferencesdialog.cpp \
    src/editor/dialogs/roompropertiesdialog.cpp \
    src/editor/dialogs/scriptpropertiesdialog.cpp \
    src/editor/dialogs/soundpropertiesdialog.cpp \
    src/editor/dialogs/spritepropertiesdialog.cpp \
    src/editor/dialogs/timelinepropertiesdialog.cpp \
    src/editor/dialogs/triggersdialog.cpp \
    src/editor/dialogs/userdefinedconstantsdialog.cpp \
    src/editor/dialogs/actions/movefixeddialog.cpp \
    src/editor/models/actionscontainermodel.cpp \
    src/editor/models/constantsmodel.cpp \
    src/editor/models/includedfilesmodel.cpp \
    src/editor/models/objecteventsmodel.cpp \
    src/editor/models/pathpointsmodel.cpp \
    src/editor/models/projecttreemodel.cpp \
    src/editor/models/spritesmodel.cpp \
    src/editor/models/timelinemomentsmodel.cpp \
    src/editor/models/triggersmodel.cpp \
    src/editor/widgets/actionscontainerwidget.cpp \
    src/editor/widgets/codeeditorwidget.cpp \
    src/editor/widgets/drawingcanvaswidget.cpp \
    src/editor/widgets/pathpointswidget.cpp \
    src/projectserialization.cpp

FORMS += \
    src/editor/dialogs/actions/setgravitydialog.ui \
    src/editor/dialogs/actions/speedhorizontaldialog.ui \
    src/editor/dialogs/actions/movefreedialog.ui \
    src/editor/dialogs/actions/movetowardsdialog.ui \
    src/editor/dialogs/actions/speedverticaldialog.ui \
    src/editor/dialogs/includedfilepropertiesdialog.ui \
    src/editor/dialogs/transparentbackgroundsettingsdialog.ui \
    src/editor/mainwindow.ui \
    src/editor/dialogs/addeventdialog.ui \
    src/editor/dialogs/backgroundpropertiesdialog.ui \
    src/editor/dialogs/codeeditordialog.ui \
    src/editor/dialogs/createspritedialog.ui \
    src/editor/dialogs/deletemomentsdialog.ui \
    src/editor/dialogs/editspritedialog.ui \
    src/editor/dialogs/extensionpackagesdialog.ui \
    src/editor/dialogs/fontpropertiesdialog.ui \
    src/editor/dialogs/gameinformationdialog.ui \
    src/editor/dialogs/globalgamesettingsdialog.ui \
    src/editor/dialogs/imageeditordialog.ui \
    src/editor/dialogs/includedfilesdialog.ui \
    src/editor/dialogs/installextensiondialog.ui \
    src/editor/dialogs/maskpropertiesdialog.ui \
    src/editor/dialogs/objectinformationdialog.ui \
    src/editor/dialogs/objectpropertiesdialog.ui \
    src/editor/dialogs/pathpropertiesdialog.ui \
    src/editor/dialogs/preferencesdialog.ui \
    src/editor/dialogs/roompropertiesdialog.ui \
    src/editor/dialogs/soundpropertiesdialog.ui \
    src/editor/dialogs/spritepropertiesdialog.ui \
    src/editor/dialogs/timelinepropertiesdialog.ui \
    src/editor/dialogs/triggersdialog.ui \
    src/editor/dialogs/userdefinedconstantsdialog.ui \
    src/editor/widgets/actionscontainerwidget.ui\
    src/editor/dialogs/actions/movefixeddialog.ui \
    src/editor/widgets/objectselectorwidget.ui

RESOURCES += \
    src/editor/resources_editor.qrc \
    src/engine/resources_engine.qrc

OTHER_FILES += \
    src/engine/vulkan_shader_modules/color.frag \
    src/engine/vulkan_shader_modules/color.vert \
    src/engine/vulkan_shader_modules/texture.frag \
    src/engine/vulkan_shader_modules/texture.vert
