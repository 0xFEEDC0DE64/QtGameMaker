#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QPushButton>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::PreferencesDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags(windowFlags() & ~Qt::Dialog | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});
}

PreferencesDialog::~PreferencesDialog() = default;
