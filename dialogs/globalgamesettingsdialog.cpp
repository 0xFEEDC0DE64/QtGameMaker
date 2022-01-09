#include "globalgamesettingsdialog.h"
#include "ui_globalgamesettingsdialog.h"

#include <QPushButton>

GlobalGameSettingsDialog::GlobalGameSettingsDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::GlobalGameSettingsDialog>()}
{
    m_ui->setupUi(this);

    setWindowFlags(windowFlags()
                    & ~Qt::Dialog
                   | Qt::Window
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint
                   | Qt::WindowCloseButtonHint);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});
}

GlobalGameSettingsDialog::~GlobalGameSettingsDialog() = default;
