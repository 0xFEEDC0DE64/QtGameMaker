#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMdiSubWindow>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>

#include "projecttreemodel.h"
#include "dialogs/spritepropertiesdialog.h"
#include "dialogs/soundpropertiesdialog.h"
#include "dialogs/backgroundpropertiesdialog.h"
#include "dialogs/pathpropertiesdialog.h"
#include "dialogs/scriptpropertiesdialog.h"
#include "dialogs/fontpropertiesdialog.h"
#include "dialogs/preferencesdialog.h"
#include "dialogs/gameinformationdialog.h"
#include "dialogs/globalgamesettingsdialog.h"
#include "dialogs/extensionpackagesdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    m_ui{std::make_unique<Ui::MainWindow>()},
    m_projectTreeModel{std::make_unique<ProjectTreeModel>(&m_project, this)}
{
    m_ui->setupUi(this);

    {
        QList<int> sizes;
        sizes.append(0.5 * m_ui->splitter->width());
        sizes.append(0.5 * m_ui->splitter->width());
        m_ui->splitter->setSizes(sizes);
    }

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
    connect(m_ui->actionCreateSprite, &QAction::triggered, this, &MainWindow::createSprite);
    connect(m_ui->actionCreateSound, &QAction::triggered, this, &MainWindow::createSound);
    connect(m_ui->actionCreateBackground, &QAction::triggered, this, &MainWindow::createBackground);
    connect(m_ui->actionCreatePath, &QAction::triggered, this, &MainWindow::createPath);
    connect(m_ui->actionCreateScript, &QAction::triggered, this, &MainWindow::createScript);
    connect(m_ui->actionCreateFont, &QAction::triggered, this, &MainWindow::createFont);
    connect(m_ui->actionGameInformation, &QAction::triggered, this, &MainWindow::showGameInformation);
    connect(m_ui->actionGlobalGameSettings, &QAction::triggered, this, &MainWindow::showGlobalGameSettings);
    connect(m_ui->actionExtensionPackages, &QAction::triggered, this, &MainWindow::showExtensionPackages);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

    m_ui->treeView->setModel(m_projectTreeModel.get());

    connect(m_ui->treeView, &QTreeView::customContextMenuRequested,
            this, &MainWindow::contextMenuRequested);

    connect(m_ui->treeView, &QTreeView::doubleClicked,
            this, &MainWindow::doubleClicked);

    connect(m_ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::selectionChanged);

    connect(m_projectTreeModel.get(), &ProjectTreeModel::rowsInserted,
            this, &MainWindow::rowsInserted);
    connect(m_projectTreeModel.get(), &ProjectTreeModel::rowsAboutToBeRemoved,
            this, &MainWindow::rowsAboutToBeRemoved);
    connect(m_projectTreeModel.get(), &ProjectTreeModel::modelAboutToBeReset,
            this, &MainWindow::modelAboutToBeReset);

    updateTitle();
}

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
    switch (m_projectTreeModel->nodeType(index))
    {
    case ProjectTreeModel::NodeType::Root:
        if (index == m_projectTreeModel->gameInformationRoot())
            showGameInformation();
        else if (index == m_projectTreeModel->globalGameSettingsRoot())
            showGlobalGameSettings();
        else if (index == m_projectTreeModel->extensionPackagesRoot())
            showExtensionPackages();
        break;
    case ProjectTreeModel::NodeType::Sprite:
    {
        auto sprite = m_projectTreeModel->getSprite(index);
        if (!sprite)
            break;

        if (const auto iter = m_spritePropertiesDialogs.find(sprite); iter != std::cend(m_spritePropertiesDialogs))
        {
            m_ui->mdiArea->setActiveSubWindow(iter->second);
        }
        else
        {
            auto dialog = new SpritePropertiesDialog{*sprite};
            auto subwindow = m_ui->mdiArea->addSubWindow(dialog);
            auto action = m_ui->menuWindow->addAction(dialog->windowTitle());
            action->setCheckable(true);
            connect(action, &QAction::triggered,
                    m_ui->mdiArea, [mdiArea=m_ui->mdiArea,subwindow,action](){
                mdiArea->setActiveSubWindow(subwindow);
                action->setChecked(subwindow->windowState().testFlag(Qt::WindowActive));
            });
            connect(subwindow, &QMdiSubWindow::windowStateChanged,
                    action, [action](Qt::WindowStates oldState, Qt::WindowStates newState){
                action->setChecked(newState.testFlag(Qt::WindowActive));
            });
            connect(dialog, &QWidget::windowTitleChanged, action, &QAction::setText);
            connect(dialog, &QDialog::finished,
                    this, [&spritePropertiesDialogs=m_spritePropertiesDialogs,subwindow](){
                        for (auto iter = std::begin(spritePropertiesDialogs); iter != std::end(spritePropertiesDialogs); )
                        {
                            if (iter->second == subwindow)
                                iter = spritePropertiesDialogs.erase(iter);
                            else
                                iter++;
                        }
                    });
            connect(dialog, &QDialog::finished,
                    subwindow, &QObject::deleteLater);
            connect(dialog, &QDialog::finished,
                    action, &QObject::deleteLater);
            m_spritePropertiesDialogs[sprite] = subwindow;
            dialog->show();
        }
        break;
    }
    case ProjectTreeModel::NodeType::Sound:
    {
        auto sound = m_projectTreeModel->getSound(index);
        if (!sound)
            break;

        if (const auto iter = m_soundPropertiesDialogs.find(sound); iter != std::cend(m_soundPropertiesDialogs))
        {
            m_ui->mdiArea->setActiveSubWindow(iter->second);
        }
        else
        {
            auto dialog = new SoundPropertiesDialog{*sound};
            auto subwindow = m_ui->mdiArea->addSubWindow(dialog);
            auto action = m_ui->menuWindow->addAction(dialog->windowTitle());
            action->setCheckable(true);
            connect(action, &QAction::triggered,
                    m_ui->mdiArea, [mdiArea=m_ui->mdiArea,subwindow,action](){
                        mdiArea->setActiveSubWindow(subwindow);
                        action->setChecked(subwindow->windowState().testFlag(Qt::WindowActive));
                    });
            connect(subwindow, &QMdiSubWindow::windowStateChanged,
                    action, [action](Qt::WindowStates oldState, Qt::WindowStates newState){
                        action->setChecked(newState.testFlag(Qt::WindowActive));
                    });
            connect(dialog, &QWidget::windowTitleChanged, action, &QAction::setText);
            connect(dialog, &QDialog::finished,
                    this, [&soundPropertiesDialogs=m_soundPropertiesDialogs,subwindow](){
                        for (auto iter = std::begin(soundPropertiesDialogs); iter != std::end(soundPropertiesDialogs); )
                        {
                            if (iter->second == subwindow)
                                iter = soundPropertiesDialogs.erase(iter);
                            else
                                iter++;
                        }
                    });
            connect(dialog, &QDialog::finished,
                    subwindow, &QObject::deleteLater);
            connect(dialog, &QDialog::finished,
                    action, &QObject::deleteLater);
            m_soundPropertiesDialogs[sound] = subwindow;
            dialog->show();
        }
        break;
    }
    case ProjectTreeModel::NodeType::Background:
    {
        auto background = m_projectTreeModel->getBackground(index);
        if (!background)
            break;

        if (const auto iter = m_backgroundPropertiesDialogs.find(background); iter != std::cend(m_backgroundPropertiesDialogs))
        {
            m_ui->mdiArea->setActiveSubWindow(iter->second);
        }
        else
        {
            auto dialog = new BackgroundPropertiesDialog{*background};
            auto subwindow = m_ui->mdiArea->addSubWindow(dialog);
            auto action = m_ui->menuWindow->addAction(dialog->windowTitle());
            action->setCheckable(true);
            connect(action, &QAction::triggered,
                    m_ui->mdiArea, [mdiArea=m_ui->mdiArea,subwindow,action](){
                        mdiArea->setActiveSubWindow(subwindow);
                        action->setChecked(subwindow->windowState().testFlag(Qt::WindowActive));
                    });
            connect(subwindow, &QMdiSubWindow::windowStateChanged,
                    action, [action](Qt::WindowStates oldState, Qt::WindowStates newState){
                        action->setChecked(newState.testFlag(Qt::WindowActive));
                    });
            connect(dialog, &QWidget::windowTitleChanged, action, &QAction::setText);
            connect(dialog, &QDialog::finished,
                    this, [&backgroundPropertiesDialogs=m_backgroundPropertiesDialogs,subwindow](){
                        for (auto iter = std::begin(backgroundPropertiesDialogs); iter != std::end(backgroundPropertiesDialogs); )
                        {
                            if (iter->second == subwindow)
                                iter = backgroundPropertiesDialogs.erase(iter);
                            else
                                iter++;
                        }
                    });
            connect(dialog, &QDialog::finished,
                    subwindow, &QObject::deleteLater);
            connect(dialog, &QDialog::finished,
                    action, &QObject::deleteLater);
            m_backgroundPropertiesDialogs[background] = subwindow;
            dialog->show();
        }
        break;
    }
    case ProjectTreeModel::NodeType::Path:
    {
        auto path = m_projectTreeModel->getPath(index);
        if (!path)
            break;

        if (const auto iter = m_pathPropertiesDialogs.find(path); iter != std::cend(m_pathPropertiesDialogs))
        {
            m_ui->mdiArea->setActiveSubWindow(iter->second);
        }
        else
        {
            auto dialog = new PathPropertiesDialog{*path};
            auto subwindow = m_ui->mdiArea->addSubWindow(dialog);
            auto action = m_ui->menuWindow->addAction(dialog->windowTitle());
            action->setCheckable(true);
            connect(action, &QAction::triggered,
                    m_ui->mdiArea, [mdiArea=m_ui->mdiArea,subwindow,action](){
                        mdiArea->setActiveSubWindow(subwindow);
                        action->setChecked(subwindow->windowState().testFlag(Qt::WindowActive));
                    });
            connect(subwindow, &QMdiSubWindow::windowStateChanged,
                    action, [action](Qt::WindowStates oldState, Qt::WindowStates newState){
                        action->setChecked(newState.testFlag(Qt::WindowActive));
                    });
            connect(dialog, &QWidget::windowTitleChanged, action, &QAction::setText);
            connect(dialog, &QDialog::finished,
                    this, [&pathPropertiesDialogs=m_pathPropertiesDialogs,subwindow](){
                        for (auto iter = std::begin(pathPropertiesDialogs); iter != std::end(pathPropertiesDialogs); )
                        {
                            if (iter->second == subwindow)
                                iter = pathPropertiesDialogs.erase(iter);
                            else
                                iter++;
                        }
                    });
            connect(dialog, &QDialog::finished,
                    subwindow, &QObject::deleteLater);
            connect(dialog, &QDialog::finished,
                    action, &QObject::deleteLater);
            m_pathPropertiesDialogs[path] = subwindow;
            dialog->show();
        }
        break;
    }
    case ProjectTreeModel::NodeType::Script:
    {
        auto script = m_projectTreeModel->getScript(index);
        if (!script)
            break;

        if (const auto iter = m_scriptPropertiesDialogs.find(script); iter != std::cend(m_scriptPropertiesDialogs))
        {
            m_ui->mdiArea->setActiveSubWindow(iter->second);
        }
        else
        {
            auto dialog = new ScriptPropertiesDialog{*script};
            auto subwindow = m_ui->mdiArea->addSubWindow(dialog);
            auto action = m_ui->menuWindow->addAction(dialog->windowTitle());
            action->setCheckable(true);
            connect(action, &QAction::triggered,
                    m_ui->mdiArea, [mdiArea=m_ui->mdiArea,subwindow,action](){
                        mdiArea->setActiveSubWindow(subwindow);
                        action->setChecked(subwindow->windowState().testFlag(Qt::WindowActive));
                    });
            connect(subwindow, &QMdiSubWindow::windowStateChanged,
                    action, [action](Qt::WindowStates oldState, Qt::WindowStates newState){
                        action->setChecked(newState.testFlag(Qt::WindowActive));
                    });
            connect(dialog, &QWidget::windowTitleChanged, action, &QAction::setText);
            connect(dialog, &QDialog::finished,
                    this, [&scriptPropertiesDialogs=m_scriptPropertiesDialogs,subwindow](){
                        for (auto iter = std::begin(scriptPropertiesDialogs); iter != std::end(scriptPropertiesDialogs); )
                        {
                            if (iter->second == subwindow)
                                iter = scriptPropertiesDialogs.erase(iter);
                            else
                                iter++;
                        }
                    });
            connect(dialog, &QDialog::finished,
                    subwindow, &QObject::deleteLater);
            connect(dialog, &QDialog::finished,
                    action, &QObject::deleteLater);
            m_scriptPropertiesDialogs[script] = subwindow;
            dialog->show();
        }
        break;
    }
    case ProjectTreeModel::NodeType::Font:
    {
        auto font = m_projectTreeModel->getFont(index);
        if (!font)
            break;

        if (const auto iter = m_fontPropertiesDialogs.find(font); iter != std::cend(m_fontPropertiesDialogs))
        {
            m_ui->mdiArea->setActiveSubWindow(iter->second);
        }
        else
        {
            auto dialog = new FontPropertiesDialog{*font};
            auto subwindow = m_ui->mdiArea->addSubWindow(dialog);
            auto action = m_ui->menuWindow->addAction(dialog->windowTitle());
            action->setCheckable(true);
            connect(action, &QAction::triggered,
                    m_ui->mdiArea, [mdiArea=m_ui->mdiArea,subwindow,action](){
                        mdiArea->setActiveSubWindow(subwindow);
                        action->setChecked(subwindow->windowState().testFlag(Qt::WindowActive));
                    });
            connect(subwindow, &QMdiSubWindow::windowStateChanged,
                    action, [action](Qt::WindowStates oldState, Qt::WindowStates newState){
                        action->setChecked(newState.testFlag(Qt::WindowActive));
                    });
            connect(dialog, &QWidget::windowTitleChanged, action, &QAction::setText);
            connect(dialog, &QDialog::finished,
                    this, [&fontPropertiesDialogs=m_fontPropertiesDialogs,subwindow](){
                        for (auto iter = std::begin(fontPropertiesDialogs); iter != std::end(fontPropertiesDialogs); )
                        {
                            if (iter->second == subwindow)
                                iter = fontPropertiesDialogs.erase(iter);
                            else
                                iter++;
                        }
                    });
            connect(dialog, &QDialog::finished,
                    subwindow, &QObject::deleteLater);
            connect(dialog, &QDialog::finished,
                    action, &QObject::deleteLater);
            m_fontPropertiesDialogs[font] = subwindow;
            dialog->show();
        }
        break;
    }
    }
}

