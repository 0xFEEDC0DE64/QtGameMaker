#include "createspritedialog.h"
#include "ui_createspritedialog.h"

#include <QPushButton>

CreateSpriteDialog::CreateSpriteDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::CreateSpriteDialog>()}
{
    m_ui->setupUi(this);

    setWindowFlags(windowFlags()
                    & ~Qt::Dialog
                   | Qt::Window
                   | Qt::WindowCloseButtonHint);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});
}

CreateSpriteDialog::CreateSpriteDialog(const QSize &size, QWidget *parent) :
    CreateSpriteDialog{parent}
{
    m_ui->spinBoxWidth->setValue(size.width());
    m_ui->spinBoxHeight->setValue(size.height());
}

CreateSpriteDialog::~CreateSpriteDialog() = default;

QSize CreateSpriteDialog::size() const
{
    return QSize{m_ui->spinBoxWidth->value(), m_ui->spinBoxHeight->value()};
}
