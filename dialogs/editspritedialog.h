#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class EditSpriteDialog; }
struct Sprite;

class EditSpriteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditSpriteDialog(Sprite &sprite, QWidget *parent = nullptr);
    ~EditSpriteDialog();

private:
    const std::unique_ptr<Ui::EditSpriteDialog> m_ui;

    Sprite &m_sprite;
};