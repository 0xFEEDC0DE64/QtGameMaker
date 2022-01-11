#include "fontpropertiesdialog.h"
#include "ui_fontpropertiesdialog.h"

#include <QDebug>
#include <QMessageBox>

#include "projectcontainer.h"
#include "projecttreemodel.h"

FontPropertiesDialog::FontPropertiesDialog(Font &font, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::FontPropertiesDialog>()},
    m_font{font},
    m_projectModel{projectModel}
{
    m_ui->setupUi(this);

    updateTitle();

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    m_ui->lineEditName->setText(m_font.name);
    m_ui->fontComboBox->setCurrentFont(m_font.font);
    m_ui->spinBoxSize->setValue(m_font.font.pointSize());
    m_ui->checkBoxBold->setChecked(m_font.font.bold());
    m_ui->checkBoxItalic->setChecked(m_font.font.italic());
    m_ui->spinBoxCharRangeFrom->setValue(m_font.range.from);
    m_ui->spinBoxCharRangeTo->setValue(m_font.range.to);

    m_ui->labelPreview->setFont(currentFont());

    connect(&m_projectModel, &ProjectTreeModel::fontNameChanged,
            this, &FontPropertiesDialog::fontNameChanged);

    connect(m_ui->pushButtonNormal, &QAbstractButton::clicked,
            this, &FontPropertiesDialog::normalRange);
    connect(m_ui->pushButtonDigits, &QAbstractButton::clicked,
            this, &FontPropertiesDialog::digitsRange);
    connect(m_ui->pushButtonAll, &QAbstractButton::clicked,
            this, &FontPropertiesDialog::allRange);
    connect(m_ui->pushButtonLetters, &QAbstractButton::clicked,
            this, &FontPropertiesDialog::lettersRange);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &FontPropertiesDialog::changed);
    connect(m_ui->fontComboBox, &QFontComboBox::currentFontChanged,
            this, &FontPropertiesDialog::changed);
    connect(m_ui->spinBoxSize, &QSpinBox::valueChanged,
            this, &FontPropertiesDialog::changed);
    connect(m_ui->checkBoxBold, &QCheckBox::toggled,
            this, &FontPropertiesDialog::changed);
    connect(m_ui->checkBoxItalic, &QCheckBox::toggled,
            this, &FontPropertiesDialog::changed);
    connect(m_ui->spinBoxCharRangeFrom, &QSpinBox::valueChanged,
            this, &FontPropertiesDialog::changed);
    connect(m_ui->spinBoxCharRangeTo, &QSpinBox::valueChanged,
            this, &FontPropertiesDialog::changed);
}

FontPropertiesDialog::~FontPropertiesDialog() = default;

void FontPropertiesDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (m_font.name != m_ui->lineEditName->text())
    {
        if (!m_projectModel.rename<Font>(m_font, m_ui->lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Font failed!"), tr("Renaming Font failed!"));
            return;
        }
    }

    m_font.font = currentFont();
    m_font.range = {
        .from = m_ui->spinBoxCharRangeFrom->value(),
        .to =   m_ui->spinBoxCharRangeTo->value(),
    };

    QDialog::accept();
}

void FontPropertiesDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Font has been modified."),
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

void FontPropertiesDialog::normalRange()
{
    m_ui->spinBoxCharRangeFrom->setValue(32);
    m_ui->spinBoxCharRangeFrom->setValue(127);
}

void FontPropertiesDialog::digitsRange()
{
    m_ui->spinBoxCharRangeFrom->setValue(48);
    m_ui->spinBoxCharRangeFrom->setValue(57);
}

void FontPropertiesDialog::allRange()
{
    m_ui->spinBoxCharRangeFrom->setValue(0);
    m_ui->spinBoxCharRangeFrom->setValue(255);
}

void FontPropertiesDialog::lettersRange()
{
    m_ui->spinBoxCharRangeFrom->setValue(65);
    m_ui->spinBoxCharRangeFrom->setValue(122);
}

void FontPropertiesDialog::changed()
{
    m_ui->labelPreview->setFont(currentFont());
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void FontPropertiesDialog::fontNameChanged(const Font &font)
{
    if (&font != &m_font)
        return;

    {
        QSignalBlocker blocker{m_ui->lineEditName};
        m_ui->lineEditName->setText(font.name);
    }

    updateTitle();
}

void FontPropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Font Properties: %0%1")
                       .arg(m_font.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}

QFont FontPropertiesDialog::currentFont() const
{
    QFont font;
    font.setFamily(m_ui->fontComboBox->currentFont().family());
    font.setPointSize(m_ui->spinBoxSize->value());
    font.setBold(m_ui->checkBoxBold->isChecked());
    font.setItalic(m_ui->checkBoxItalic->isChecked());
    return font;
}
