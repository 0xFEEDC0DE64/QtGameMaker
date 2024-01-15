#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QActionGroup>
#include <QStyleFactory>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QDebug>

#include "projectserialization.h"
#include "models/projecttreemodel.h"
#include "dialogs/preferencesdialog.h"
#include "dialogs/spritepropertiesdialog.h"
#include "dialogs/soundpropertiesdialog.h"
#include "dialogs/backgroundpropertiesdialog.h"
#include "dialogs/pathpropertiesdialog.h"
#include "dialogs/scriptpropertiesdialog.h"
#include "dialogs/fontpropertiesdialog.h"
#include "dialogs/timelinepropertiesdialog.h"
#include "dialogs/objectpropertiesdialog.h"
#include "dialogs/roompropertiesdialog.h"
#include "dialogs/objectinformationdialog.h"
#include "dialogs/transparentbackgroundsettingsdialog.h"
#include "dialogs/gameinformationdialog.h"
#include "dialogs/globalgamesettingsdialog.h"
#include "dialogs/extensionpackagesdialog.h"
#include "dialogs/userdefinedconstantsdialog.h"
#include "dialogs/triggersdialog.h"
#include "dialogs/includedfilesdialog.h"
#include "engine/gameengine.h"

namespace {
template<typename T> struct PropertiesDialogForDetail;
template<> struct PropertiesDialogForDetail<Sprite> { using Type = SpritePropertiesDialog; };
template<> struct PropertiesDialogForDetail<Sound> { using Type = SoundPropertiesDialog; };
template<> struct PropertiesDialogForDetail<Background> { using Type = BackgroundPropertiesDialog; };
template<> struct PropertiesDialogForDetail<Path> { using Type = PathPropertiesDialog; };
template<> struct PropertiesDialogForDetail<Script> { using Type = ScriptPropertiesDialog; };
template<> struct PropertiesDialogForDetail<Font> { using Type = FontPropertiesDialog; };
template<> struct PropertiesDialogForDetail<TimeLine> { using Type = TimeLinePropertiesDialog; };
template<> struct PropertiesDialogForDetail<Object> { using Type = ObjectPropertiesDialog; };
template<> struct PropertiesDialogForDetail<Room> { using Type = RoomPropertiesDialog; };
template<typename T> using PropertiesDialogFor = typename PropertiesDialogForDetail<T>::Type;
}

