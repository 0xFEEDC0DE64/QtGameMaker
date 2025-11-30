#pragma once

#include <QDialog>
#include <QPixmap>

#include <memory>
#include <vector>

namespace Ui { class EditSpriteDialog; }
class EditorSettings;
struct Sprite;
class SpritesModel;

class EditSpriteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditSpriteDialog(const std::vector<QPixmap> &pixmaps, const QString &spriteName,
                              EditorSettings &settings, QWidget *parent = nullptr);
    ~EditSpriteDialog();

    void accept() override;
    void reject() override;

    const std::vector<QPixmap> &pixmaps() const;

private slots:
    void newSprite();
    void createFromFile();
    void addFromFile();
    void saveAsPng();
    void createFromStrip();
    void addFromStrip();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void erase();
    void delete_();
    void moveLeft();
    void moveRight();
    void addEmpty();
    void insertEmpty();
    void edit();
    void transparentBackgroundSettings();

    void activated(const QModelIndex &index);
    void currentChanged();

    void changed();

private:
    void updateTitle();

    const std::unique_ptr<Ui::EditSpriteDialog> m_ui;

    EditorSettings &m_settings;

    const QString m_spriteName;

    bool m_unsavedChanges{};

    const std::unique_ptr<SpritesModel> m_model;
};
