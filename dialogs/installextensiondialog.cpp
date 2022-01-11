#include "installextensiondialog.h"
#include "ui_installextensiondialog.h"

InstallExtensionDialog::InstallExtensionDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::InstallExtensionDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);
}

InstallExtensionDialog::~InstallExtensionDialog() = default;
