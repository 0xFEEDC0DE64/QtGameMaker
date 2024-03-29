#pragma once

#include <QDialog>
#include <QString>

#include <memory>

namespace Ui { class SpritePropertiesDialog; }
struct Sprite;
class ProjectTreeModel;
class MainWindow;

class SpritePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpritePropertiesDialog(Sprite &sprite, ProjectTreeModel &projectModel, MainWindow &mainWindow);
    ~SpritePropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void loadSprite();
    void saveSprite();
    void editSprite();
    void centerOrigin();
    void modifyMask();

    void changed();

    void spriteNameChanged(const Sprite &sprite);

private:
    void updateTitle();
    void updateSpriteInfo();

    const std::unique_ptr<Ui::SpritePropertiesDialog> m_ui;

    Sprite &m_sprite;
    ProjectTreeModel &m_projectModel;

    bool m_unsavedChanges{};

    std::vector<QPixmap> m_pixmaps;
};
