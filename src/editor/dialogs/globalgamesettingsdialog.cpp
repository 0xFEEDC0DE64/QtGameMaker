#include "globalgamesettingsdialog.h"
#include "ui_globalgamesettingsdialog.h"

#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

#include "editorguiutils.h"

GlobalGameSettingsDialog::GlobalGameSettingsDialog(GlobalGameSettings &globalGameSettings, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::GlobalGameSettingsDialog>()},
    m_globalGameSettings{globalGameSettings}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);

    updateTitle();

    QWidget* widgets[]{ m_ui->widgetResolution, m_ui->widgetCustomProgressBar };
    for (QWidget *widget : widgets)
    {
        QSizePolicy sp_retain = widget->sizePolicy();
        sp_retain.setRetainSizeWhenHidden(true);
        widget->setSizePolicy(sp_retain);
    }

    improveButtonBox(m_ui->buttonBox);

    connect(m_ui->checkBoxFullscreen, &QCheckBox::toggled,
            this, &GlobalGameSettingsDialog::changed);
    connect(m_ui->radioButtonFixedScale, &QCheckBox::toggled,
            this, &GlobalGameSettingsDialog::changed);
    connect(m_ui->radioButtonKeepAspectRatio, &QRadioButton::toggled,
            this, &GlobalGameSettingsDialog::changed);
    connect(m_ui->radioButtonFullScale, &QRadioButton::toggled,
            this, &GlobalGameSettingsDialog::changed);
    connect(m_ui->checkBoxInterpolateColors, &QRadioButton::toggled,
            this, &GlobalGameSettingsDialog::changed);
    // TODO toolButtonColorOutside
    connect(m_ui->checkBoxAllowResize, &QCheckBox::toggled,
            this, &GlobalGameSettingsDialog::changed);
    connect(m_ui->checkBoxAlwaysOntop, &QCheckBox::toggled,
            this, &GlobalGameSettingsDialog::changed);
    connect(m_ui->checkBoxNoBorder, &QCheckBox::toggled,
            this, &GlobalGameSettingsDialog::changed);
    connect(m_ui->checkBoxNoWindowButtons, &QCheckBox::toggled,
            this, &GlobalGameSettingsDialog::changed);
    connect(m_ui->checkBoxDisplayCursor, &QCheckBox::toggled,
            this, &GlobalGameSettingsDialog::changed);
    connect(m_ui->checkBoxFreezeOnFocusLost, &QCheckBox::toggled,
            this, &GlobalGameSettingsDialog::changed);
    connect(m_ui->checkBoxDisableScreensavers, &QCheckBox::toggled,
            this, &GlobalGameSettingsDialog::changed);
}

GlobalGameSettingsDialog::~GlobalGameSettingsDialog() = default;

void GlobalGameSettingsDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    // TODO

    QDialog::accept();
}

void GlobalGameSettingsDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Global Game Settings has been modified."),
        tr("Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save
    );
    switch (result)
    {
    case QMessageBox::Save:
        accept();
        return;
    case QMessageBox::Discard:
        QDialog::reject();
        return;
    case QMessageBox::Cancel:
        return;
    default:
        qWarning() << "unexpected dialog result" << result;
    }
}

void GlobalGameSettingsDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void GlobalGameSettingsDialog::updateTitle()
{
    setWindowTitle(tr("Global Game Settings%0")
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}
