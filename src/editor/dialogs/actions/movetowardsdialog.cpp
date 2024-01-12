#include "movetowardsdialog.h"
#include "ui_movetowardsdialog.h"

#include <QPushButton>

MoveTowardsDialog::MoveTowardsDialog(MoveTowardsAction &action, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::MoveTowardsDialog>()},
    m_action{action}
{
    m_ui->setupUi(this);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});
}

MoveTowardsDialog::~MoveTowardsDialog() = default;
