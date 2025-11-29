#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QPushButton>

#include "editorguiutils.h"

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::PreferencesDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);

    improveButtonBox(m_ui->buttonBox);
}

PreferencesDialog::~PreferencesDialog() = default;
