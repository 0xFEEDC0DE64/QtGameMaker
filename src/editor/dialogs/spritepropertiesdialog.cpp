#include "spritepropertiesdialog.h"
#include "ui_spritepropertiesdialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QSignalBlocker>

#include "projectcontainer.h"
#include "models/projecttreemodel.h"
#include "editspritedialog.h"
#include "maskpropertiesdialog.h"
#include "mainwindow.h"
#include "imagehelpers.h"
#include "editorguiutils.h"

SpritePropertiesDialog::SpritePropertiesDialog(Sprite &sprite, ProjectTreeModel &projectModel, MainWindow &mainWindow) :
    QDialog{&mainWindow},
    m_ui{std::make_unique<Ui::SpritePropertiesDialog>()},
    m_sprite{sprite},
    m_projectModel{projectModel},
    m_mainWindow{mainWindow},
    m_pixmaps{m_sprite.pixmaps}
{
    m_ui->setupUi(this);

    updateTitle();

    improveButtonBox(m_ui->buttonBox);

    m_ui->lineEditName->setText(m_sprite.name);
    updateSpriteInfo();
    m_ui->spinBoxOriginX->setValue(m_sprite.origin.x);
    m_ui->spinBoxOriginY->setValue(m_sprite.origin.y);
    m_ui->checkBoxPreciseCollisionChecking->setChecked(m_sprite.preciseCollisionChecking);
    m_ui->checkBoxSeparateCollisionMasks->setChecked(m_sprite.separateCollisionMasks);

    connect(&m_projectModel, &ProjectTreeModel::spriteNameChanged,
            this, &SpritePropertiesDialog::spriteNameChanged);

    connect(m_ui->pushButtonLoad, &QAbstractButton::clicked,
            this, &SpritePropertiesDialog::loadSprite);
    connect(m_ui->pushButtonSave, &QAbstractButton::clicked,
            this, &SpritePropertiesDialog::saveSprite);
    connect(m_ui->pushButtonEdit, &QAbstractButton::clicked,
            this, &SpritePropertiesDialog::editSprite);
    connect(m_ui->pushButtonCenterOrigin, &QAbstractButton::clicked,
            this, &SpritePropertiesDialog::centerOrigin);
    connect(m_ui->pushButtonModifyCollisionmask, &QAbstractButton::clicked,
            this, &SpritePropertiesDialog::modifyMask);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &SpritePropertiesDialog::changed);
    connect(m_ui->spinBoxOriginX, &QSpinBox::valueChanged,
            this, &SpritePropertiesDialog::changed);
    connect(m_ui->spinBoxOriginY, &QSpinBox::valueChanged,
            this, &SpritePropertiesDialog::changed);
    connect(m_ui->checkBoxPreciseCollisionChecking, &QCheckBox::checkStateChanged,
            this, &SpritePropertiesDialog::changed);
    connect(m_ui->checkBoxSeparateCollisionMasks, &QCheckBox::checkStateChanged,
            this, &SpritePropertiesDialog::changed);
}

SpritePropertiesDialog::~SpritePropertiesDialog() = default;

void SpritePropertiesDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (m_sprite.name != m_ui->lineEditName->text())
    {
        if (!m_projectModel.rename<Sprite>(m_sprite, m_ui->lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Sprite failed!"), tr("Renaming Sprite failed!"));
            return;
        }
    }

    if (!m_projectModel.setSpritePixmaps(m_sprite, std::move(m_pixmaps)))
    {
        QMessageBox::critical(this, tr("Setting Sprite pixmaps failed!"), tr("Setting Sprite pixmaps failed!"));
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
    auto pixmap = loadPixmap(this);
    if (pixmap.isNull())
        return;

    m_pixmaps = std::vector<QPixmap>{ std::move(pixmap) };
    changed();
    updateSpriteInfo();
}

void SpritePropertiesDialog::saveSprite()
{
    if (m_pixmaps.empty() || m_pixmaps.front().isNull())
    {
        QMessageBox::warning(this, tr("No sprites available to save!"), tr("No sprites available to save!"));
        return;
    }

    saveImage(this, m_pixmaps.front().toImage());
}

void SpritePropertiesDialog::editSprite()
{
    EditSpriteDialog dialog {
        m_pixmaps,
        m_sprite.name,
        m_mainWindow.settings(),
        this
    };
    if (dialog.exec() == QDialog::Accepted)
    {
        m_pixmaps = dialog.pixmaps();
        changed();
        updateSpriteInfo();
    }
}

void SpritePropertiesDialog::centerOrigin()
{
    QPoint center;
    if (m_pixmaps.empty() || m_pixmaps.front().isNull())
        qDebug() << "unexpected empty pixmaps";
    else
        center = {
            m_pixmaps.front().width() / 2,
            m_pixmaps.front().height() / 2
        };

    m_ui->spinBoxOriginX->setValue(center.x());
    m_ui->spinBoxOriginY->setValue(center.y());
}

void SpritePropertiesDialog::modifyMask()
{
    MaskPropertiesDialog dialog{this};
    dialog.exec();
}

void SpritePropertiesDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void SpritePropertiesDialog::spriteNameChanged(const Sprite &sprite)
{
    if (&sprite != &m_sprite)
        return;

    {
        QSignalBlocker blocker{m_ui->lineEditName};
        m_ui->lineEditName->setText(sprite.name);
    }

    updateTitle();
}

void SpritePropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Sprite Properties: %0%1")
                       .arg(m_sprite.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}

void SpritePropertiesDialog::updateSpriteInfo()
{
    m_ui->labelPreview->setPixmap(m_pixmaps.empty() ? QPixmap{} : m_pixmaps.front());
    m_ui->labelWidth->setText(tr("Width: %0").arg(m_pixmaps.empty() ? tr("n/a") : QString::number(m_pixmaps.front().width())));
    m_ui->labelHeight->setText(tr("Height: %0").arg(m_pixmaps.empty() ? tr("n/a") : QString::number(m_pixmaps.front().height())));
    m_ui->labelSubimages->setText(tr("Number of subimages: %0").arg(m_pixmaps.size()));
}
