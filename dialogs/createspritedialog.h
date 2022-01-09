#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class CreateSpriteDialog; }

class CreateSpriteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateSpriteDialog(QWidget *parent = nullptr);
    explicit CreateSpriteDialog(const QSize &size, QWidget *parent = nullptr);
    ~CreateSpriteDialog();

    QSize size() const;

private:
    const std::unique_ptr<Ui::CreateSpriteDialog> m_ui;
};
