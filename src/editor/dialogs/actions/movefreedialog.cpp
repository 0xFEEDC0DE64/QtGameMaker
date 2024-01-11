#include "movefreedialog.h"
#include "ui_movefreedialog.h"

#include <QPushButton>

MoveFreeDialog::MoveFreeDialog(MoveFreeAction &action, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::MoveFreeDialog>()},
    m_action{action}
{
    m_ui->setupUi(this);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});
}

MoveFreeDialog::~MoveFreeDialog() = default;
