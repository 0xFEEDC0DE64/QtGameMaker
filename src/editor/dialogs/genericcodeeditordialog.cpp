#include "genericcodeeditordialog.h"

GenericCodeEditorDialog::GenericCodeEditorDialog(const QString &title, QWidget *parent) :
    CodeEditorDialog{title, parent}
{
#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowMinimizeButtonHint);
    setWindowFlag(Qt::WindowMaximizeButtonHint);
    setWindowFlag(Qt::WindowCloseButtonHint);
}
