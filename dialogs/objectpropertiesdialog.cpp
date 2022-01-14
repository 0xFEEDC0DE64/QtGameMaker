#include "objectpropertiesdialog.h"
#include "ui_objectpropertiesdialog.h"

#include <QMenu>
#include <QDebug>
#include <QMessageBox>

#include <algorithm>

#include "models/projecttreemodel.h"
#include "models/objecteventsmodel.h"
#include "addeventdialog.h"

ObjectPropertiesDialog::ObjectPropertiesDialog(Object &object, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ObjectPropertiesDialog>()},
    m_object{object},
    m_projectModel{projectModel},
    m_events{m_object.events},
    m_eventsModel{std::make_unique<ObjectEventsModel>(m_events)},
    m_spritesMenu{new QMenu{m_ui->toolButtonSprite}},
    m_spriteName{object.spriteName}
{
    m_ui->setupUi(this);

    updateTitle();

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    m_ui->lineEditName->setText(m_object.name);
    m_ui->lineEditSprite->setText(m_spriteName.isEmpty() ? tr("<no sprite>") : m_spriteName);
    updateSpritePreview();
    m_ui->toolButtonSprite->setMenu(m_spritesMenu);
    m_ui->checkBoxVisible->setChecked(m_object.visible);
    m_ui->checkBoxSolid->setChecked(m_object.solid);
    m_ui->spinBoxDepth->setValue(m_object.depth);
    m_ui->checkBoxPersistent->setChecked(m_object.persistent);

    m_ui->listViewEvents->setModel(m_eventsModel.get());

    connect(&m_projectModel, &ProjectTreeModel::objectNameChanged,
            this, &ObjectPropertiesDialog::objectNameChanged);
    connect(&m_projectModel, &ProjectTreeModel::spriteNameChanged,
            this, &ObjectPropertiesDialog::spriteNameChanged);
    connect(&m_projectModel, &ProjectTreeModel::spriteAboutToBeRemoved,
            this, &ObjectPropertiesDialog::spriteAboutToBeRemoved);
    connect(&m_projectModel, &ProjectTreeModel::spritePixmapsChanged,
            this, &ObjectPropertiesDialog::spritePixmapsChanged);

    connect(m_eventsModel.get(), &QAbstractItemModel::modelReset,
            this, &ObjectPropertiesDialog::changed);
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

    connect(m_spritesMenu, &QMenu::aboutToShow,
            this, &ObjectPropertiesDialog::spritesMenuAboutToShow);

    connect(m_ui->listViewEvents->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ObjectPropertiesDialog::currentEventChanged);

    connect(m_ui->listViewEvents, &QListView::customContextMenuRequested,
            this, &ObjectPropertiesDialog::eventsContextMenuRequested);

    connect(m_ui->actionsWidget, &ActionsContainerWidget::changed,
            this, &ObjectPropertiesDialog::changed);
}

ObjectPropertiesDialog::~ObjectPropertiesDialog() = default;

void ObjectPropertiesDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (m_object.name != m_ui->lineEditName->text())
    {
        if (!m_projectModel.rename<Object>(m_object, m_ui->lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Object failed!"), tr("Renaming Object failed!"));
            return;
        }
    }

    m_object.spriteName = m_spriteName;
    m_object.visible = m_ui->checkBoxVisible->isChecked();
    m_object.solid = m_ui->checkBoxSolid->isChecked();
    m_object.depth = m_ui->spinBoxDepth->value();
    m_object.persistent = m_ui->checkBoxPersistent->isChecked();
    m_object.events = std::move(m_events);

    QDialog::accept();
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
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ObjectPropertiesDialog::editSprite()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ObjectPropertiesDialog::showInformation()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ObjectPropertiesDialog::addEvent()
{
    AddEventDialog dialog{this};
    if (dialog.exec() == QDialog::Accepted)
    {
        if (!m_eventsModel->addEvent(dialog.eventType()))
            QMessageBox::warning(this, tr("Could not add Event!"), tr("Could not add Event!"));
    }
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

    AddEventDialog dialog{this};
    if (dialog.exec() == QDialog::Accepted)
    {
        if (!m_eventsModel->changeEvent(event->first, dialog.eventType()))
            QMessageBox::warning(this, tr("Could not change Event!"), tr("Could not change Event!"));
    }
}

void ObjectPropertiesDialog::duplicateEvent()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ObjectPropertiesDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void ObjectPropertiesDialog::objectNameChanged(const Object &object)
{
    if (&object != &m_object)
        return;

    {
        QSignalBlocker blocker{m_ui->lineEditName};
        m_ui->lineEditName->setText(object.name);
    }

    updateTitle();
}

void ObjectPropertiesDialog::spriteNameChanged(const Sprite &sprite, const QString &oldName)
{
    if (m_spriteName.isEmpty())
        return;

    if (m_spriteName != oldName)
        return;

    m_spriteName = sprite.name;
    {
        QSignalBlocker blocker{m_ui->lineEditSprite};
        m_ui->lineEditSprite->setText(sprite.name);
    }
    updateSpritePreview();
}

void ObjectPropertiesDialog::spriteAboutToBeRemoved(const Sprite &sprite)
{
    if (m_spriteName.isEmpty())
        return;

    if (m_spriteName != sprite.name)
        return;

    m_spriteName.clear();
    {
        QSignalBlocker blocker{m_ui->lineEditSprite};
        m_ui->lineEditSprite->setText(tr("<no sprite>"));
    }
    m_ui->labelSpritePreview->setPixmap(QPixmap{});
}

void ObjectPropertiesDialog::spritePixmapsChanged(const Sprite &sprite)
{
    if (m_spriteName.isEmpty())
        return;

    if (m_spriteName != sprite.name)
        return;

    updateSpritePreview(sprite.pixmaps);
}

void ObjectPropertiesDialog::spritesMenuAboutToShow()
{
    m_spritesMenu->clear();
    m_spritesMenu->addAction(tr("<no sprite>"), this, &ObjectPropertiesDialog::clearSprite);
    for (const auto &sprite : m_projectModel.project()->sprites)
        m_spritesMenu->addAction(sprite.pixmaps.empty() ? QPixmap{} : sprite.pixmaps.front(),
                                 sprite.name,
                                 this,
                                 [&sprite,this](){ setSprite(sprite); });
}

void ObjectPropertiesDialog::currentEventChanged(const QModelIndex &index)
{
    if (index.isValid())
    {
        if (auto event = m_eventsModel->getEvent(index))
            m_ui->actionsWidget->setActionsContainer(&event->second);
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
    auto event = index.isValid() ? m_eventsModel->getEvent(index) : nullptr;

    QMenu menu{this};
    menu.addAction(tr("&Add Event"), this, &ObjectPropertiesDialog::addEvent);
    menu.addAction(tr("&Change Event"), this, &ObjectPropertiesDialog::replaceEvent)->setEnabled(event);
    menu.addAction(tr("&Duplicate Event"), this, &ObjectPropertiesDialog::duplicateEvent)->setEnabled(event);
    menu.addAction(tr("D&elete Event"), this, &ObjectPropertiesDialog::deleteEvent)->setEnabled(event);
    menu.exec(m_ui->listViewEvents->viewport()->mapToGlobal(pos));
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
    m_spriteName = sprite.name;
    updateSpritePreview(sprite.pixmaps);
    m_ui->lineEditSprite->setText(sprite.name);
    changed();
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
    m_ui->labelSpritePreview->setPixmap(pixmaps.empty() ? QPixmap{} : pixmaps.front().scaled(32, m_ui->labelSpritePreview->height(), Qt::KeepAspectRatio));
}
