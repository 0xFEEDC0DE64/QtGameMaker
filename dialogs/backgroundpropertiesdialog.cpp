#include "backgroundpropertiesdialog.h"
#include "ui_backgroundpropertiesdialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#include "projectcontainer.h"
#include "projecttreemodel.h"
#include "imageeditordialog.h"

BackgroundPropertiesDialog::BackgroundPropertiesDialog(Background &background, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::BackgroundPropertiesDialog>()},
    m_background{background},
    m_projectModel{projectModel},
    m_pixmap{m_background.pixmap}
{
    m_ui->setupUi(this);

    updateTitle();

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    m_ui->lineEditName->setText(m_background.name);
    updateSpriteInfo();
    m_ui->checkBoxTileset->setChecked(m_background.tileset);

    connect(&m_projectModel, &ProjectTreeModel::backgroundNameChanged,
            this, &BackgroundPropertiesDialog::backgroundNameChanged);

    connect(m_ui->pushButtonLoad, &QAbstractButton::pressed,
            this, &BackgroundPropertiesDialog::loadBackground);
    connect(m_ui->pushButtonSave, &QAbstractButton::pressed,
            this, &BackgroundPropertiesDialog::saveBackground);
    connect(m_ui->pushButtonEdit, &QAbstractButton::pressed,
            this, &BackgroundPropertiesDialog::editBackground);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &BackgroundPropertiesDialog::changed);
    connect(m_ui->checkBoxTileset, &QCheckBox::stateChanged,
            this, &BackgroundPropertiesDialog::changed);
}

BackgroundPropertiesDialog::~BackgroundPropertiesDialog() = default;

void BackgroundPropertiesDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (m_background.name != m_ui->lineEditName->text())
    {
        if (!m_projectModel.rename<Background>(m_background, m_ui->lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Background failed!"), tr("Renaming Background failed!"));
            return;
        }
    }

    m_background.pixmap = std::move(m_pixmap);
    m_background.tileset = m_ui->checkBoxTileset->isChecked();

    QDialog::accept();
}

void BackgroundPropertiesDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Background has been modified."),
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

void BackgroundPropertiesDialog::loadBackground()
{
    const auto path = QFileDialog::getOpenFileName(this, tr("Open a Background Image..."), {}, tr("BMP Files (*.bmp), PNG Files (*png)"));
    if (path.isEmpty())
        return;

    QPixmap pixmap;
    if (!pixmap.load(path))
    {
        QMessageBox::warning(this, tr("Could not load sprite!"), tr("Could not load sprite!"));
        return;
    }

    m_pixmap = std::move(pixmap);
    changed();
    updateSpriteInfo();
}

void BackgroundPropertiesDialog::saveBackground()
{
    if (m_pixmap.isNull())
    {
        QMessageBox::warning(this, tr("No background available to save!"), tr("No background available to save!"));
        return;
    }

    const auto path = QFileDialog::getSaveFileName(this, tr("Save a Background Image..."), m_background.name + ".png", tr("PNG Files (*.png)"));
    if (path.isEmpty())
        return;

    if (!m_pixmap.save(path))
    {
        QMessageBox::warning(this, tr("Could not save Background!"), tr("Could not save Background!"));
        return;
    }
}

void BackgroundPropertiesDialog::editBackground()
{
    ImageEditorDialog dialog{m_pixmap, tr("Image Editor: %0").arg(m_background.name), this};
    if (dialog.exec() == QDialog::Accepted)
    {
        m_pixmap = dialog.pixmap();
        changed();
        updateSpriteInfo();
    }
}

void BackgroundPropertiesDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void BackgroundPropertiesDialog::backgroundNameChanged(const Background &background)
{
    if (&background != &m_background)
        return;

    {
        QSignalBlocker blocker{m_ui->lineEditName};
        m_ui->lineEditName->setText(background.name);
    }

    updateTitle();
}

void BackgroundPropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Background Properties: %0%1")
                       .arg(m_background.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}

void BackgroundPropertiesDialog::updateSpriteInfo()
{
    m_ui->labelWidth->setText(tr("Width: %0").arg(m_pixmap.width()));
    m_ui->labelHeight->setText(tr("Height: %0").arg(m_pixmap.height()));
    m_ui->labelPreview->setPixmap(m_pixmap);
}
