#include "speedverticaldialog.h"
#include "ui_speedverticaldialog.h"

#include <QPushButton>

SpeedVerticalDialog::SpeedVerticalDialog(SpeedVerticalAction &action, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::SpeedVerticalDialog>()},
    m_action{action}
{
    m_ui->setupUi(this);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});
}

SpeedVerticalDialog::~SpeedVerticalDialog() = default;
