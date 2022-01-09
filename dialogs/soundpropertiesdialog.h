#pragma once

#include <QDialog>
#include <QString>
#include <QSoundEffect>

#include <memory>
#include <optional>

namespace Ui { class SoundPropertiesDialog; }
struct Sound;

class SoundPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SoundPropertiesDialog(Sound &sound, QWidget *parent = nullptr);
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

private:
    const std::unique_ptr<Ui::SoundPropertiesDialog> m_ui;

    Sound &m_sound;

    bool m_unsavedChanges{};

    std::optional<QString> m_newPath;

    QSoundEffect m_soundEffect;
};
