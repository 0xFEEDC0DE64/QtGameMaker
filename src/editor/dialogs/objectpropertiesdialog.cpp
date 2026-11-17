#include "objectpropertiesdialog.h"
#include "ui_objectpropertiesdialog.h"

#include <QMenu>
#include <QDebug>
#include <QMessageBox>
#include <QScopedValueRollback>

#include <algorithm>

#include "mainwindow.h"
#include "models/projecttreemodel.h"
#include "models/objecteventsmodel.h"
#include "addeventdialog.h"
#include "editorguiutils.h"
#include "editorsettings.h"
#include "widgets/resourcemenuadapter.h"

ObjectPropertiesDialog::ObjectPropertiesDialog(Object &object, ProjectTreeModel &projectModel, MainWindow &mainWindow) :
    QDialog{&mainWindow},
    m_ui{std::make_unique<Ui::ObjectPropertiesDialog>()},
    m_object{object},
    m_projectModel{projectModel},
    m_mainWindow{mainWindow},
    m_events{m_object.events},
    m_collisionEvents{m_object.collisionEvents},
    m_eventsModel{std::make_unique<ObjectEventsModel>(m_events, m_collisionEvents)},
    m_menuSprites{new QMenu{this}},
    m_menuMaskSprites{new QMenu{this}},
    m_spriteName{object.spriteName},
    m_maskSpriteName{object.maskSpriteName}
{
    m_ui->setupUi(this);

    updateTitle();

    improveButtonBox(m_ui->buttonBox);

    m_ui->widgetParent->setProjectModel(&m_projectModel);
    m_ui->widgetParent->setForbiddenObjectName(m_object.name);
    m_ui->actionsWidget->setProjectModel(&m_projectModel);

    m_ui->lineEditName->setText(m_object.name);
    m_ui->lineEditSprite->setText(m_spriteName.isEmpty() ? tr("<no sprite>") : m_spriteName);
    m_ui->widgetParent->setEmptySelectionText(tr("<no parent>"));
    if (QSignalBlocker blocker{m_ui->widgetParent}; object.parentName.isEmpty())
        m_ui->widgetParent->clearObject();
    else
        m_ui->widgetParent->setObject(object.parentName);
    m_ui->lineEditMask->setText(m_maskSpriteName.isEmpty() ? tr("<same as sprite>") : m_maskSpriteName);
    updateSpritePreview();
    m_ui->toolButtonSprite->setMenu(m_menuSprites);
    m_ui->toolButtonMask->setMenu(m_menuMaskSprites);
    m_ui->checkBoxVisible->setChecked(m_object.visible);
    m_ui->checkBoxSolid->setChecked(m_object.solid);
    m_ui->spinBoxDepth->setValue(m_object.depth);
    m_ui->checkBoxPersistent->setChecked(m_object.persistent);

    m_ui->lineEditSprite->setMenu(m_menuSprites);
    m_ui->lineEditMask->setMenu(m_menuMaskSprites);

    m_ui->listViewEvents->setModel(m_eventsModel.get());

    connect(&m_projectModel, &ProjectTreeModel::objectNameChanged,
            this, &ObjectPropertiesDialog::objectNameChanged);
    connect(&m_projectModel, &ProjectTreeModel::spriteNameChanged,
            this, &ObjectPropertiesDialog::spriteNameChanged);
    connect(&m_projectModel, &ProjectTreeModel::objectAboutToBeRemoved,
            this, &ObjectPropertiesDialog::objectAboutToBeRemoved);
    connect(&m_projectModel, &ProjectTreeModel::spriteAboutToBeRemoved,
            this, &ObjectPropertiesDialog::spriteAboutToBeRemoved);
    connect(&m_projectModel, &ProjectTreeModel::spritePixmapsChanged,
            this, &ObjectPropertiesDialog::spritePixmapsChanged);

    connect(m_eventsModel.get(), &QAbstractItemModel::modelReset,
            this, &ObjectPropertiesDialog::changed);
    connect(m_eventsModel.get(), &QAbstractItemModel::rowsInserted,
            this, &ObjectPropertiesDialog::rowsInserted);
    connect(m_eventsModel.get(), &QAbstractItemModel::rowsInserted,
            this, &ObjectPropertiesDialog::changed);
    connect(m_eventsModel.get(), &QAbstractItemModel::dataChanged,
            this, &ObjectPropertiesDialog::changed);
    connect(m_eventsModel.get(), &QAbstractItemModel::rowsRemoved,
            this, &ObjectPropertiesDialog::changed);

    connect(m_ui->pushButtonNewSprite, &QAbstractButton::clicked,
            this, &ObjectPropertiesDialog::newSprite);
    connect(m_ui->pushButtonEditSprite, &QAbstractButton::clicked,
            this, &ObjectPropertiesDialog::editSprite);
    connect(m_ui->pushButtonShowInformation, &QAbstractButton::clicked,
            this, &ObjectPropertiesDialog::showInformation);
    connect(m_ui->pushButtonAddEvent, &QAbstractButton::clicked,
            this, &ObjectPropertiesDialog::addEvent);
    connect(m_ui->pushButtonDeleteEvent, &QAbstractButton::clicked,
            this, &ObjectPropertiesDialog::deleteEvent);
    connect(m_ui->pushButtonChangeEvent, &QAbstractButton::clicked,
            this, &ObjectPropertiesDialog::replaceEvent);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &ObjectPropertiesDialog::changed);
    connect(m_ui->checkBoxVisible, &QCheckBox::toggled,
            this, &ObjectPropertiesDialog::changed);
    connect(m_ui->checkBoxSolid, &QCheckBox::toggled,
            this, &ObjectPropertiesDialog::changed);
    connect(m_ui->spinBoxDepth, &QSpinBox::valueChanged,
            this, &ObjectPropertiesDialog::changed);
    connect(m_ui->checkBoxPersistent, &QCheckBox::toggled,
            this, &ObjectPropertiesDialog::changed);

    m_spriteMenuAdapter = std::make_unique<ResourceMenuAdapter>(
        m_projectModel, m_projectModel.rootFor<Sprite>(), *m_menuSprites,
        [this](const QModelIndex &index) {
            if (const Sprite *sprite = m_projectModel.get<Sprite>(index)) setSprite(*sprite);
        },
        [this](QMenu &menu) { menu.addAction(tr("<no sprite>"), this, &ObjectPropertiesDialog::clearSprite); });
    m_maskMenuAdapter = std::make_unique<ResourceMenuAdapter>(
        m_projectModel, m_projectModel.rootFor<Sprite>(), *m_menuMaskSprites,
        [this](const QModelIndex &index) {
            if (const Sprite *sprite = m_projectModel.get<Sprite>(index)) setMaskSprite(*sprite);
        },
        [this](QMenu &menu) { menu.addAction(tr("<same as sprite>"), this, &ObjectPropertiesDialog::clearMaskSprite); });

    connect(m_ui->widgetParent, &ObjectSelectorWidget::changed,
            this, &ObjectPropertiesDialog::changed);

    connect(m_ui->listViewEvents->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ObjectPropertiesDialog::currentEventChanged);

    connect(m_ui->listViewEvents, &QListView::customContextMenuRequested,
            this, &ObjectPropertiesDialog::eventsContextMenuRequested);

    connect(m_ui->actionsWidget, &ActionsContainerWidget::changed,
            this, &ObjectPropertiesDialog::changed);

    connect(&mainWindow.settings(), &EditorSettings::advancedModeChanged,
            this, &ObjectPropertiesDialog::advancedModeChanged);
    advancedModeChanged(mainWindow.settings().advancedMode());
}

