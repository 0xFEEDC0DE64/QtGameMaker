#include "backgroundpropertiesdialog.h"
#include "ui_backgroundpropertiesdialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#include "projectcontainer.h"

BackgroundPropertiesDialog::BackgroundPropertiesDialog(Background &background, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::BackgroundPropertiesDialog>()},
    m_background{background}
{
    m_ui->setupUi(this);

    setWindowTitle(tr("Background Properties: %0").arg(m_background.name));

    m_ui->lineEditName->setText(m_background.name);
    m_ui->labelWidth->setText(tr("Width: %0").arg(m_background.pixmap.width()));
    m_ui->labelHeight->setText(tr("Height: %0").arg(m_background.pixmap.height()));
    m_ui->labelPreview->setPixmap(m_background.pixmap);

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
    if (m_background.name != m_ui->lineEditName->text())
    {
        QMessageBox::critical(this, tr("Not implemented"), tr("Changing the name is not yet implemented!"));
        return;
    }

    // TODO

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
    QFileDialog::getOpenFileName(this, tr("Open a Background Image..."));
}

void BackgroundPropertiesDialog::saveBackground()
{
    QFileDialog::getSaveFileName(this, tr("Save a Background Image..."), m_background.name + ".png", tr("PNG Files (*.png)"));
}

void BackgroundPropertiesDialog::editBackground()
{

}

void BackgroundPropertiesDialog::changed()
{
    if (!m_unsavedChanges)
    {
        setWindowTitle(tr("Background Properties: %0*").arg(m_background.name));
        m_unsavedChanges = true;
    }
}