MainWindow::MainWindow(const QString &filePath, QWidget *parent) :
    QMainWindow{parent},
    m_ui{std::make_unique<Ui::MainWindow>()},
    m_filePath{filePath},
    m_projectTreeModel{std::make_unique<ProjectTreeModel>(m_project, this)}
{
    m_ui->setupUi(this);

    m_ui->splitter->setSizes({
        m_ui->splitter->width() / 2,
        m_ui->splitter->width() / 2
    });

    m_ui->actionNew->setShortcut(QKeySequence::New);
    m_ui->actionOpen->setShortcut(QKeySequence::Open);
    m_ui->actionSave->setShortcut(QKeySequence::Save);
    m_ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
    m_ui->actionExit->setShortcut(QKeySequence::Quit);
    m_ui->actionFindResource->setShortcut(QKeySequence::Find);
    m_ui->actionPrevious->setShortcut(QKeySequence::PreviousChild);
    m_ui->actionNext->setShortcut(QKeySequence::NextChild);
    m_ui->actionContents->setShortcut(QKeySequence::HelpContents);

    {
        const auto updateViewActions = [this](){
            m_ui->actionSubWindowedView->setChecked(m_ui->mdiArea->viewMode() == QMdiArea::SubWindowView);
            m_ui->actionTabbedView->setChecked(m_ui->mdiArea->viewMode() == QMdiArea::TabbedView);
        };
        updateViewActions();
        connect(m_ui->actionSubWindowedView, &QAction::triggered, m_ui->mdiArea, [mdiArea=m_ui->mdiArea,updateViewActions](){
            mdiArea->setViewMode(QMdiArea::SubWindowView);
            updateViewActions();
        });
        connect(m_ui->actionTabbedView, &QAction::triggered, m_ui->mdiArea, [mdiArea=m_ui->mdiArea,updateViewActions](){
            mdiArea->setViewMode(QMdiArea::TabbedView);
            updateViewActions();
        });
    }

    connect(m_ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(m_ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(m_ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(m_ui->actionCreateExecutable, &QAction::triggered, this, &MainWindow::createExecutable);
    connect(m_ui->actionPublishGame, &QAction::triggered, this, &MainWindow::publishGame);
    connect(m_ui->actionImportResources, &QAction::triggered, this, &MainWindow::importResources);
    connect(m_ui->actionExportResources, &QAction::triggered, this, &MainWindow::exportResources);
    connect(m_ui->actionPreferences, &QAction::triggered, this, &MainWindow::preferences);
    connect(m_ui->actionCreate, &QAction::triggered, this, &MainWindow::create);
    connect(m_ui->actionDuplicate, &QAction::triggered, this, &MainWindow::duplicate);
    connect(m_ui->actionCreateGroup, &QAction::triggered, this, &MainWindow::createGroup);
    connect(m_ui->actionSortByName, &QAction::triggered, this, &MainWindow::sortByName);
    connect(m_ui->actionDelete, &QAction::triggered, this, &MainWindow::delete_);
    connect(m_ui->actionRename, &QAction::triggered, this, &MainWindow::rename);
    connect(m_ui->actionProperties, &QAction::triggered, this, &MainWindow::showProperties);
    connect(m_ui->actionFindResource, &QAction::triggered, this, &MainWindow::findResource);
    connect(m_ui->actionShowObjectInformation, &QAction::triggered, this, &MainWindow::showObjectInformation);
    connect(m_ui->actionTransparentBackgroundSettings, &QAction::triggered, this, &MainWindow::transparentBackgroundSettings);
    connect(m_ui->actionCreateSprite, &QAction::triggered, this, &MainWindow::createFor<Sprite>);
    connect(m_ui->actionCreateSound, &QAction::triggered, this, &MainWindow::createFor<Sound>);
    connect(m_ui->actionCreateBackground, &QAction::triggered, this, &MainWindow::createFor<Background>);
    connect(m_ui->actionCreatePath, &QAction::triggered, this, &MainWindow::createFor<Path>);
    connect(m_ui->actionCreateScript, &QAction::triggered, this, &MainWindow::createFor<Script>);
    connect(m_ui->actionCreateFont, &QAction::triggered, this, &MainWindow::createFor<Font>);
    connect(m_ui->actionCreateTimeLine, &QAction::triggered, this, &MainWindow::createFor<TimeLine>);
    connect(m_ui->actionCreateObject, &QAction::triggered, this, &MainWindow::createFor<Object>);
    connect(m_ui->actionCreateRoom, &QAction::triggered, this, &MainWindow::createFor<Room>);
    connect(m_ui->actionGameInformation, &QAction::triggered, this, &MainWindow::showGameInformation);
    connect(m_ui->actionGlobalGameSettings, &QAction::triggered, this, &MainWindow::showGlobalGameSettings);
    connect(m_ui->actionExtensionPackages, &QAction::triggered, this, &MainWindow::showExtensionPackages);
    connect(m_ui->actionDefineConstants, &QAction::triggered, this, &MainWindow::showDefineConstants);
    connect(m_ui->actionDefineTriggers, &QAction::triggered, this, &MainWindow::showDefineTriggers);
    connect(m_ui->actionIncludedFiles, &QAction::triggered, this, &MainWindow::showIncludedFiles);
    connect(m_ui->actionRunNormally, &QAction::triggered, this, &MainWindow::runGame);
    connect(m_ui->actionRunInDebugMode, &QAction::triggered, this, &MainWindow::debugGame);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    setupStylesMenu();

    m_actionGroupViewMode = new QActionGroup{this};
    m_actionGroupViewMode->setExclusive(true);
    m_actionGroupViewMode->addAction(m_ui->actionSubWindowedView);
    m_actionGroupViewMode->addAction(m_ui->actionTabbedView);

    m_actionGroupWindows = new QActionGroup{this};
    m_actionGroupWindows->setExclusive(true);

    m_ui->treeView->setModel(m_projectTreeModel.get());

    connect(m_projectTreeModel.get(), &ProjectTreeModel::errorOccured,
            this, &MainWindow::modelErrorOccured);

    connect(m_ui->treeView, &QTreeView::customContextMenuRequested,
            this, &MainWindow::contextMenuRequested);

    connect(m_ui->treeView, &QTreeView::doubleClicked,
            this, &MainWindow::doubleClicked);

    connect(m_ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::selectionChanged);

    connect(m_projectTreeModel.get(), &ProjectTreeModel::rowsInserted,
            this, &MainWindow::rowsInserted);
    connect(m_projectTreeModel.get(), &QAbstractTableModel::rowsInserted,
            this, &MainWindow::changed);
    connect(m_projectTreeModel.get(), &ProjectTreeModel::rowsAboutToBeRemoved,
            this, &MainWindow::rowsAboutToBeRemoved);
    connect(m_projectTreeModel.get(), &QAbstractTableModel::rowsRemoved,
            this, &MainWindow::changed);
    connect(m_projectTreeModel.get(), &ProjectTreeModel::modelAboutToBeReset,
            this, &MainWindow::modelAboutToBeReset);
    connect(m_projectTreeModel.get(), &QAbstractTableModel::dataChanged,
            this, &MainWindow::changed);

    updateTitle();

    if (!m_filePath.isEmpty())
        loadFile(m_filePath);
}

template<>
std::map<Sprite*, QMdiSubWindow*> &MainWindow::propertyWindowsFor<Sprite>()
{
    return m_spritePropertiesWindows;
}

template<>
std::map<Sound*, QMdiSubWindow*> &MainWindow::propertyWindowsFor<Sound>()
{
    return m_soundPropertiesWindows;
}

template<>
std::map<Background*, QMdiSubWindow*> &MainWindow::propertyWindowsFor<Background>()
{
    return m_backgroundPropertiesWindows;
}

template<>
std::map<Path*, QMdiSubWindow*> &MainWindow::propertyWindowsFor<Path>()
{
    return m_pathPropertiesWindows;
}

template<>
std::map<Script*, QMdiSubWindow*> &MainWindow::propertyWindowsFor<Script>()
{
    return m_scriptPropertiesWindows;
}

template<>
std::map<Font*, QMdiSubWindow*> &MainWindow::propertyWindowsFor<Font>()
{
    return m_fontPropertiesWindows;
}

template<>
std::map<TimeLine*, QMdiSubWindow*> &MainWindow::propertyWindowsFor<TimeLine>()
{
    return m_timeLinePropertiesWindows;
}

template<>
std::map<Object*, QMdiSubWindow*> &MainWindow::propertyWindowsFor<Object>()
{
    return m_objectPropertiesWindows;
}

template<>
std::map<Room*, QMdiSubWindow*> &MainWindow::propertyWindowsFor<Room>()
{
    return m_roomPropertiesWindows;
}

template<typename T>
void MainWindow::openPropertiesWindowFor(T &entry)
{
    auto &propertyWindows = propertyWindowsFor<T>();
    if (const auto iter = propertyWindows.find(&entry); iter != std::cend(propertyWindows))
    {
        m_ui->mdiArea->setActiveSubWindow(iter->second);
        return;
    }

    auto dialog = new PropertiesDialogFor<T>{entry, *m_projectTreeModel, *this};
    auto subwindow = m_ui->mdiArea->addSubWindow(dialog);
    auto action = m_ui->menuWindow->addAction(dialog->windowTitle());
    m_actionGroupWindows->addAction(action);
    action->setCheckable(true);
    connect(action, &QAction::triggered,
            m_ui->mdiArea, [mdiArea=m_ui->mdiArea,subwindow,action](){
                mdiArea->setActiveSubWindow(subwindow);
                action->setChecked(subwindow->windowState().testFlag(Qt::WindowActive));
            });
    connect(subwindow, &QMdiSubWindow::windowStateChanged,
            action, [action](Qt::WindowStates oldState, Qt::WindowStates newState){
                Q_UNUSED(oldState)
                action->setChecked(newState.testFlag(Qt::WindowActive));
            });
    connect(dialog, &QWidget::windowTitleChanged, action, &QAction::setText);
    connect(dialog, &QDialog::finished,
            this, [this,&propertyWindows,subwindow](int result){
                if (result == QDialog::Accepted)
                    changed();
                for (auto iter = std::begin(propertyWindows); iter != std::end(propertyWindows); )
                {
                    if (iter->second == subwindow)
                        iter = propertyWindows.erase(iter);
                    else
                        iter++;
                }
            });
    connect(dialog, &QDialog::finished,
            subwindow, &QObject::deleteLater);
    connect(dialog, &QDialog::finished,
            action, &QObject::deleteLater);
    propertyWindows[&entry] = subwindow;
    dialog->show();
}

template void MainWindow::openPropertiesWindowFor<Sprite>(Sprite &entry);
template void MainWindow::openPropertiesWindowFor<Sound>(Sound &entry);
template void MainWindow::openPropertiesWindowFor<Background>(Background &entry);
template void MainWindow::openPropertiesWindowFor<Path>(Path &entry);
template void MainWindow::openPropertiesWindowFor<Script>(Script &entry);
template void MainWindow::openPropertiesWindowFor<Font>(Font &entry);
template void MainWindow::openPropertiesWindowFor<TimeLine>(TimeLine &entry);
template void MainWindow::openPropertiesWindowFor<Object>(Object &entry);
template void MainWindow::openPropertiesWindowFor<Room>(Room &entry);

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_ui->mdiArea->closeAllSubWindows();
    if (!m_ui->mdiArea->subWindowList().empty())
    {
        event->ignore();
        return;
    }

    if (m_unsavedChanges)
    {
        const auto result = QMessageBox::warning(
            this,
            tr("The Game has been modified."),
            tr("Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save
        );
        switch (result)
        {
        case QMessageBox::Save:
            saveFile();
            if (m_unsavedChanges)
            {
                event->ignore();
                return;
            }
            else
                break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            event->ignore();
            return;
        default:
            qWarning() << "unexpected dialog result" << result;
            event->ignore();
            return;
        }
    }

    QMainWindow::closeEvent(event);
}

MainWindow::~MainWindow() = default;

void MainWindow::contextMenuRequested(const QPoint &pos)
{
    QMenu menu{this};
    menu.addAction(m_ui->actionCreate);
    menu.addAction(m_ui->actionDuplicate);
    menu.addSeparator();
    menu.addAction(m_ui->actionCreateGroup);
    menu.addSeparator();
    menu.addAction(m_ui->actionSortByName);
    menu.addSeparator();
    menu.addAction(m_ui->actionDelete);
    menu.addSeparator();
    menu.addAction(m_ui->actionRename);
    menu.addSeparator();
    menu.addAction(m_ui->actionProperties);
    menu.exec(m_ui->treeView->viewport()->mapToGlobal(pos));
}

void MainWindow::doubleClicked(const QModelIndex &index)
{
    if (m_projectTreeModel->nodeType(index) == ProjectTreeModel::NodeType::Root)
    {
        if (index == m_projectTreeModel->gameInformationRoot())
            showGameInformation();
        else if (index == m_projectTreeModel->globalGameSettingsRoot())
            showGlobalGameSettings();
        else if (index == m_projectTreeModel->extensionPackagesRoot())
            showExtensionPackages();
        return;
    }
    if (doubleClickedFor<Sprite>(index)) return;
    if (doubleClickedFor<Sound>(index)) return;
    if (doubleClickedFor<Background>(index)) return;
    if (doubleClickedFor<Path>(index)) return;
    if (doubleClickedFor<Script>(index)) return;
    if (doubleClickedFor<Font>(index)) return;
    if (doubleClickedFor<TimeLine>(index)) return;
    if (doubleClickedFor<Object>(index)) return;
    if (doubleClickedFor<Room>(index)) return;
}

void MainWindow::selectionChanged(const QModelIndex &index)
{
    switch (const auto nodeType = m_projectTreeModel->nodeType(index))
    {
    case ProjectTreeModel::NodeType::Root:
        if (index == m_projectTreeModel->rootFor<Sprite>() ||
            index == m_projectTreeModel->rootFor<Sound>() ||
            index == m_projectTreeModel->rootFor<Background>() ||
            index == m_projectTreeModel->rootFor<Path>() ||
            index == m_projectTreeModel->rootFor<Script>() ||
            index == m_projectTreeModel->rootFor<Font>() ||
            index == m_projectTreeModel->rootFor<TimeLine>() ||
            index == m_projectTreeModel->rootFor<Object>() ||
            index == m_projectTreeModel->rootFor<Room>())
        {
            m_ui->actionCreate->setEnabled(true);
            m_ui->actionCreateGroup->setText(tr("Cr&eate Group"));
            m_ui->actionCreateGroup->setEnabled(true);
            m_ui->actionSortByName->setEnabled(true);

            if (index == m_projectTreeModel->rootFor<Sprite>())          m_ui->actionCreate->setText(tr("&Create Sprite"));
            else if (index == m_projectTreeModel->rootFor<Sound>())      m_ui->actionCreate->setText(tr("&Create Sound"));
            else if (index == m_projectTreeModel->rootFor<Background>()) m_ui->actionCreate->setText(tr("&Create Background"));
            else if (index == m_projectTreeModel->rootFor<Path>())       m_ui->actionCreate->setText(tr("&Create Path"));
            else if (index == m_projectTreeModel->rootFor<Script>())     m_ui->actionCreate->setText(tr("&Create Script"));
            else if (index == m_projectTreeModel->rootFor<Font>())       m_ui->actionCreate->setText(tr("&Create Font"));
            else if (index == m_projectTreeModel->rootFor<TimeLine>())   m_ui->actionCreate->setText(tr("&Create Time Line"));
            else if (index == m_projectTreeModel->rootFor<Object>())     m_ui->actionCreate->setText(tr("&Create Object"));
            else if (index == m_projectTreeModel->rootFor<Room>())       m_ui->actionCreate->setText(tr("&Create Room"));
        }
        else
        {
            m_ui->actionCreate->setEnabled(false);
            m_ui->actionCreate->setText(tr("&Create"));
            m_ui->actionCreateGroup->setText(tr("Cr&eate Group"));
            m_ui->actionCreateGroup->setEnabled(false);
            m_ui->actionSortByName->setEnabled(false);
        }

        m_ui->actionDuplicate->setEnabled(false);
        m_ui->actionDelete->setEnabled(false);
        m_ui->actionRename->setEnabled(false);
        m_ui->actionProperties->setEnabled(
            index == m_projectTreeModel->gameInformationRoot() ||
            index == m_projectTreeModel->globalGameSettingsRoot() ||
            index == m_projectTreeModel->extensionPackagesRoot());
        break;
    case ProjectTreeModel::NodeType::Sprite:
    case ProjectTreeModel::NodeType::Sound:
    case ProjectTreeModel::NodeType::Background:
    case ProjectTreeModel::NodeType::Path:
    case ProjectTreeModel::NodeType::Script:
    case ProjectTreeModel::NodeType::Font:
    case ProjectTreeModel::NodeType::TimeLine:
    case ProjectTreeModel::NodeType::Object:
    case ProjectTreeModel::NodeType::Room:
        m_ui->actionCreate->setEnabled(true);
        switch (nodeType)
        {
        case ProjectTreeModel::NodeType::Sprite:     m_ui->actionCreate->setText(tr("&Insert Sprite")); break;
        case ProjectTreeModel::NodeType::Sound:      m_ui->actionCreate->setText(tr("&Insert Sound")); break;
        case ProjectTreeModel::NodeType::Background: m_ui->actionCreate->setText(tr("&Insert Background")); break;
        case ProjectTreeModel::NodeType::Path:       m_ui->actionCreate->setText(tr("&Insert Path")); break;
        case ProjectTreeModel::NodeType::Script:     m_ui->actionCreate->setText(tr("&Insert Script")); break;
        case ProjectTreeModel::NodeType::Font:       m_ui->actionCreate->setText(tr("&Insert Font")); break;
        case ProjectTreeModel::NodeType::TimeLine:   m_ui->actionCreate->setText(tr("&Insert Time Line")); break;
        case ProjectTreeModel::NodeType::Object:     m_ui->actionCreate->setText(tr("&Insert Object")); break;
        case ProjectTreeModel::NodeType::Room:       m_ui->actionCreate->setText(tr("&Insert Room")); break;
        default:
            __builtin_unreachable();
        }
        m_ui->actionDuplicate->setEnabled(true);
        m_ui->actionCreateGroup->setText(tr("I&nsert Group"));
        m_ui->actionCreateGroup->setEnabled(true);
        m_ui->actionSortByName->setEnabled(false);
        m_ui->actionDelete->setEnabled(true);
        m_ui->actionRename->setEnabled(true);
        m_ui->actionProperties->setEnabled(true);
        break;
    default:
        m_ui->actionCreate->setEnabled(false);
        m_ui->actionCreate->setText(tr("&Create"));
        m_ui->actionDuplicate->setEnabled(false);
        m_ui->actionCreateGroup->setText(tr("Cr&eate Group"));
        m_ui->actionCreateGroup->setEnabled(false);
        m_ui->actionSortByName->setEnabled(false);
        m_ui->actionDelete->setEnabled(false);
        m_ui->actionRename->setEnabled(false);
        m_ui->actionProperties->setEnabled(false);
    }
}

void MainWindow::modelErrorOccured(const QString &message)
{
    QMessageBox::warning(this, tr("Error occured!"), tr("Error occured!") + "\n\n" + message);
}

void MainWindow::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void MainWindow::newFile()
{
    m_ui->mdiArea->closeAllSubWindows();
    if (!m_ui->mdiArea->subWindowList().empty())
        return;

    if (m_unsavedChanges)
    {
        const auto result = QMessageBox::warning(
            this,
            tr("The Game has been modified."),
            tr("Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save
        );
        switch (result)
        {
        case QMessageBox::Save:
            saveFile();
            if (m_unsavedChanges)
                return;
            else
                break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            return;
        default:
            qWarning() << "unexpected dialog result" << result;
            return;
        }
    }

    m_project = {};
    m_projectTreeModel->setProject(&m_project);

    m_filePath = QString{};
    m_unsavedChanges = false;

    updateTitle();
}

void MainWindow::openFile()
{
    auto path = QFileDialog::getOpenFileName(this, tr("Load a game"), {}, tr("GMK Files (*.gmk)"));
    if (path.isEmpty())
        return;

    loadFile(path);
}

void MainWindow::saveFile()
{
    if (m_filePath.isEmpty())
    {
        saveFileAs();
        return;
    }

    {
        QFile file{m_filePath};
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QMessageBox::warning(this, tr("Could not save game!"), tr("Could not save game!") + "\n\n" + file.errorString());
            saveFileAs();
            return;
        }

        QDataStream dataStream{&file};
        dataStream << m_project;
    }

    m_unsavedChanges = false;

    updateTitle();
}

void MainWindow::saveFileAs()
{
    auto path = QFileDialog::getSaveFileName(this, tr("Save the game"), "mygame.gmk", tr("GMK Files (*.gmk)"));
    if (path.isEmpty())
        return;

    {
        QFile file{path};
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QMessageBox::warning(this, tr("Could not save game!"), tr("Could not save game!") + "\n\n" + file.errorString());
            return;
        }

        QDataStream dataStream{&file};
        dataStream << m_project;
    }

    m_filePath = path;
    m_unsavedChanges = false;

    updateTitle();
}

void MainWindow::createExecutable()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void MainWindow::publishGame()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void MainWindow::importResources()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void MainWindow::exportResources()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void MainWindow::preferences()
{
    PreferencesDialog dialog{this};
    dialog.exec();
}

void MainWindow::create()
{
    const auto index = m_ui->treeView->currentIndex();
    if (!index.isValid())
        return;

         if (index == m_projectTreeModel->rootFor<Sprite>())     createFor<Sprite>();
    else if (index == m_projectTreeModel->rootFor<Sound>())      createFor<Sound>();
    else if (index == m_projectTreeModel->rootFor<Background>()) createFor<Background>();
    else if (index == m_projectTreeModel->rootFor<Path>())       createFor<Path>();
    else if (index == m_projectTreeModel->rootFor<Script>())     createFor<Script>();
    else if (index == m_projectTreeModel->rootFor<Font>())       createFor<Font>();
    else if (index == m_projectTreeModel->rootFor<TimeLine>())   createFor<TimeLine>();
    else if (index == m_projectTreeModel->rootFor<Object>())     createFor<Object>();
    else if (index == m_projectTreeModel->rootFor<Room>())       createFor<Room>();
    else
    {
        switch (m_projectTreeModel->nodeType(index))
        {
        using NodeType = ProjectTreeModel::NodeType;
        case NodeType::Root:
            break;
        case NodeType::Sprite:
        case NodeType::Sound:
        case NodeType::Background:
        case NodeType::Path:
        case NodeType::Script:
        case NodeType::Font:
        case NodeType::TimeLine:
        case NodeType::Object:
        case NodeType::Room:
            if (!m_projectTreeModel->insertRows(index.row(), 1, index.parent()))
                QMessageBox::warning(this, tr("Inserting failed!"), tr("Inserting failed!"));
            break;
        }
    }
}

void MainWindow::duplicate()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void MainWindow::createGroup()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void MainWindow::sortByName()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void MainWindow::delete_()
{
    const auto index = m_ui->treeView->currentIndex();
    if (!index.isValid())
        return;

    switch (m_projectTreeModel->nodeType(index))
    {
    using NodeType = ProjectTreeModel::NodeType;
    case NodeType::Root:
        return;
    case NodeType::Sprite:
    case NodeType::Sound:
    case NodeType::Background:
    case NodeType::Path:
    case NodeType::Script:
    case NodeType::Font:
    case NodeType::TimeLine:
    case NodeType::Object:
    case NodeType::Room:
        if (QMessageBox::question(this,
                                  tr("Confirm"),
                                  tr("You are about to delete %0. This will be permanent. Continue?")
                                      .arg(m_projectTreeModel->data(index, Qt::DisplayRole).toString())
                                  ) != QMessageBox::Yes)
            return;
        break;
    }

    if (!m_projectTreeModel->removeRow(index.row(), index.parent()))
        QMessageBox::warning(this, tr("Deleting failed!"), tr("Deleting failed!"));
}

void MainWindow::rename()
{
    m_ui->treeView->edit(m_ui->treeView->currentIndex());
}

void MainWindow::showProperties()
{
    const auto index = m_ui->treeView->currentIndex();
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index";
        return;
    }

    doubleClicked(index);
}

void MainWindow::findResource()
{
    bool ok;
    const auto filter = QInputDialog::getText(this, tr("Finding a resource"), tr("Resource name:"), QLineEdit::Normal, {}, &ok);
    if (!ok)
        return;

    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void MainWindow::showObjectInformation()
{
    openOrActivateWindow<ObjectInformationDialog>(m_objectInformationWindow, m_project);
}

void MainWindow::transparentBackgroundSettings()
{
    TransparentBackgroundSettingsDialog dialog{this};
    dialog.exec();
}

template<typename T>
void MainWindow::createFor()
{
    if (!m_projectTreeModel->insertRows(m_project.containerFor<T>().size(), 1, m_projectTreeModel->rootFor<T>()))
        QMessageBox::warning(this, tr("Creating failed!"), tr("Creating failed!"));
}

template void MainWindow::createFor<Sprite>();
template void MainWindow::createFor<Sound>();
template void MainWindow::createFor<Background>();
template void MainWindow::createFor<Path>();
template void MainWindow::createFor<Script>();
template void MainWindow::createFor<Font>();
template void MainWindow::createFor<TimeLine>();
template void MainWindow::createFor<Object>();
template void MainWindow::createFor<Room>();

void MainWindow::showGameInformation()
{
    GameInformationDialog dialog{this};
    if (dialog.exec() /*== QDialog::Accepted*/)
        changed();
}

void MainWindow::showGlobalGameSettings()
{
    GlobalGameSettingsDialog dialog{m_project.globalGameSettings, this};
    if (dialog.exec() == QDialog::Accepted)
        changed();
}

void MainWindow::showExtensionPackages()
{
    ExtensionPackagesDialog dialog{this};
    dialog.exec();
}

void MainWindow::showDefineConstants()
{
    openOrActivateWindow<UserDefinedConstantsDialog>(m_userDefinedConstantsWindow, m_project);
}

void MainWindow::showDefineTriggers()
{
    openOrActivateWindow<TriggersDialog>(m_triggersWindow, m_project);
}

void MainWindow::showIncludedFiles()
{
    openOrActivateWindow<IncludedFilesDialog>(m_includedFilesWindow, m_project);
}

void MainWindow::runGame()
{
    GameEngine engine{m_project};

    setEnabled(false);
    engine.run();
    setEnabled(true);
}

void MainWindow::debugGame()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Qt Game Maker"),
                       tr("This <b>About-Dialog</b> is not finished."));
}

