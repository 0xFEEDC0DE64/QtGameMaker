#include "soundpropertiesdialog.h"
#include "ui_soundpropertiesdialog.h"

#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QFileInfo>

#include "projectcontainer.h"
#include "models/projecttreemodel.h"
#include "mainwindow.h"

SoundPropertiesDialog::SoundPropertiesDialog(Sound &sound, ProjectTreeModel &projectModel, MainWindow &mainWindow) :
    QDialog{&mainWindow},
    m_ui{std::make_unique<Ui::SoundPropertiesDialog>()},
    m_sound{sound},
    m_projectModel{projectModel},
    m_path{m_sound.path}
{
    m_ui->setupUi(this);

    updateTitle();

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    m_ui->lineEditName->setText(m_sound.name);
    if (!m_sound.path.isEmpty())
    {
        m_ui->labelFilename->setText(tr("Filename: %0").arg(QFileInfo{m_sound.path}.fileName()));
        m_soundEffect.setSource(QUrl::fromLocalFile(m_sound.path));
    }
    m_ui->radioButtonNormal->setChecked(m_sound.type == Sound::Type::Sound);
    m_ui->radioButtonMusic->setChecked(m_sound.type == Sound::Type::Music);
    m_ui->radioButton3D->setChecked(false);
    m_ui->radioButtonMultimedia->setChecked(false);
    m_ui->checkBoxChorus->setChecked(m_sound.effects.chorus);
    m_ui->checkBoxFlanger->setChecked(m_sound.effects.flanger);
    m_ui->checkBoxGargle->setChecked(m_sound.effects.gargle);
    m_ui->checkBoxEcho->setChecked(m_sound.effects.echo);
    m_ui->checkBoxReverb->setChecked(m_sound.effects.reverb);
    m_ui->horizontalSliderVolume->setValue(m_sound.volume);
    m_ui->horizontalSliderPan->setValue(m_sound.pan);
    m_ui->checkBoxPreload->setChecked(m_sound.preload);

    connect(&m_projectModel, &ProjectTreeModel::soundNameChanged,
            this, &SoundPropertiesDialog::soundNameChanged);

    connect(m_ui->pushButtonLoad, &QAbstractButton::clicked,
            this, &SoundPropertiesDialog::loadSound);
    connect(m_ui->pushButtonPlay, &QAbstractButton::clicked,
            this, &SoundPropertiesDialog::playSound);
    connect(m_ui->pushButtonStop, &QAbstractButton::clicked,
            this, &SoundPropertiesDialog::stopSound);
    connect(m_ui->pushButtonSave, &QAbstractButton::clicked,
            this, &SoundPropertiesDialog::saveSound);
    connect(m_ui->pushButtonEdit, &QAbstractButton::clicked,
            this, &SoundPropertiesDialog::editSound);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->radioButtonNormal, &QRadioButton::toggled,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->radioButtonMusic, &QRadioButton::toggled,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->radioButton3D, &QRadioButton::toggled,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->radioButtonMultimedia, &QRadioButton::toggled,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->checkBoxChorus, &QCheckBox::checkStateChanged,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->checkBoxFlanger, &QCheckBox::checkStateChanged,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->checkBoxGargle, &QCheckBox::checkStateChanged,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->checkBoxEcho, &QCheckBox::checkStateChanged,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->checkBoxReverb, &QCheckBox::checkStateChanged,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->horizontalSliderVolume, &QSlider::valueChanged,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->horizontalSliderPan, &QSlider::valueChanged,
            this, &SoundPropertiesDialog::changed);
    connect(m_ui->checkBoxPreload, &QCheckBox::checkStateChanged,
            this, &SoundPropertiesDialog::changed);
}

SoundPropertiesDialog::~SoundPropertiesDialog() = default;

void SoundPropertiesDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (m_sound.name != m_ui->lineEditName->text())
    {
        if (!m_projectModel.rename<Sound>(m_sound, m_ui->lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Sound failed!"), tr("Renaming Sound failed!"));
            return;
        }
    }

    m_sound.path = std::move(m_path);

    if (m_ui->radioButtonNormal->isChecked())
        m_sound.type = Sound::Type::Sound;
    else if (m_ui->radioButtonMusic->isChecked())
        m_sound.type = Sound::Type::Music;
    else if (m_ui->radioButton3D->isChecked() ||
             m_ui->radioButtonMultimedia->isChecked())
    {
        QMessageBox::critical(this, tr("Not implemented"), tr("This kind of sound is not yet supported!"));
        return;
    }
    else
    {
        QMessageBox::critical(this, tr("No kind selected!"), tr("No kind selected!"));
        return;
    }

    m_sound.effects.chorus = m_ui->checkBoxChorus->isChecked();
    m_sound.effects.flanger = m_ui->checkBoxFlanger->isChecked();
    m_sound.effects.gargle = m_ui->checkBoxGargle->isChecked();
    m_sound.effects.echo = m_ui->checkBoxEcho->isChecked();
    m_sound.effects.reverb = m_ui->checkBoxReverb->isChecked();
    m_sound.volume = m_ui->horizontalSliderVolume->value();
    m_sound.pan = m_ui->horizontalSliderPan->value();

    QDialog::accept();
}

void SoundPropertiesDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Sound has been modified."),
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

void SoundPropertiesDialog::loadSound()
{
    auto path = QFileDialog::getOpenFileName(this, tr("Open a Sound File..."));
    if (path.isEmpty())
        return;

    if (QFile file{path}; !file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("Could not load Sound!"), tr("Could not load Sound!") + "\n\n" + file.errorString());
        return;
    }

    m_path = std::move(path);
    changed();
    m_ui->labelFilename->setText(tr("Filename: %0").arg(QFileInfo{m_path}.fileName()));
    m_soundEffect.setSource(QUrl::fromLocalFile(m_path));
}

void SoundPropertiesDialog::saveSound()
{
    if (m_path.isEmpty())
    {
        QMessageBox::warning(this, tr("Could not save Sound!"), tr("Could not save Sound!") + "\n\n" + tr("No sound has been selected yet."));
        return;
    }

    const auto savePath = QFileDialog::getSaveFileName(this, tr("Save a Sound File..."), m_sound.name + ".wav", tr("WAV Files (*.wav)"));
    if (savePath.isEmpty())
        return;

    if (!QFile::copy(m_path, savePath))
    {
        QMessageBox::warning(this, tr("Could not save Sound!"), tr("Could not save Sound!"));
        return;
    }
}

void SoundPropertiesDialog::playSound()
{
    m_soundEffect.play();
}

void SoundPropertiesDialog::stopSound()
{
    m_soundEffect.stop();
}

void SoundPropertiesDialog::editSound()
{
    QMessageBox::critical(this, tr("Setup not complete"), tr("No valid external editor has been indicated for this type of sound. You can specify this editor in the Preferences."));
}

void SoundPropertiesDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void SoundPropertiesDialog::soundNameChanged(const Sound &sound)
{
    if (&sound != &m_sound)
        return;

    {
        QSignalBlocker blocker{m_ui->lineEditName};
        m_ui->lineEditName->setText(sound.name);
    }

    updateTitle();
}

void SoundPropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Sound Properties: %0%1")
                       .arg(m_sound.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}
