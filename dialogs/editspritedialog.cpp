#include "editspritedialog.h"
#include "ui_editspritedialog.h"

#include "projectcontainer.h"

EditSpriteDialog::EditSpriteDialog(Sprite &sprite, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::EditSpriteDialog>()},
    m_sprite{sprite}
{
    m_ui->setupUi(this);
    setWindowFlags(windowFlags()
                    & ~Qt::Dialog
                   | Qt::Window
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint
                   | Qt::WindowCloseButtonHint);

    setWindowTitle(tr("Sprite editor - %0").arg(m_sprite.name));
}

EditSpriteDialog::~EditSpriteDialog() = default;