void MainWindow::rowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(last)

    m_ui->treeView->expand(parent);
    const auto index = m_projectTreeModel->index(first, 0, parent);
    m_ui->treeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    doubleClicked(index);
}

void MainWindow::rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    if (rowsAboutToBeRemovedFor<Sprite>(parent, first, last)) return;
    if (rowsAboutToBeRemovedFor<Sound>(parent, first, last)) return;
    if (rowsAboutToBeRemovedFor<Background>(parent, first, last)) return;
    if (rowsAboutToBeRemovedFor<Path>(parent, first, last)) return;
    if (rowsAboutToBeRemovedFor<Script>(parent, first, last)) return;
    if (rowsAboutToBeRemovedFor<Font>(parent, first, last)) return;
    if (rowsAboutToBeRemovedFor<TimeLine>(parent, first, last)) return;
    if (rowsAboutToBeRemovedFor<Object>(parent, first, last)) return;
    if (rowsAboutToBeRemovedFor<Room>(parent, first, last)) return;
}

void MainWindow::modelAboutToBeReset()
{
    modelAboutToBeResetFor<Sprite>();
    modelAboutToBeResetFor<Sound>();
    modelAboutToBeResetFor<Background>();
    modelAboutToBeResetFor<Path>();
    modelAboutToBeResetFor<Script>();
    modelAboutToBeResetFor<Font>();
    modelAboutToBeResetFor<TimeLine>();
    modelAboutToBeResetFor<Object>();
    modelAboutToBeResetFor<Room>();
}