ObjectPropertiesDialog::~ObjectPropertiesDialog() = default;

void ObjectPropertiesDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (applyChanges()) QDialog::accept();
}

bool ObjectPropertiesDialog::applyChanges()
{
    if (!m_unsavedChanges) return true;

    if (m_object.name != m_ui->lineEditName->text())
    {
        if (!m_projectModel.rename<Object>(m_object, m_ui->lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Object failed!"), tr("Renaming Object failed!"));
            return false;
        }
    }

    if (!m_projectModel.setObjectSpriteName(m_object, QString{m_spriteName}))
    {
        QMessageBox::critical(this, tr("Setting Object Sprite failed!"), tr("Setting Object Sprite failed!"));
        return false;
    }

    m_object.visible = m_ui->checkBoxVisible->isChecked();
    m_object.solid = m_ui->checkBoxSolid->isChecked();
    m_object.depth = m_ui->spinBoxDepth->value();
    m_object.persistent = m_ui->checkBoxPersistent->isChecked();
    m_object.parentName = m_ui->widgetParent->objectName();
    m_object.maskSpriteName = m_maskSpriteName;
    m_object.events = m_events;
    m_object.collisionEvents = m_collisionEvents;

    m_unsavedChanges = false;
    setWindowModified(false);
    updateTitle();
    return true;
}

void ObjectPropertiesDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Object has been modified."),
        tr("Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save
    );
    switch (result)
    {
    case QMessageBox::Save:
        accept();
        return;
    case QMessageBox::Discard:
        QDialog::reject();
        return;
    case QMessageBox::Cancel:
        return;
    default:
        qWarning() << "unexpected dialog result" << result;
    }
}

