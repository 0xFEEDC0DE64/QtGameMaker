#include "movetowardsdialog.h"
#include "ui_movetowardsdialog.h"

MoveTowardsDialog::MoveTowardsDialog(MoveTowardsAction &action, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::MoveTowardsDialog>()},
    m_action{action}
{
    m_ui->setupUi(this);
}

MoveTowardsDialog::~MoveTowardsDialog() = default;
