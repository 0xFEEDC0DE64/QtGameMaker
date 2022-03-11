#include "roompropertiesdialog.h"
#include "ui_roompropertiesdialog.h"

#include <QSpinBox>
#include <QLabel>
#include <QToolButton>
#include <QMenu>
#include <QMessageBox>
#include <QDebug>

#include "projectcontainer.h"
#include "models/projecttreemodel.h"
#include "roomscene.h"
#include "editorguiutils.h"
#include "genericcodeeditordialog.h"

RoomPropertiesDialog::RoomPropertiesDialog(Room &room, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::RoomPropertiesDialog>()},
    m_room{room},
    m_projectModel{projectModel},
    m_objects{m_room.objects},
    m_scene{std::make_unique<RoomScene>(this)},
    m_creationCode{m_room.creationCode},
    m_spinBoxSnapX{new QSpinBox{this}},
    m_spinBoxSnapY{new QSpinBox{this}},
    m_menuObjects{new QMenu{this}},
    m_labelX{new QLabel{tr("x: %0").arg(0)}},
    m_labelY{new QLabel{tr("y: %0").arg(0)}}
{
    m_ui->setupUi(this);

    updateTitle();

    m_ui->roomEditWidget->setObjects(&m_objects);
    m_ui->roomEditWidget->setFixedWidth(m_room.width);
    m_ui->roomEditWidget->setFixedHeight(m_room.height);
    m_ui->roomEditWidget->setSnapX(m_room.snapX);
    m_ui->roomEditWidget->setSnapY(m_room.snapY);
    m_ui->roomEditWidget->setGridEnabled(m_room.gridEnabled);
    m_ui->roomEditWidget->setIsometricGrid(m_room.isometricGrid);
    m_ui->roomEditWidget->setProjectTreeModel(&m_projectModel);

    m_ui->graphicsView->setScene(m_scene.get());
    m_ui->graphicsView->setBackgroundBrush(makeGridBrush(16, 16, QPen{Qt::black}, Qt::white));

    {
        int index{11};

        {
            auto label = new QLabel{tr("Snap &X:"), this};
            label->setBuddy(m_spinBoxSnapX);
            m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(index++), label);
        }
        m_spinBoxSnapX->setValue(m_ui->roomEditWidget->snapX());
        m_spinBoxSnapX->setMaximumWidth(50);
        m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(index++), m_spinBoxSnapX);

        {
            auto label = new QLabel{tr("Snap &Y:"), this};
            label->setBuddy(m_spinBoxSnapY);
            m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(index++), label);
        }
        m_spinBoxSnapY->setValue(m_ui->roomEditWidget->snapY());
        m_spinBoxSnapY->setMaximumWidth(50);
        m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(index++), m_spinBoxSnapY);
    }

    m_ui->actionGridEnabled->setChecked(m_ui->roomEditWidget->gridEnabled());
    m_ui->actionIsometricGrid->setChecked(m_ui->roomEditWidget->isometricGrid());

    {
        auto toolButton = new QToolButton{this};
        toolButton->setText(tr("Show"));
        toolButton->setIcon(QIcon{":/qtgameengine/icons/find.png"});
        toolButton->setPopupMode(QToolButton::InstantPopup);
        toolButton->addAction(m_ui->actionShowObjects);
        toolButton->addAction(m_ui->actionShowTiles);
        toolButton->addAction(m_ui->actionShowBackgrounds);
        toolButton->addAction(m_ui->actionShowForegrounds);
        toolButton->addAction(m_ui->actionShowViews);
        m_ui->toolBar->addWidget(toolButton);
    }

    m_ui->toolButtonObject->setMenu(m_menuObjects);

    m_ui->lineEditObject->setMenu(m_menuObjects);

    m_ui->scrollAreaObject->setMenu(m_menuObjects);

    m_ui->scrollAreaRoom->setBackgroundRole(QPalette::Dark);

    m_labelX->setFrameStyle(QFrame::Sunken);
    m_ui->statusbar->addWidget(m_labelX, 1);
    m_labelY->setFrameStyle(QFrame::Sunken);
    m_ui->statusbar->addWidget(m_labelY, 1);

    {
        auto frame = new QFrame{this};
        frame->setFrameStyle(QFrame::Sunken);
        m_ui->statusbar->addPermanentWidget(frame, 4);
    }

    m_ui->lineEditName->setText(m_room.name);
    m_ui->lineEditCaption->setText(m_room.caption);
    m_ui->spinBoxWidth->setValue(m_ui->roomEditWidget->width());
    m_ui->spinBoxHeight->setValue(m_ui->roomEditWidget->height());
    m_ui->spinBoxSpeed->setValue(m_room.speed);
    m_ui->checkBoxPersistent->setChecked(m_room.persistent);

    connect(&m_projectModel, &ProjectTreeModel::roomNameChanged,
            this, &RoomPropertiesDialog::roomNameChanged);
    connect(&m_projectModel, &ProjectTreeModel::spritePixmapsChanged,
            this, &RoomPropertiesDialog::spritePixmapsChanged);
    connect(&m_projectModel, &ProjectTreeModel::objectNameChanged,
            this, &RoomPropertiesDialog::objectNameChanged);
    connect(&m_projectModel, &ProjectTreeModel::objectAboutToBeRemoved,
            this, &RoomPropertiesDialog::objectAboutToBeRemoved);
    connect(&m_projectModel, &ProjectTreeModel::objectSpriteNameChanged,
            this, &RoomPropertiesDialog::objectSpriteNameChanged);

    connect(m_ui->actionUndo, &QAction::triggered,
            this, &RoomPropertiesDialog::undo);
    connect(m_ui->actionClear, &QAction::triggered,
            this, &RoomPropertiesDialog::undo);
    connect(m_ui->actionShift, &QAction::triggered,
            this, &RoomPropertiesDialog::undo);
    connect(m_ui->actionSortX, &QAction::triggered,
            this, &RoomPropertiesDialog::undo);
    connect(m_ui->actionSortY, &QAction::triggered,
            this, &RoomPropertiesDialog::undo);
    connect(m_ui->actionLock, &QAction::triggered,
            this, &RoomPropertiesDialog::undo);
    connect(m_ui->actionUnlock, &QAction::triggered,
            this, &RoomPropertiesDialog::undo);

    connect(m_spinBoxSnapX, &QSpinBox::valueChanged,
            m_ui->roomEditWidget, &RoomEditWidget::setSnapX);
    connect(m_spinBoxSnapY, &QSpinBox::valueChanged,
            m_ui->roomEditWidget, &RoomEditWidget::setSnapY);
    connect(m_ui->actionGridEnabled, &QAction::toggled,
            m_ui->roomEditWidget, &RoomEditWidget::setGridEnabled);
    connect(m_ui->actionIsometricGrid, &QAction::toggled,
            m_ui->roomEditWidget, &RoomEditWidget::setIsometricGrid);

    connect(m_ui->spinBoxWidth, &QSpinBox::valueChanged,
            m_ui->roomEditWidget, &RoomEditWidget::setFixedWidth);
    connect(m_ui->spinBoxHeight, &QSpinBox::valueChanged,
            m_ui->roomEditWidget, &RoomEditWidget::setFixedHeight);

    connect(m_spinBoxSnapX, &QSpinBox::valueChanged,
            this, &RoomPropertiesDialog::changed);
    connect(m_spinBoxSnapY, &QSpinBox::valueChanged,
            this, &RoomPropertiesDialog::changed);
    connect(m_ui->actionGridEnabled, &QAction::toggled,
            this, &RoomPropertiesDialog::changed);
    connect(m_ui->actionIsometricGrid, &QAction::toggled,
            this, &RoomPropertiesDialog::changed);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &RoomPropertiesDialog::changed);
    connect(m_ui->lineEditCaption, &QLineEdit::textChanged,
            this, &RoomPropertiesDialog::changed);
    connect(m_ui->spinBoxWidth, &QSpinBox::valueChanged,
            this, &RoomPropertiesDialog::changed);
    connect(m_ui->spinBoxHeight, &QSpinBox::valueChanged,
            this, &RoomPropertiesDialog::changed);
    connect(m_ui->spinBoxSpeed, &QSpinBox::valueChanged,
            this, &RoomPropertiesDialog::changed);
    connect(m_ui->checkBoxPersistent, &QCheckBox::toggled,
            this, &RoomPropertiesDialog::changed);

    connect(m_ui->pushButtonCreationCode, &QAbstractButton::clicked,
            this, &RoomPropertiesDialog::editCreationCode);

    connect(m_ui->roomEditWidget, &RoomEditWidget::snapXChanged,
            m_spinBoxSnapX, &QSpinBox::setValue);
    connect(m_ui->roomEditWidget, &RoomEditWidget::snapYChanged,
            m_spinBoxSnapY, &QSpinBox::setValue);
    connect(m_ui->roomEditWidget, &RoomEditWidget::gridEnabledChanged,
            m_ui->actionGridEnabled, &QAction::setChecked);
    connect(m_ui->roomEditWidget, &RoomEditWidget::isometricGridChanged,
            m_ui->actionIsometricGrid, &QAction::setChecked);
    connect(m_ui->roomEditWidget, &RoomEditWidget::cursorMoved,
            this, &RoomPropertiesDialog::cursorMoved);
    connect(m_ui->roomEditWidget, &RoomEditWidget::changed,
            this, &RoomPropertiesDialog::changed);

    connect(m_menuObjects, &QMenu::aboutToShow,
            this, &RoomPropertiesDialog::objectsMenuAboutToShow);

    if (!m_projectModel.project()->objects.empty())
        setObject(m_projectModel.project()->objects.back());
}

