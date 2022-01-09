#include "extensionpackagesdialog.h"
#include "ui_extensionpackagesdialog.h"

ExtensionPackagesDialog::ExtensionPackagesDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ExtensionPackagesDialog>()}
{
    m_ui->setupUi(this);
    setWindowFlags(windowFlags()
                   & ~Qt::Dialog
                   | Qt::Window
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint
                   | Qt::WindowCloseButtonHint);
}

ExtensionPackagesDialog::~ExtensionPackagesDialog() = default;
