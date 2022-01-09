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

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});
    m_ui->buttonBox->addButton(tr("Install"), QDialogButtonBox::ActionRole)
        ->setIcon(QIcon{":/qtgameengine/icons/extension-packages-file.png"});
}

ExtensionPackagesDialog::~ExtensionPackagesDialog() = default;
