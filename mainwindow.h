#pragma once

#include <QMainWindow>

#include <memory>
#include <map>

#include "projectcontainer.h"

class QMdiSubWindow;
namespace Ui { class MainWindow; }
class ProjectTreeModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void contextMenuRequested(const QPoint &pos);
    void doubleClicked(const QModelIndex &index);
    void selectionChanged(const QModelIndex &index);

    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void createExecutable();
    void publishGame();
    void importResources();
    void exportResources();
    void preferences();
    void create();
    void duplicate();
    void createGroup();
    void sortByName();
    void delete_();
    void rename();
    void showProperties();
    void createSprite();
    void createSound();
    void createBackground();
    void createPath();
    void createScript();
    void createFont();
    void showGameInformation();
    void showGlobalGameSettings();
    void showExtensionPackages();
    void about();

    void rowsInserted(const QModelIndex &parent, int first, int last);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void modelAboutToBeReset();

private:
    void updateTitle();

    const std::unique_ptr<Ui::MainWindow> m_ui;

    ProjectContainer m_project;

    QString m_filePath;
    bool m_unsavedChanges{};

    const std::unique_ptr<ProjectTreeModel> m_projectTreeModel;

    std::map<Sprite*, QMdiSubWindow*> m_spritePropertiesDialogs;
    std::map<Sound*, QMdiSubWindow*> m_soundPropertiesDialogs;
    std::map<Background*, QMdiSubWindow*> m_backgroundPropertiesDialogs;
    std::map<Path*, QMdiSubWindow*> m_pathPropertiesDialogs;
    std::map<Script*, QMdiSubWindow*> m_scriptPropertiesDialogs;
    std::map<Font*, QMdiSubWindow*> m_fontPropertiesDialogs;
};