void ObjectPropertiesDialog::newSprite()
{
    auto sprite = m_projectModel.create<Sprite>();
    if (!sprite)
    {
        QMessageBox::warning(this, tr("Creating Sprite failed"), tr("Creating Sprite failed"));
        return;
    }
    setSprite(*sprite);
}

void ObjectPropertiesDialog::editSprite()
{
    auto &sprites = m_projectModel.project()->sprites;
    const auto iter = std::find_if(std::begin(sprites), std::end(sprites),
                                   [&](const Sprite &sprite){ return sprite.name == m_spriteName; });
    if (iter == std::end(sprites))
    {
        qCritical() << "sprite" << m_spriteName << "not found";
        return;
    }

    m_mainWindow.openPropertiesWindowFor(*iter);
}

void ObjectPropertiesDialog::showInformation()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ObjectPropertiesDialog::addEvent()
{
    AddEventDialog dialog{m_projectModel, m_mainWindow, this};
    if (dialog.exec() == QDialog::Accepted)
        if (const auto &eventType = dialog.eventType())
            if (!m_eventsModel->addEvent(*eventType))
                QMessageBox::warning(this, tr("Could not add Event!"), tr("Could not add Event!"));
}

void ObjectPropertiesDialog::deleteEvent()
{
    const auto index = m_ui->listViewEvents->currentIndex();
    if (!index.isValid())
        return;

    if (QMessageBox::question(this, tr("Confirm"), tr("Are you sure, that you really want to remove the event with all its actions?")) != QMessageBox::Yes)
        return;

    if (!m_eventsModel->removeRow(index.row()))
        QMessageBox::warning(this, tr("Could not delete Event!"), tr("Could not delete Event!"));
}

void ObjectPropertiesDialog::replaceEvent()
{
    const auto index = m_ui->listViewEvents->currentIndex();
    if (!index.isValid())
        return;

    const auto event = m_eventsModel->getEvent(index);
    if (!event)
        return;

    std::variant<Object::EventType, QString> x = event->first;

    AddEventDialog dialog{m_projectModel, m_mainWindow, this};
    if (dialog.exec() == QDialog::Accepted)
        if (const auto &eventType = dialog.eventType())
        if (!m_eventsModel->changeEvent(event->first, *eventType))
            QMessageBox::warning(this, tr("Could not change Event!"), tr("Could not change Event!"));
}

void ObjectPropertiesDialog::duplicateEvent()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ObjectPropertiesDialog::changed()
{
    if (m_updatingExternalReferences) return;

    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        setWindowModified(true);
        updateTitle();
    }
}

