#include "spritepropertiesdialog.h"
#include "ui_spritepropertiesdialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#include "projectcontainer.h"
#include "editspritedialog.h"

SpritePropertiesDialog::SpritePropertiesDialog(Sprite &sprite, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::SpritePropertiesDialog>()},
    m_sprite{sprite}
{
    m_ui->setupUi(this);

    setWindowTitle(tr("Sprite Properties: %0").arg(m_sprite.name));

    m_ui->lineEditName->setText(m_sprite.name);
    m_ui->labelWidth->setText(tr("Width: %0").arg(m_sprite.pixmaps.empty() ? tr("n/a") : QString::number(m_sprite.pixmaps.front().width())));
    m_ui->labelHeight->setText(tr("Height: %0").arg(m_sprite.pixmaps.empty() ? tr("n/a") : QString::number(m_sprite.pixmaps.front().height())));
    m_ui->labelSubimages->setText(tr("Number of subimages: %0").arg(m_sprite.pixmaps.size()));
    m_ui->spinBoxOriginX->setValue(m_sprite.origin.x);
    m_ui->spinBoxOriginY->setValue(m_sprite.origin.y);
    m_ui->checkBoxPreciseCollisionChecking->setChecked(m_sprite.preciseCollisionChecking);
    m_ui->checkBoxSeparateCollisionMasks->setChecked(m_sprite.separateCollisionMasks);
    m_ui->labelPreview->setPixmap(m_sprite.pixmaps.empty() ? QPixmap{} : m_sprite.pixmaps.front());

    connect(m_ui->pushButtonLoad, &QAbstractButton::pressed,
            this, &SpritePropertiesDialog::loadSprite);
    connect(m_ui->pushButtonSave, &QAbstractButton::pressed,
            this, &SpritePropertiesDialog::saveSprite);
    connect(m_ui->pushButtonEdit, &QAbstractButton::pressed,
            this, &SpritePropertiesDialog::editSprite);
    connect(m_ui->pushButtonCenterOrigin, &QAbstractButton::pressed,
            this, &SpritePropertiesDialog::centerOrigin);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &SpritePropertiesDialog::changed);
    connect(m_ui->spinBoxOriginX, &QSpinBox::valueChanged,
            this, &SpritePropertiesDialog::changed);
    connect(m_ui->spinBoxOriginY, &QSpinBox::valueChanged,
            this, &SpritePropertiesDialog::changed);
    connect(m_ui->checkBoxPreciseCollisionChecking, &QCheckBox::stateChanged,
            this, &SpritePropertiesDialog::changed);
    connect(m_ui->checkBoxSeparateCollisionMasks, &QCheckBox::stateChanged,
            this, &SpritePropertiesDialog::changed);
}

SpritePropertiesDialog::~SpritePropertiesDialog() = default;

void SpritePropertiesDialog::accept()
{
    if (m_sprite.name != m_ui->lineEditName->text())
    {
        QMessageBox::critical(this, tr("Not implemented"), tr("Changing the name is not yet implemented!"));
        return;
    }

    m_sprite.origin.x = m_ui->spinBoxOriginX->value();
    m_sprite.origin.y = m_ui->spinBoxOriginY->value();
    m_sprite.preciseCollisionChecking = m_ui->checkBoxPreciseCollisionChecking->isChecked();
    m_sprite.separateCollisionMasks = m_ui->checkBoxSeparateCollisionMasks->isChecked();

    QDialog::accept();
}

void SpritePropertiesDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Sprite has been modified."),
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

void SpritePropertiesDialog::loadSprite()
{
    QFileDialog::getOpenFileName(this, tr("Open a Sprite Image..."));
}

void SpritePropertiesDialog::saveSprite()
{
    QFileDialog::getSaveFileName(this, tr("Save a Sprite Image..."), m_sprite.name + ".png", tr("PNG Files (*.png)"));
}

void SpritePropertiesDialog::editSprite()
{
    EditSpriteDialog{m_sprite}.exec();
}

void SpritePropertiesDialog::centerOrigin()
{
    if (m_sprite.pixmaps.empty())
    {
        qDebug() << "unexpected empty pixmaps";
        return;
    }

    m_ui->spinBoxOriginX->setValue(m_sprite.pixmaps.front().width() / 2);
    m_ui->spinBoxOriginY->setValue(m_sprite.pixmaps.front().height() / 2);
}

void SpritePropertiesDialog::changed()
{
    if (!m_unsavedChanges)
    {
        setWindowTitle(tr("Sprite Properties: %0*").arg(m_sprite.name));
        m_unsavedChanges = true;
    }
}