void MainWindow::setupStylesMenu()
{
    auto currentStyle = QApplication::style();
    qDebug() << currentStyle;

    m_actionGroupStyles = new QActionGroup{this};
    m_actionGroupStyles->setExclusive(true);

    for (const auto &key : QStyleFactory::keys())
    {
        auto name = [&key](){
            const std::unique_ptr<QStyle> style{QStyleFactory::create(key)};
            if (!style)
            {
                qWarning() << "invalid style" << key;
                return QString{};
            }

            return style->
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                name
#else
                objectName
#endif
                ();
        }();

        if (name.isEmpty())
            continue;

        auto action = m_ui->menuStyle->addAction(tr("%0 (%1)").arg(key, name), this, [this,key](){
            auto style = QStyleFactory::create(key);
            if (!style)
            {
                QMessageBox::warning(this, tr("Could not instantiate Style!"), tr("Could not instantiate Style!"));
                return;
            }
            QApplication::setStyle(style);
        });
        m_actionGroupStyles->addAction(action);
        action->setCheckable(true);
        action->setChecked(currentStyle && currentStyle->
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                           name
#else
                           objectName
#endif
                           () == name);
    }
}

void MainWindow::loadFile(const QString &path)
{
    ProjectContainer project;

    {
        QFile file{path};
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, tr("Could not load game!"), tr("Could not load game!") + "\n\n" + file.errorString());
            return;
        }

        QDataStream dataStream{&file};
        dataStream >> project;
    }

    m_ui->mdiArea->closeAllSubWindows();
    if (!m_ui->mdiArea->subWindowList().empty())
        return;

    if (m_unsavedChanges)
    {
        const auto result = QMessageBox::warning(
            this,
            tr("The Game has been modified."),
            tr("Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save
            );
        switch (result)
        {
        case QMessageBox::Save:
            saveFile();
            if (m_unsavedChanges)
                return;
            else
                break;
        case QMessageBox::Discard:
            break;
        case QMessageBox::Cancel:
            return;
        default:
            qWarning() << "unexpected dialog result" << result;
            return;
        }
    }

    m_project = std::move(project);
    m_projectTreeModel->setProject(&m_project);

    m_filePath = path;
    m_unsavedChanges = false;

    updateTitle();
}

