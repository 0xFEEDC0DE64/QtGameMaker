#include "objectpropertiesdialog.h"
#include "ui_objectpropertiesdialog.h"

#include <QMenu>
#include <QDebug>
#include <QMessageBox>

#include <algorithm>

#include "projecttreemodel.h"
#include "objecteventsmodel.h"
#include "objectactionsmodel.h"
#include "addeventdialog.h"

ObjectPropertiesDialog::ObjectPropertiesDialog(Object &object, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ObjectPropertiesDialog>()},
    m_object{object},
    m_projectModel{projectModel},
    m_events{m_object.events},
    m_eventsModel{std::make_unique<ObjectEventsModel>(m_events)},
    m_actionsModel{std::make_unique<ObjectActionsModel>()},
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
    if (!m_spriteName.isEmpty())
    {
        const auto &sprites = m_projectModel.project()->sprites;
        const auto iter = std::find_if(std::cbegin(sprites), std::cend(sprites),
                                       [&](const Sprite &sprite){ return sprite.name == m_spriteName; });
        if (iter == std::cend(sprites))
            qWarning() << "sprite" << m_spriteName << "not found";
        else
            m_ui->labelSpritePreview->setPixmap(iter->pixmaps.empty() ? QPixmap{} : iter->pixmaps.front().scaled(m_ui->labelSpritePreview->size(), Qt::KeepAspectRatio));
    }
    m_ui->toolButtonSprite->setMenu(m_spritesMenu);
    m_ui->checkBoxVisible->setChecked(m_object.visible);
    m_ui->checkBoxSolid->setChecked(m_object.solid);
    m_ui->spinBoxDepth->setValue(m_object.depth);
    m_ui->checkBoxPersistent->setChecked(m_object.persistent);

    m_ui->listViewEvents->setModel(m_eventsModel.get());

    connect(&m_projectModel, &ProjectTreeModel::objectNameChanged,
            this, &ObjectPropertiesDialog::objectNameChanged);

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
        if (!m_events.contains(dialog.eventType()))
        {
            m_eventsModel->beginResetModel();
            m_events[dialog.eventType()];
            m_eventsModel->endResetModel();
            m_unsavedChanges = true;
        }
    }
}

void ObjectPropertiesDialog::deleteEvent()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ObjectPropertiesDialog::replaceEvent()
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

void ObjectPropertiesDialog::clearSprite()
{
    m_ui->labelSpritePreview->setPixmap(QPixmap{});
    m_ui->lineEditSprite->setText(tr("<no sprite>"));
    m_spriteName = QString{};
    changed();
}

void ObjectPropertiesDialog::setSprite(const Sprite &sprite)
{
    m_ui->labelSpritePreview->setPixmap(sprite.pixmaps.empty() ? QPixmap{} : sprite.pixmaps.front().scaled(m_ui->labelSpritePreview->size(), Qt::KeepAspectRatio));
    m_ui->lineEditSprite->setText(sprite.name);
    m_spriteName = sprite.name;
    changed();
}

void ObjectPropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Object Properties: %0%1")
                       .arg(m_object.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}
