#pragma once

#include <QDialog>
#include <QPixmap>

#include <memory>
#include <vector>

namespace Ui { class EditSpriteDialog; }
struct Sprite;
class SpritesModel;

class EditSpriteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditSpriteDialog(const std::vector<QPixmap> &pixmaps, const QString &spriteName, QWidget *parent = nullptr);
    ~EditSpriteDialog();

    void accept() override;
    void reject() override;

    const std::vector<QPixmap> &pixmaps() const { return m_pixmaps; }

private slots:
    void newSprite();
    void activated(const QModelIndex &index);

    void changed();

private:
    void updateTitle();

    const std::unique_ptr<Ui::EditSpriteDialog> m_ui;

    std::vector<QPixmap> m_pixmaps;
    const QString m_spriteName;

    bool m_unsavedChanges{};

    const std::unique_ptr<SpritesModel> m_model;
};
