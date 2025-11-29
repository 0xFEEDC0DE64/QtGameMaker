#include "createspritedialog.h"
#include "ui_createspritedialog.h"

#include <QPushButton>

#include "editorguiutils.h"

CreateSpriteDialog::CreateSpriteDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::CreateSpriteDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);

    improveButtonBox(m_ui->buttonBox);
}

CreateSpriteDialog::CreateSpriteDialog(const QSize &size, QWidget *parent) :
    CreateSpriteDialog{parent}
{
    m_ui->spinBoxWidth->setValue(size.width());
    m_ui->spinBoxHeight->setValue(size.height());
}

CreateSpriteDialog::~CreateSpriteDialog() = default;

QSize CreateSpriteDialog::spriteSize() const
{
    return QSize{m_ui->spinBoxWidth->value(), m_ui->spinBoxHeight->value()};
}
