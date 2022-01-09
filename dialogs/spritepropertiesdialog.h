#pragma once

#include <QDialog>
#include <QString>

#include <memory>
#include <optional>

namespace Ui { class SpritePropertiesDialog; }
struct Sprite;

class SpritePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpritePropertiesDialog(Sprite &sprite, QWidget *parent = nullptr);
    ~SpritePropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void loadSprite();
    void saveSprite();
    void editSprite();
    void centerOrigin();

    void changed();

private:
    void updateTitle();

    const std::unique_ptr<Ui::SpritePropertiesDialog> m_ui;

    Sprite &m_sprite;

    bool m_unsavedChanges{};

    std::optional<std::vector<QPixmap>> m_newPixmaps;
};
