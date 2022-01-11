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
    explicit MainWindow(const QString &filePath, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void contextMenuRequested(const QPoint &pos);
    void doubleClicked(const QModelIndex &index);
    void selectionChanged(const QModelIndex &index);
    void modelErrorOccured(const QString &message);
    void changed();

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
    void findResource();
    void showObjectInformation();
private:
    template<typename T>
    void createFor();
private slots:
    void showGameInformation();
    void showGlobalGameSettings();
    void showExtensionPackages();
    void showDefineConstants();
    void showDefineTriggers();
    void showIncludedFiles();
    void about();

    void rowsInserted(const QModelIndex &parent, int first, int last);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void modelAboutToBeReset();

private:
    void loadFile(const QString &path);
    void updateTitle();

    template<typename T, typename ...Targs>
    void openOrActivateWindow(QMdiSubWindow * &ptr, Targs &&...args);

    template<typename T>
    std::map<T*, QMdiSubWindow*> &propertyWindowsFor();

    template<typename T>
    bool doubleClickedFor(const QModelIndex &index);

    template<typename T>
    bool rowsAboutToBeRemovedFor(const QModelIndex &parent, int first, int last);

    template<typename T>
    void modelAboutToBeResetFor();

    const std::unique_ptr<Ui::MainWindow> m_ui;

    ProjectContainer m_project;

    QString m_filePath;
    bool m_unsavedChanges{};

    const std::unique_ptr<ProjectTreeModel> m_projectTreeModel;

    std::map<Sprite*, QMdiSubWindow*> m_spritePropertiesWindows;
    std::map<Sound*, QMdiSubWindow*> m_soundPropertiesWindows;
    std::map<Background*, QMdiSubWindow*> m_backgroundPropertiesWindows;
    std::map<Path*, QMdiSubWindow*> m_pathPropertiesWindows;
    std::map<Script*, QMdiSubWindow*> m_scriptPropertiesWindows;
    std::map<Font*, QMdiSubWindow*> m_fontPropertiesWindows;
    std::map<TimeLine*, QMdiSubWindow*> m_timeLinePropertiesWindows;
    std::map<Object*, QMdiSubWindow*> m_objectPropertiesWindows;
    std::map<Room*, QMdiSubWindow*> m_roomPropertiesWindows;

    QMdiSubWindow *m_objectInformationWindow{};
    QMdiSubWindow *m_userDefinedConstantsWindow{};
    QMdiSubWindow *m_triggersWindow{};
    QMdiSubWindow *m_includedFilesWindow{};
};