RoomPropertiesDialog::~RoomPropertiesDialog() = default;

void RoomPropertiesDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (m_room.name != m_ui->lineEditName->text())
    {
        if (!m_projectModel.rename<Room>(m_room, m_ui->lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Room failed!"), tr("Renaming Room failed!"));
            return;
        }
    }

    m_room.caption = m_ui->lineEditCaption->text();
    m_room.width = m_ui->spinBoxWidth->value();
    m_room.height = m_ui->spinBoxHeight->value();
    m_room.speed = m_ui->spinBoxSpeed->value();
    m_room.persistent = m_ui->checkBoxPersistent->isChecked();
    m_room.creationCode = m_creationCode;
    m_room.snapX = m_spinBoxSnapX->value();
    m_room.snapY = m_spinBoxSnapY->value();
    m_room.gridEnabled = m_ui->actionGridEnabled->isChecked();
    m_room.isometricGrid = m_ui->actionIsometricGrid->isChecked();

    m_room.objects = m_objects;

    QDialog::accept();
}

void RoomPropertiesDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Room has been modified."),
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

void RoomPropertiesDialog::undo()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void RoomPropertiesDialog::clearInstances()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void RoomPropertiesDialog::shiftInstances()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void RoomPropertiesDialog::sortInstancesX()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void RoomPropertiesDialog::sortInstancesY()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void RoomPropertiesDialog::lockInstances()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void RoomPropertiesDialog::unlockInstances()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void RoomPropertiesDialog::editCreationCode()
{
    GenericCodeEditorDialog dialog{tr("Room Creation Code"), this};
    dialog.setScript(m_creationCode);
    if (dialog.exec() == QDialog::Accepted)
    {
        m_creationCode = dialog.script();
        changed();
    }
}

void RoomPropertiesDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void RoomPropertiesDialog::roomNameChanged(const Room &room)
{
    if (&room != &m_room)
        return;

    {
        QSignalBlocker blocker{m_ui->lineEditName};
        m_ui->lineEditName->setText(room.name);
    }

    updateTitle();
}

void RoomPropertiesDialog::spritePixmapsChanged(const Sprite &sprite)
{
    if (!m_selectedObject)
        return;

    if (m_selectedObject->spriteName.isEmpty())
        return;

    if (m_selectedObject->spriteName != sprite.name)
        return;

    QPixmap pixmap;
    if (!sprite.pixmaps.empty() && !sprite.pixmaps.front().isNull())
        pixmap = sprite.pixmaps.front();
    m_ui->labelObjectPreview->setPixmap(std::move(pixmap));
}

void RoomPropertiesDialog::objectNameChanged(const Object &object)
{
    if (!m_selectedObject)
        return;

    if (&object != m_selectedObject)
        return;

    m_ui->lineEditObject->setText(object.name);
}

void RoomPropertiesDialog::objectAboutToBeRemoved(const Object &object)
{
    if (!m_selectedObject)
        return;

    if (&object != m_selectedObject)
        return;

    m_selectedObject = nullptr;
    m_ui->lineEditObject->clear();
    m_ui->labelObjectPreview->setPixmap({});
    m_ui->roomEditWidget->setSelectedObject(nullptr);
}