void ObjectPropertiesDialog::objectNameChanged(const Object &object, const QString &oldName)
{
    QScopedValueRollback updatingReferences{m_updatingExternalReferences, true};

    const auto renameTargets = [&object, &oldName](ActionsContainer &actions) {
        for (Action &action : actions)
            std::visit([&](auto &value) {
                if (auto *target = std::get_if<AppliesToObject>(&value.appliesTo);
                    target && target->objectName == oldName)
                    target->objectName = object.name;
            }, action);
    };
    for (auto &[event, actions] : m_events) renameTargets(actions);
    for (auto &[event, actions] : m_collisionEvents) renameTargets(actions);
    if (m_collisionEvents.contains(oldName))
    {
        auto *selection = m_ui->listViewEvents->selectionModel();
        auto selectedEvent = selection->currentIndex().isValid()
            ? m_eventsModel->getEvent(selection->currentIndex())
            : ObjectEventsModel::get_event_result_t{};
        auto selectedType = selectedEvent
            ? std::optional{selectedEvent->first}
            : std::optional<std::variant<Object::EventType, QString>>{};
        if (selectedType)
            if (auto *target = std::get_if<QString>(&*selectedType);
                target && *target == oldName)
                *target = object.name;

        m_ui->actionsWidget->setActionsContainer(nullptr);
        m_eventsModel->changeEvent(oldName, object.name);

        QModelIndex restoredIndex;
        if (selectedType)
            for (int row = 0; row < m_eventsModel->rowCount({}); ++row)
                if (const auto event = m_eventsModel->getEvent(row);
                    event && event->first == *selectedType)
                {
                    restoredIndex = m_eventsModel->index(row, 0);
                    break;
                }

        if (restoredIndex.isValid())
            selection->setCurrentIndex(restoredIndex, QItemSelectionModel::ClearAndSelect);
        else
        {
            selection->clearSelection();
            selection->clearCurrentIndex();
        }
        currentEventChanged(restoredIndex);
    }

    if (&object == &m_object)
    {
        {
            QSignalBlocker blocker{m_ui->lineEditName};
            m_ui->lineEditName->setText(object.name);
        }

        updateTitle();
    }

    if (QSignalBlocker blocker{m_ui->widgetParent};
        !m_ui->widgetParent->objectName().isEmpty() && m_ui->widgetParent->objectName() == oldName)
        m_ui->widgetParent->setObject(object.name);
}

void ObjectPropertiesDialog::spriteNameChanged(const Sprite &sprite, const QString &oldName)
{
    if (!m_spriteName.isEmpty() && m_spriteName == oldName)
    {
        m_spriteName = sprite.name;
        {
            QSignalBlocker blocker{m_ui->lineEditSprite};
            m_ui->lineEditSprite->setText(sprite.name);
        }
        updateSpritePreview();
    }

    if (!m_maskSpriteName.isEmpty() && m_maskSpriteName == oldName)
    {
        m_maskSpriteName = sprite.name;
        {
            QSignalBlocker blocker{m_ui->lineEditMask};
            m_ui->lineEditMask->setText(sprite.name);
        }
    }
}

void ObjectPropertiesDialog::objectAboutToBeRemoved(const Object &object)
{
    QScopedValueRollback updatingReferences{m_updatingExternalReferences, true};

    if (QSignalBlocker blocker{m_ui->widgetParent};
        !m_ui->widgetParent->objectName().isEmpty() && m_ui->widgetParent->objectName() == object.name)
        m_ui->widgetParent->clearObject();

    const auto clearTargets = [&object](ActionsContainer &actions) {
        for (Action &action : actions)
            std::visit([&object](auto &value) {
                if (auto *target = std::get_if<AppliesToObject>(&value.appliesTo);
                    target && target->objectName == object.name)
                    target->objectName.clear();
            }, action);
    };
    for (auto &[event, actions] : m_events)
        clearTargets(actions);
    for (auto &[event, actions] : m_collisionEvents)
        clearTargets(actions);
    if (m_collisionEvents.contains(object.name))
    {
        m_ui->actionsWidget->setActionsContainer(nullptr);
        m_eventsModel->removeEvent(object.name);
    }
}

void ObjectPropertiesDialog::spriteAboutToBeRemoved(const Sprite &sprite)
{
    if (!m_spriteName.isEmpty() && m_spriteName == sprite.name)
    {
        m_spriteName.clear();
        {
            QSignalBlocker blocker{m_ui->lineEditSprite};
            m_ui->lineEditSprite->setText(tr("<no sprite>"));
        }
        m_ui->labelSpritePreview->setPixmap(QPixmap{});
    }

    if (!m_maskSpriteName.isEmpty() && m_maskSpriteName == sprite.name)
    {
        m_maskSpriteName.clear();
        {
            QSignalBlocker blocker{m_ui->lineEditMask};
            m_ui->lineEditMask->setText(tr("<same as sprite>"));
        }
    }
}

