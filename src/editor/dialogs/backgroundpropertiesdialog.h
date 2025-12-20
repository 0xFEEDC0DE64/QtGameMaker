#pragma once

#include <QDialog>
#include <QString>

#include <memory>

namespace Ui { class BackgroundPropertiesDialog; }
struct Background;
class ProjectTreeModel;
class MainWindow;

class BackgroundPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BackgroundPropertiesDialog(Background &background, ProjectTreeModel &projectModel, MainWindow &mainWindow);
    ~BackgroundPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void loadBackground();
    void saveBackground();
    void editBackground();

    void changed();

    void backgroundNameChanged(const Background &background);

    void advancedModeChanged(bool advancedMode);

private:
    void updateTitle();
    void updateSpriteInfo();

    const std::unique_ptr<Ui::BackgroundPropertiesDialog> m_ui;

    Background &m_background;
    ProjectTreeModel &m_projectModel;
    MainWindow &m_mainWindow;

    bool m_unsavedChanges{};

    QPixmap m_pixmap;
};