void RoomPropertiesDialog::objectSpriteNameChanged(const Object &object)
{
    if (!m_selectedObject)
        return;

    if (&object != m_selectedObject)
        return;

    QPixmap pixmap;
    if (!object.spriteName.isEmpty())
    {
        const auto iter = std::find_if(std::cbegin(m_projectModel.project()->sprites), std::cend(m_projectModel.project()->sprites),
                                       [&object](const auto &sprite){ return object.spriteName == sprite.name; });
        if (iter == std::cend(m_projectModel.project()->sprites))
            qWarning() << "invalid sprite" << object.spriteName;
        else if (!iter->pixmaps.empty() && !iter->pixmaps.front().isNull())
            pixmap = iter->pixmaps.front();
    }
    m_ui->labelObjectPreview->setPixmap(std::move(pixmap));
}

void RoomPropertiesDialog::objectsMenuAboutToShow()
{
    m_menuObjects->clear();
    for (const auto &object : m_projectModel.project()->objects)
    {
        QIcon icon;
        if (!object.spriteName.isEmpty())
        {
            const auto iter = std::find_if(std::cbegin(m_projectModel.project()->sprites), std::cend(m_projectModel.project()->sprites),
                                           [&object](const auto &sprite){ return object.spriteName == sprite.name; });
            if (iter == std::cend(m_projectModel.project()->sprites))
                qWarning() << "invalid sprite" << object.spriteName;
            else if (!iter->pixmaps.empty() && !iter->pixmaps.front().isNull())
                icon = iter->pixmaps.front();
        }
        m_menuObjects->addAction(icon, object.name, this,
                                 [&object,this](){ setObject(object); });
    }
}

void RoomPropertiesDialog::cursorMoved(const QPoint &point)
{
    m_labelX->setText(tr("X: %0").arg(point.x()));
    m_labelY->setText(tr("Y: %0").arg(point.y()));
}

void RoomPropertiesDialog::setObject(const Object &object)
{
    m_selectedObject = &object;
    QPixmap pixmap;
    if (!object.spriteName.isEmpty())
    {
        const auto iter = std::find_if(std::cbegin(m_projectModel.project()->sprites), std::cend(m_projectModel.project()->sprites),
                                       [&object](const auto &sprite){ return object.spriteName == sprite.name; });
        if (iter == std::cend(m_projectModel.project()->sprites))
            qWarning() << "invalid sprite" << object.spriteName;
        else if (!iter->pixmaps.empty() && !iter->pixmaps.front().isNull())
            pixmap = iter->pixmaps.front();
    }
    m_ui->labelObjectPreview->setPixmap(std::move(pixmap));
    m_ui->lineEditObject->setText(object.name);
    m_ui->roomEditWidget->setSelectedObject(&object);
}

void RoomPropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Room Properties: %0%1")
                       .arg(m_room.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}
