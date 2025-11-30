#include "transparentbackgroundsettingsdialog.h"
#include "ui_transparentbackgroundsettingsdialog.h"

#include <QMessageBox>

#include "editorguiutils.h"
#include "editorsettings.h"

TransparentBackgroundSettingsDialog::TransparentBackgroundSettingsDialog(const EditorSettings &settings, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::TransparentBackgroundSettingsDialog>()}
{
    m_ui->setupUi(this);

    improveButtonBox(m_ui->buttonBox);

    const auto type = settings.transparentBackgroundPatternType();
    switch (type)
    {
    case TransparentBackgroundPattern::Type::BlockPattern:
        m_ui->radioButtonBlockPattern->setChecked(true);
        goto after;
    case TransparentBackgroundPattern::Type::SingleColor:
        m_ui->radioButtonSingleColor->setChecked(true);
        goto after;
    }
    qWarning() << "unknown TransparentBackgroundPattern::Type" << std::to_underlying(type);
after:

    m_ui->pushButtonColor1->setColor(settings.transparentBackgroundBlockPatternColor1());
    m_ui->pushButtonColor2->setColor(settings.transparentBackgroundBlockPatternColor2());
    m_ui->spinBoxBlockSize->setValue(settings.transparentBackgroundBlockPatternBlockSize());
    m_ui->pushButtonColor->setColor(settings.transparentBackgroundSingleColor());
}

TransparentBackgroundSettingsDialog::~TransparentBackgroundSettingsDialog() = default;

void TransparentBackgroundSettingsDialog::save(EditorSettings &settings)
{
    if (m_ui->radioButtonBlockPattern->isChecked())
        settings.setTransparentBackgroundPatternType(TransparentBackgroundPattern::Type::BlockPattern);
    else if (m_ui->radioButtonSingleColor->isChecked())
        settings.setTransparentBackgroundPatternType(TransparentBackgroundPattern::Type::SingleColor);
    else
        qWarning() << "no type is selected?!";

    settings.setTransparentBackgroundBlockPatternColor1(m_ui->pushButtonColor1->color());
    settings.setTransparentBackgroundBlockPatternColor2(m_ui->pushButtonColor2->color());
    settings.setTransparentBackgroundBlockPatternBlockSize(m_ui->spinBoxBlockSize->value());
    settings.setTransparentBackgroundSingleColor(m_ui->pushButtonColor->color());
}

void TransparentBackgroundSettingsDialog::accept()
{
    if (!m_ui->radioButtonBlockPattern->isChecked() &&
        !m_ui->radioButtonSingleColor->isChecked())
    {
        QMessageBox::warning(this, tr("No pattern type selected!"), tr("No pattern type selected!"));
        return;
    }

    if (m_ui->spinBoxBlockSize->value() < 1)
    {
        QMessageBox::warning(this, tr("Invalid block size selected!"), tr("Invalid block size selected!"));
        return;
    }

    QDialog::accept();
}
