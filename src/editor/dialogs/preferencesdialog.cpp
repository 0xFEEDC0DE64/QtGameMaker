#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QMessageBox>

#include "editorguiutils.h"
#include "editorsettings.h"

PreferencesDialog::PreferencesDialog(const EditorSettings &settings, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::PreferencesDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);

    improveButtonBox(m_ui->buttonBox);

    m_ui->checkBoxShowRecentFiles->setChecked(settings.showRecentFiles());
    m_ui->checkBoxHideWebsiteLogo->setChecked(settings.hideWebsiteImage());
}

PreferencesDialog::~PreferencesDialog() = default;

void PreferencesDialog::save(EditorSettings &settings)
{
    settings.setShowRecentFiles(m_ui->checkBoxShowRecentFiles->isChecked());
    settings.setHideWebsiteImage(m_ui->checkBoxHideWebsiteLogo->isChecked());
}