void ObjectPropertiesDialog::spritePixmapsChanged(const Sprite &sprite)
{
    if (m_spriteName.isEmpty())
        return;

    if (m_spriteName != sprite.name)
        return;

    updateSpritePreview(sprite.pixmaps);
}

void ObjectPropertiesDialog::currentEventChanged(const QModelIndex &index)
{
    if (index.isValid())
    {
        if (auto event = m_eventsModel->getEvent(index))
            m_ui->actionsWidget->setActionsContainer(&event->second.get());
        else
            goto none;
    }
    else
    {
none:
        m_ui->actionsWidget->setActionsContainer(nullptr);
    }
}

void ObjectPropertiesDialog::eventsContextMenuRequested(const QPoint &pos)
{
    const auto index = m_ui->listViewEvents->indexAt(pos);
    auto event = index.isValid() ? m_eventsModel->getEvent(index) : std::nullopt;

    QMenu menu{this};
    menu.addAction(tr("&Add Event"), this, &ObjectPropertiesDialog::addEvent);
    menu.addAction(tr("&Change Event"), this, &ObjectPropertiesDialog::replaceEvent)->setEnabled(event.has_value());
    menu.addAction(tr("&Duplicate Event"), this, &ObjectPropertiesDialog::duplicateEvent)->setEnabled(event.has_value());
    menu.addAction(tr("D&elete Event"), this, &ObjectPropertiesDialog::deleteEvent)->setEnabled(event.has_value());
    menu.exec(m_ui->listViewEvents->viewport()->mapToGlobal(pos));
}

void ObjectPropertiesDialog::rowsInserted(const QModelIndex &parent, int first)
{
    if (m_updatingExternalReferences) return;
    m_ui->listViewEvents->setCurrentIndex(m_eventsModel->index(first, 0, parent));
}

void ObjectPropertiesDialog::clearSprite()
{
    m_spriteName.clear();
    m_ui->labelSpritePreview->setPixmap(QPixmap{});
    m_ui->lineEditSprite->setText(tr("<no sprite>"));
    changed();
}

void ObjectPropertiesDialog::setSprite(const Sprite &sprite)
{
    if (m_spriteName == sprite.name)
        return;

    m_spriteName = sprite.name;
    updateSpritePreview(sprite.pixmaps);
    m_ui->lineEditSprite->setText(sprite.name);
    changed();
}

void ObjectPropertiesDialog::clearMaskSprite()
{
    m_maskSpriteName.clear();
    m_ui->lineEditMask->setText(tr("<same as sprite>"));
    changed();
}

void ObjectPropertiesDialog::setMaskSprite(const Sprite &sprite)
{
    m_maskSpriteName = sprite.name;
    m_ui->lineEditMask->setText(sprite.name);
    changed();
}

void ObjectPropertiesDialog::advancedModeChanged(bool advancedMode)
{
    for (QWidget *widgets[] {
             m_ui->widgetDetails,
             m_ui->pushButtonShowInformation
         }; QWidget *widget : widgets)
        widget->setVisible(advancedMode);
}

void ObjectPropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Object Properties: %0%1")
                       .arg(m_object.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}

void ObjectPropertiesDialog::updateSpritePreview()
{
    if (m_spriteName.isEmpty())
        m_ui->labelSpritePreview->setPixmap(QPixmap{});
    else
    {
        const auto &sprites = m_projectModel.project()->sprites;
        const auto iter = std::find_if(std::cbegin(sprites), std::cend(sprites),
                                       [&](const Sprite &sprite){ return sprite.name == m_spriteName; });
        if (iter == std::cend(sprites))
            qWarning() << "sprite" << m_spriteName << "not found";
        else
            updateSpritePreview(iter->pixmaps);
    }
}

void ObjectPropertiesDialog::updateSpritePreview(const std::vector<QPixmap> &pixmaps)
{
    m_ui->labelSpritePreview->setPixmap(pixmaps.empty() ? QPixmap{} : pixmaps.front().scaled(32, 32, Qt::KeepAspectRatio));
}
