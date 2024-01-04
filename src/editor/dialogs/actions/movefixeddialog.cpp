#include "movefixeddialog.h"
#include "ui_movefixeddialog.h"

#include <QPushButton>

MoveFixedDialog::MoveFixedDialog(MoveFixedAction &action, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::MoveFixedDialog>()},
    m_action{action}
{
    m_ui->setupUi(this);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});
}

MoveFixedDialog::~MoveFixedDialog() = default;
