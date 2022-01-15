#include "extensionpackagesdialog.h"
#include "ui_extensionpackagesdialog.h"

#include "installextensiondialog.h"

ExtensionPackagesDialog::ExtensionPackagesDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ExtensionPackagesDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    if (auto button = m_ui->buttonBox->addButton(tr("Install"), QDialogButtonBox::ActionRole))
    {
        button->setIcon(QIcon{":/qtgameengine/icons/extension-packages-file.png"});
        connect(button, &QAbstractButton::clicked,
                this, &ExtensionPackagesDialog::install);
    }
}

ExtensionPackagesDialog::~ExtensionPackagesDialog() = default;

void ExtensionPackagesDialog::install()
{
    InstallExtensionDialog dialog{this};
    dialog.exec();
}
