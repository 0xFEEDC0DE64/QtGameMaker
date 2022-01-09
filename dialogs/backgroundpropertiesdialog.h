#pragma once

#include <QDialog>
#include <QString>

#include <memory>

namespace Ui { class BackgroundPropertiesDialog; }
struct Background;

class BackgroundPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BackgroundPropertiesDialog(Background &background, QWidget *parent = nullptr);
    ~BackgroundPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void loadBackground();
    void saveBackground();
    void editBackground();

    void changed();

private:
    const std::unique_ptr<Ui::BackgroundPropertiesDialog> m_ui;

    Background &m_background;

    bool m_unsavedChanges{};
};
