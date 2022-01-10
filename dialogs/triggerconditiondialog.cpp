#include "triggerconditiondialog.h"

TriggerConditionDialog::TriggerConditionDialog(QWidget *parent) :
    CodeEditorDialog{parent}
{
#ifdef Q_OS_LINUX
    setWindowFlags(windowFlags() & ~Qt::Dialog | Qt::Window);
#endif
    setWindowFlag(Qt::WindowMinimizeButtonHint);
    setWindowFlag(Qt::WindowMaximizeButtonHint);
    setWindowFlag(Qt::WindowCloseButtonHint);
}
