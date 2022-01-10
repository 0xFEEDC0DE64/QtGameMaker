#pragma once

#include <QDialog>
#include <QString>

#include <memory>
#include <optional>

namespace Ui { class BackgroundPropertiesDialog; }
struct Background;
class ProjectTreeModel;

class BackgroundPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BackgroundPropertiesDialog(Background &background, ProjectTreeModel &projectModel, QWidget *parent = nullptr);
    ~BackgroundPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void loadBackground();
    void saveBackground();
    void editBackground();

    void changed();

    void backgroundNameChanged(const Background &background);

private:
    void updateTitle();
    void updateSpriteInfo();

    const std::unique_ptr<Ui::BackgroundPropertiesDialog> m_ui;

    Background &m_background;
    ProjectTreeModel &m_projectModel;

    bool m_unsavedChanges{};

    QPixmap m_pixmap;
};
