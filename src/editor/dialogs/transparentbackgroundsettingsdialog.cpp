#include "transparentbackgroundsettingsdialog.h"
#include "ui_transparentbackgroundsettingsdialog.h"

TransparentBackgroundSettingsDialog::TransparentBackgroundSettingsDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::TransparentBackgroundSettingsDialog>()}
{
    m_ui->setupUi(this);
}

TransparentBackgroundSettingsDialog::~TransparentBackgroundSettingsDialog() = default;
