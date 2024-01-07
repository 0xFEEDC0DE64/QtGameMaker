#pragma once

#include <QDialog>
#include <QString>
#include <QSoundEffect>

#include <memory>

namespace Ui { class SoundPropertiesDialog; }
struct Sound;
class ProjectTreeModel;
class MainWindow;

class SoundPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SoundPropertiesDialog(Sound &sound, ProjectTreeModel &projectModel, MainWindow &mainWindow);
    ~SoundPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void loadSound();
    void saveSound();
    void playSound();
    void stopSound();
    void editSound();

    void changed();

    void soundNameChanged(const Sound &sound);

private:
    void updateTitle();

    const std::unique_ptr<Ui::SoundPropertiesDialog> m_ui;

    Sound &m_sound;
    ProjectTreeModel &m_projectModel;

    bool m_unsavedChanges{};

    QString m_path;

    QSoundEffect m_soundEffect;
};
