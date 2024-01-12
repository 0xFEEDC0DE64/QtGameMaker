#include "speedhorizontaldialog.h"
#include "ui_speedhorizontaldialog.h"

#include <QPushButton>

SpeedHorizontalDialog::SpeedHorizontalDialog(SpeedHorizontalAction &action, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::SpeedHorizontalDialog>()},
    m_action{action}
{
    m_ui->setupUi(this);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});
}

SpeedHorizontalDialog::~SpeedHorizontalDialog() = default;
