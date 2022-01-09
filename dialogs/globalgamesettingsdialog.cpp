#include "globalgamesettingsdialog.h"
#include "ui_globalgamesettingsdialog.h"

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
}

GlobalGameSettingsDialog::~GlobalGameSettingsDialog() = default;
