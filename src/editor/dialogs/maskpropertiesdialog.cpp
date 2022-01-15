#include "maskpropertiesdialog.h"
#include "ui_maskpropertiesdialog.h"

MaskPropertiesDialog::MaskPropertiesDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::MaskPropertiesDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);
}

MaskPropertiesDialog::~MaskPropertiesDialog() = default;