void MainWindow::selectionChanged(const QModelIndex &index)
{
    switch (const auto nodeType = m_projectTreeModel->nodeType(index))
    {
    case ProjectTreeModel::NodeType::Root:
        if (index == m_projectTreeModel->spritesRoot() ||
            index == m_projectTreeModel->soundsRoot() ||
            index == m_projectTreeModel->backgroundsRoot() ||
            index == m_projectTreeModel->pathsRoot() ||
            index == m_projectTreeModel->scriptsRoot() ||
            index == m_projectTreeModel->fontsRoot())
        {
            m_ui->actionCreate->setEnabled(true);
            m_ui->actionCreateGroup->setText(tr("Cr&eate Group"));
            m_ui->actionCreateGroup->setEnabled(true);
            m_ui->actionSortByName->setEnabled(true);

            if (index == m_projectTreeModel->spritesRoot())
                m_ui->actionCreate->setText(tr("&Create Sprite"));
            else if (index == m_projectTreeModel->soundsRoot())
                m_ui->actionCreate->setText(tr("&Create Sound"));
            else if (index == m_projectTreeModel->backgroundsRoot())
                m_ui->actionCreate->setText(tr("&Create Background"));
            else if (index == m_projectTreeModel->pathsRoot())
                m_ui->actionCreate->setText(tr("&Create Path"));
            else if (index == m_projectTreeModel->scriptsRoot())
                m_ui->actionCreate->setText(tr("&Create Script"));
            else if (index == m_projectTreeModel->fontsRoot())
                m_ui->actionCreate->setText(tr("&Create Font"));
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
        m_ui->actionCreate->setEnabled(true);
        switch (nodeType)
        {
        case ProjectTreeModel::NodeType::Sprite:     m_ui->actionCreate->setText(tr("&Insert Sprite")); break;
        case ProjectTreeModel::NodeType::Sound:      m_ui->actionCreate->setText(tr("&Insert Sound")); break;
        case ProjectTreeModel::NodeType::Background: m_ui->actionCreate->setText(tr("&Insert Background")); break;
        case ProjectTreeModel::NodeType::Path:       m_ui->actionCreate->setText(tr("&Insert Path")); break;
        case ProjectTreeModel::NodeType::Script:     m_ui->actionCreate->setText(tr("&Insert Script")); break;
        case ProjectTreeModel::NodeType::Font:       m_ui->actionCreate->setText(tr("&Insert Font")); break;
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

    m_filePath = {};
    m_unsavedChanges = false;

    updateTitle();
}

void MainWindow::openFile()
{
    auto path = QFileDialog::getOpenFileName(this, tr("Load a game"), {}, tr("GMK Files (*.gmk)"));
    if (path.isEmpty())
        return;

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
    PreferencesDialog{this}.exec();
}

void MainWindow::create()
{
    const auto index = m_ui->treeView->currentIndex();
    if (!index.isValid())
        return;

    if (index == m_projectTreeModel->spritesRoot())
        createSprite();
    else if (index == m_projectTreeModel->soundsRoot())
        createSound();
    else if (index == m_projectTreeModel->backgroundsRoot())
        createBackground();
    else if (index == m_projectTreeModel->pathsRoot())
        createPath();
    else if (index == m_projectTreeModel->scriptsRoot())
        createScript();
    else if (index == m_projectTreeModel->fontsRoot())
        createFont();
    else
    {
        switch (m_projectTreeModel->nodeType(index))
        {
        case ProjectTreeModel::NodeType::Sprite:
        case ProjectTreeModel::NodeType::Sound:
        case ProjectTreeModel::NodeType::Background:
        case ProjectTreeModel::NodeType::Path:
        case ProjectTreeModel::NodeType::Script:
        case ProjectTreeModel::NodeType::Font:
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

void MainWindow::createSprite()
{
    if (!m_projectTreeModel->insertRows(m_project.sprites.size(), 1, m_projectTreeModel->spritesRoot()))
        QMessageBox::warning(this, tr("Creating Sprite failed!"), tr("Creating Sprite failed!"));
}

void MainWindow::createSound()
{
    if (!m_projectTreeModel->insertRows(m_project.sounds.size(), 1, m_projectTreeModel->soundsRoot()))
        QMessageBox::warning(this, tr("Creating Sound failed!"), tr("Creating Sound failed!"));
}

void MainWindow::createBackground()
{
    if (!m_projectTreeModel->insertRows(m_project.backgrounds.size(), 1, m_projectTreeModel->backgroundsRoot()))
        QMessageBox::warning(this, tr("Creating Background failed!"), tr("Creating Background failed!"));
}

void MainWindow::createPath()
{
    if (!m_projectTreeModel->insertRows(m_project.paths.size(), 1, m_projectTreeModel->pathsRoot()))
        QMessageBox::warning(this, tr("Creating Path failed!"), tr("Creating Path failed!"));
}

void MainWindow::createScript()
{
    if (!m_projectTreeModel->insertRows(m_project.scripts.size(), 1, m_projectTreeModel->scriptsRoot()))
        QMessageBox::warning(this, tr("Creating Script failed!"), tr("Creating Script failed!"));
}

void MainWindow::createFont()
{
    if (!m_projectTreeModel->insertRows(m_project.fonts.size(), 1, m_projectTreeModel->fontsRoot()))
        QMessageBox::warning(this, tr("Creating Font failed!"), tr("Creating Font failed!"));
}

void MainWindow::showGameInformation()
{
    GameInformationDialog{this}.exec();
}

void MainWindow::showGlobalGameSettings()
{
    GlobalGameSettingsDialog{this}.exec();
}

void MainWindow::showExtensionPackages()
{
    ExtensionPackagesDialog{this}.exec();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Qt Game Maker"),
                       tr("This <b>About-Dialog</b> is not finished."));
}

void MainWindow::rowsInserted(const QModelIndex &parent, int first, int last)
{
    m_ui->treeView->expand(parent);
    const auto index = m_projectTreeModel->index(first, 0, parent);
    m_ui->treeView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    doubleClicked(index);
}

void MainWindow::rowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    if (parent == m_projectTreeModel->spritesRoot())
    {
        for (int row = first; row <= last; row++)
        {
            if (const auto sprite = m_projectTreeModel->getSprite(m_projectTreeModel->index(row, 0, parent)))
            {
                if (const auto iter = m_spritePropertiesDialogs.find(sprite); iter != std::end(m_spritePropertiesDialogs))
                {
                    delete iter->second;
                    m_spritePropertiesDialogs.erase(iter);
                }
            }
        }
    }
    else if (parent == m_projectTreeModel->soundsRoot())
    {
        for (int row = first; row <= last; row++)
        {
            if (const auto sound = m_projectTreeModel->getSound(m_projectTreeModel->index(row, 0, parent)))
            {
                if (const auto iter = m_soundPropertiesDialogs.find(sound); iter != std::end(m_soundPropertiesDialogs))
                {
                    delete iter->second;
                    m_soundPropertiesDialogs.erase(iter);
                }
            }
        }
    }
    else if (parent == m_projectTreeModel->backgroundsRoot())
    {
        for (int row = first; row <= last; row++)
        {
            if (const auto background = m_projectTreeModel->getBackground(m_projectTreeModel->index(row, 0, parent)))
            {
                if (const auto iter = m_backgroundPropertiesDialogs.find(background); iter != std::end(m_backgroundPropertiesDialogs))
                {
                    delete iter->second;
                    m_backgroundPropertiesDialogs.erase(iter);
                }
            }
        }
    }
    else if (parent == m_projectTreeModel->pathsRoot())
    {
        for (int row = first; row <= last; row++)
        {
            if (const auto path = m_projectTreeModel->getPath(m_projectTreeModel->index(row, 0, parent)))
            {
                if (const auto iter = m_pathPropertiesDialogs.find(path); iter != std::end(m_pathPropertiesDialogs))
                {
                    delete iter->second;
                    m_pathPropertiesDialogs.erase(iter);
                }
            }
        }
    }
    else if (parent == m_projectTreeModel->scriptsRoot())
    {
        for (int row = first; row <= last; row++)
        {
            if (const auto script = m_projectTreeModel->getScript(m_projectTreeModel->index(row, 0, parent)))
            {
                if (const auto iter = m_scriptPropertiesDialogs.find(script); iter != std::end(m_scriptPropertiesDialogs))
                {
                    delete iter->second;
                    m_scriptPropertiesDialogs.erase(iter);
                }
            }
        }
    }
    else if (parent == m_projectTreeModel->fontsRoot())
    {
        for (int row = first; row <= last; row++)
        {
            if (const auto font = m_projectTreeModel->getFont(m_projectTreeModel->index(row, 0, parent)))
            {
                if (const auto iter = m_fontPropertiesDialogs.find(font); iter != std::end(m_fontPropertiesDialogs))
                {
                    delete iter->second;
                    m_fontPropertiesDialogs.erase(iter);
                }
            }
        }
    }
}

void MainWindow::modelAboutToBeReset()
{
    for (const auto &pair : m_spritePropertiesDialogs)
        delete pair.second;
    m_spritePropertiesDialogs.clear();
    for (const auto &pair : m_soundPropertiesDialogs)
        delete pair.second;
    m_soundPropertiesDialogs.clear();
    for (const auto &pair : m_backgroundPropertiesDialogs)
        delete pair.second;
    m_backgroundPropertiesDialogs.clear();
    for (const auto &pair : m_pathPropertiesDialogs)
        delete pair.second;
    m_pathPropertiesDialogs.clear();
    for (const auto &pair : m_scriptPropertiesDialogs)
        delete pair.second;
    m_scriptPropertiesDialogs.clear();
    for (const auto &pair : m_fontPropertiesDialogs)
        delete pair.second;
    m_fontPropertiesDialogs.clear();
}

void MainWindow::updateTitle()
{
    setWindowTitle(tr("%0 - Qt Gamemaker 1.0 Ultimate%1")
                       .arg(m_filePath.isEmpty() ? "<new-game>" : QFileInfo{m_filePath}.fileName())
                       .arg(m_unsavedChanges ? tr("*") : QString{}));
}