void MainWindow::updateTitle()
{
    setWindowTitle(tr("%0%1 - Qt Gamemaker 1.0 Ultimate")
                       .arg(m_filePath.isEmpty() ? "<new-game>" : QFileInfo{m_filePath}.fileName())
                       .arg(m_unsavedChanges ? tr("*") : QString{}));
}

template<typename T, typename ...Targs>
void MainWindow::openOrActivateWindow(QMdiSubWindow * &ptr, Targs &&...args)
{
    if (ptr)
        m_ui->mdiArea->setActiveSubWindow(ptr);
    else
    {
        auto dialog = new T{std::forward<Targs>(args)...};
        auto subwindow = m_ui->mdiArea->addSubWindow(dialog);
        auto action = m_ui->menuWindow->addAction(dialog->windowTitle());
        m_actionGroupWindows->addAction(action);
        action->setCheckable(true);
        connect(action, &QAction::triggered,
                m_ui->mdiArea, [mdiArea=m_ui->mdiArea,subwindow,action](){
                    mdiArea->setActiveSubWindow(subwindow);
                    action->setChecked(subwindow->windowState().testFlag(Qt::WindowActive));
                });
        connect(subwindow, &QMdiSubWindow::windowStateChanged,
                action, [action](Qt::WindowStates oldState, Qt::WindowStates newState){
                    Q_UNUSED(oldState)
                    action->setChecked(newState.testFlag(Qt::WindowActive));
                });
        connect(dialog, &QWidget::windowTitleChanged, action, &QAction::setText);
        connect(dialog, &QDialog::finished,
                this, [&ptr](){
                    ptr = nullptr;
                });
        connect(dialog, &QDialog::finished,
                subwindow, &QObject::deleteLater);
        connect(dialog, &QDialog::finished,
                action, &QObject::deleteLater);
        ptr = subwindow;
        dialog->show();
    }
}

template<typename T>
bool MainWindow::doubleClickedFor(const QModelIndex &index)
{
    if (m_projectTreeModel->nodeType(index) != ProjectTreeModel::nodeTypeFor<T>())
        return false;

    auto entry = m_projectTreeModel->get<T>(index);
    if (!entry)
        return true;

    openPropertiesWindowFor(*entry);

    return true;
}

template<typename T>
bool MainWindow::rowsAboutToBeRemovedFor(const QModelIndex &parent, int first, int last)
{
    if (parent != m_projectTreeModel->rootFor<T>())
        return false;

    for (int row = first; row <= last; row++)
    {
        if (const auto entry = m_projectTreeModel->get<T>(m_projectTreeModel->index(row, 0, parent)))
        {
            auto &propertyWindows = propertyWindowsFor<T>();
            if (const auto iter = propertyWindows.find(entry); iter != std::end(propertyWindows))
            {
                delete iter->second;
                propertyWindows.erase(iter);
            }
        }
    }

    return true;
}

template<typename T>
void MainWindow::modelAboutToBeResetFor()
{
    auto &propertyWindows = propertyWindowsFor<T>();
    for (const auto &pair : propertyWindows)
        delete pair.second;
    propertyWindows.clear();
}
