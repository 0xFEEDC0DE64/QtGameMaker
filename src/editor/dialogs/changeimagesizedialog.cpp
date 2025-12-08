#include "changeimagesizedialog.h"
#include "ui_changeimagesizedialog.h"

#include "editorguiutils.h"

ChangeImageSizeDialog::ChangeImageSizeDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ChangeImageSizeDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);

    improveButtonBox(m_ui->buttonBox);
}

ChangeImageSizeDialog::~ChangeImageSizeDialog() = default;
