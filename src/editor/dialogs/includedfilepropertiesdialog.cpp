#include "includedfilepropertiesdialog.h"
#include "ui_includedfilepropertiesdialog.h"

IncludedFilePropertiesDialog::IncludedFilePropertiesDialog(IncludedFile &includedFile, QWidget *parent) :
    QDialog{parent},
    m_includedFile{includedFile},
    m_ui{std::make_unique<Ui::IncludedFilePropertiesDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);
}

IncludedFilePropertiesDialog::~IncludedFilePropertiesDialog() = default;

void IncludedFilePropertiesDialog::accept()
{
    QDialog::reject();
}
