#pragma once

#include <QMainWindow>

#include <memory>
#include <map>

#include "projectcontainer.h"

class QMdiSubWindow;
class QActionGroup;
namespace Ui { class MainWindow; }
class EditorSettings;
class ProjectTreeModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &filePath, EditorSettings &settings, QWidget *parent = nullptr);
    ~MainWindow();

    EditorSettings &settings() { return m_settings; }
    const EditorSettings &settings() const { return m_settings; }

private:
    template<typename T>
    std::map<T*, QMdiSubWindow*> &propertyWindowsFor();

public:
    template<typename T>
    void openPropertiesWindowFor(T &entry);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void contextMenuRequested(const QPoint &pos);
    void activated(const QModelIndex &index);
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
    void transparentBackgroundSettings();
private:
    template<typename T>
    void createFor();
private slots:
    void showGameInformation();
    void showGlobalGameSettings();
    void showExtensionPackages();
    void showDefineConstants();
public slots:
    void showDefineTriggers();
private slots:
    void showIncludedFiles();
    void runGame();
    void debugGame();
    void about();

    void rowsInserted(const QModelIndex &parent, int first, int last);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void modelAboutToBeReset();

private:
    void setupStylesMenu();
    void loadFile(const QString &path);
    void updateTitle();
    void updateVisibilities();

    QMdiSubWindow *addSubWindow(QDialog *dialog);

    template<typename T, typename ...Targs>
    void openOrActivateWindow(QMdiSubWindow * &ptr, Targs &&...args);

    template<typename T>
    bool activatedFor(const QModelIndex &index);

    template<typename T>
    bool rowsAboutToBeRemovedFor(const QModelIndex &parent, int first, int last);

    template<typename T>
    void modelAboutToBeResetFor();

    const std::unique_ptr<Ui::MainWindow> m_ui;

    EditorSettings &m_settings;

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

    QActionGroup *m_actionGroupViewMode{};
    QActionGroup *m_actionGroupStyles{};
    QActionGroup *m_actionGroupWindows{};
};
