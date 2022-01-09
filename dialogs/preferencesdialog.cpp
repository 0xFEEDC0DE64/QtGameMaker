#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::PreferencesDialog>()}
{
    m_ui->setupUi(this);
    setWindowFlags(windowFlags()
                       & ~Qt::Dialog
                   | Qt::Window
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint
                   | Qt::WindowCloseButtonHint);
}

PreferencesDialog::~PreferencesDialog() = default;
