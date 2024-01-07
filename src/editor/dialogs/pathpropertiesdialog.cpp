#include "pathpropertiesdialog.h"
#include "ui_pathpropertiesdialog.h"

#include <QSpinBox>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QDebug>
#include <QMessageBox>
#include <QSignalBlocker>

#include "projectcontainer.h"
#include "models/projecttreemodel.h"
#include "models/pathpointsmodel.h"
#include "mainwindow.h"

PathPropertiesDialog::PathPropertiesDialog(Path &path, ProjectTreeModel &projectModel, MainWindow &mainWindow) :
    QDialog{&mainWindow},
    m_ui{std::make_unique<Ui::PathPropertiesDialog>()},
    m_path{path},
    m_projectModel{projectModel},
    m_points{m_path.points},
    m_pointsModel{std::make_unique<PathPointsModel>(m_points, this)},
    m_spinBoxSnapX{new QSpinBox{this}},
    m_spinBoxSnapY{new QSpinBox{this}},
    m_menuRooms{new QMenu{this}},
    m_labelX{new QLabel{tr("x: %0").arg(0)}},
    m_labelY{new QLabel{tr("y: %0").arg(0)}},
    m_labelArea{new QLabel{tr("Area: (%0,%1)->(%2,%3)").arg(0).arg(0).arg(0).arg(0)}}
{
    m_ui->setupUi(this);

    m_ui->pathPointsWidget->setPoints(&m_points);
    m_ui->pathPointsWidget->setSnapX(m_path.snapX);
    m_ui->pathPointsWidget->setSnapY(m_path.snapY);
    m_ui->pathPointsWidget->setGridEnabled(m_path.gridEnabled);

    updateTitle();

    {
        int index{18};

        {
            auto label = new QLabel{tr("Snap &X:"), this};
            label->setBuddy(m_spinBoxSnapX);
            m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(index++), label);
        }
        m_spinBoxSnapX->setValue(m_ui->pathPointsWidget->snapX());
        m_spinBoxSnapX->setMaximumWidth(50);
        m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(index++), m_spinBoxSnapX);

        {
            auto label = new QLabel{tr("Snap &Y:"), this};
            label->setBuddy(m_spinBoxSnapY);
            m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(index++), label);
        }
        m_spinBoxSnapY->setValue(m_ui->pathPointsWidget->snapY());
        m_spinBoxSnapY->setMaximumWidth(50);
        m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(index++), m_spinBoxSnapY);
    }

    m_ui->actionGridEnabled->setChecked(m_ui->pathPointsWidget->gridEnabled());

    {
        auto toolButton = new QToolButton{this};
        toolButton->setText(tr("Show"));
        toolButton->setWhatsThis(tr("Indicate the room to show as background"));
        toolButton->setIcon(QIcon{":/qtgameengine/icons/room.png"});
        toolButton->setPopupMode(QToolButton::InstantPopup);
        toolButton->setMenu(m_menuRooms);
        m_ui->toolBar->addWidget(toolButton);
    }

    connect(m_menuRooms, &QMenu::aboutToShow,
            this, &PathPropertiesDialog::roomsMenuAboutToShow);

    m_ui->treeView->setModel(m_pointsModel.get());

    m_ui->radioButtonStraight->setChecked(m_path.type == Path::Type::Straight);
    m_ui->radioButtonSmooth->setChecked(m_path.type == Path::Type::Smooth);
    m_ui->checkBoxClosed->setChecked(m_path.closed);
    m_ui->spinBoxPrecision->setValue(m_path.precision);

    m_ui->pathPointsWidget->setClosed(m_path.closed);

    m_labelX->setFrameStyle(QFrame::Sunken);
    m_ui->statusbar->addWidget(m_labelX, 1);
    m_labelY->setFrameStyle(QFrame::Sunken);
    m_ui->statusbar->addWidget(m_labelY, 1);
    m_labelArea->setFrameStyle(QFrame::Sunken);
    m_ui->statusbar->addWidget(m_labelArea, 4);

    {
        auto frame = new QFrame{this};
        frame->setFrameStyle(QFrame::Sunken);
        m_ui->statusbar->addPermanentWidget(frame, 2);
    }

    m_ui->lineEditName->setText(m_path.name);

    m_ui->treeView->setColumnWidth(1, 75);

    connect(m_ui->pathPointsWidget, &PathPointsWidget::pointInserted,
            m_pointsModel.get(), &PathPointsModel::pointInserted);
    connect(m_ui->pathPointsWidget, &PathPointsWidget::pointMoved,
            m_pointsModel.get(), &PathPointsModel::pointMoved);

    connect(&m_projectModel, &ProjectTreeModel::pathNameChanged,
            this, &PathPropertiesDialog::pathNameChanged);

    connect(m_ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &PathPropertiesDialog::selectionChanged);

    connect(m_ui->actionUndo, &QAction::triggered,
            this, &PathPropertiesDialog::undo);
    connect(m_ui->actionClear, &QAction::triggered,
            this, &PathPropertiesDialog::clearPath);
    connect(m_ui->actionReverse, &QAction::triggered,
            this, &PathPropertiesDialog::reversePath);
    connect(m_ui->actionShift, &QAction::triggered,
            this, &PathPropertiesDialog::shiftPath);
    connect(m_ui->actionMirrorHorizontally, &QAction::triggered,
            this, &PathPropertiesDialog::mirrorPathHorizontally);
    connect(m_ui->actionMirrorVertically, &QAction::triggered,
            this, &PathPropertiesDialog::mirrorPathVertically);
    connect(m_ui->actionRotate, &QAction::triggered,
            this, &PathPropertiesDialog::rotatePath);
    connect(m_ui->actionScale, &QAction::triggered,
            this, &PathPropertiesDialog::scalePath);
    connect(m_ui->actionShiftLeft, &QAction::triggered,
            this, &PathPropertiesDialog::shiftViewLeft);
    connect(m_ui->actionShiftRight, &QAction::triggered,
            this, &PathPropertiesDialog::shiftViewRight);
    connect(m_ui->actionShiftUp, &QAction::triggered,
            this, &PathPropertiesDialog::shiftViewUp);
    connect(m_ui->actionShiftDown, &QAction::triggered,
            this, &PathPropertiesDialog::shiftViewDown);
    connect(m_ui->actionCenterView, &QAction::triggered,
            this, &PathPropertiesDialog::centerView);

    connect(m_ui->pathPointsWidget, &PathPointsWidget::snapXChanged,
            m_spinBoxSnapX, &QSpinBox::setValue);
    connect(m_ui->pathPointsWidget, &PathPointsWidget::snapYChanged,
            m_spinBoxSnapY, &QSpinBox::setValue);
    connect(m_ui->pathPointsWidget, &PathPointsWidget::gridEnabledChanged,
            m_ui->actionGridEnabled, &QAction::setChecked);
    connect(m_ui->pathPointsWidget, &PathPointsWidget::closedChanged,
            m_ui->checkBoxClosed, &QCheckBox::setChecked);
    connect(m_ui->pathPointsWidget, &PathPointsWidget::cursorMoved,
            this, &PathPropertiesDialog::cursorMoved);
    connect(m_ui->pathPointsWidget, &PathPointsWidget::selectedIndexChanged,
            this, &PathPropertiesDialog::selectedPointChanged);

    connect(m_spinBoxSnapX, &QSpinBox::valueChanged,
            m_ui->pathPointsWidget, &PathPointsWidget::setSnapX);
    connect(m_spinBoxSnapY, &QSpinBox::valueChanged,
            m_ui->pathPointsWidget, &PathPointsWidget::setSnapY);
    connect(m_ui->actionGridEnabled, &QAction::toggled,
            m_ui->pathPointsWidget, &PathPointsWidget::setGridEnabled);
    connect(m_ui->checkBoxClosed, &QCheckBox::toggled,
            m_ui->pathPointsWidget, &PathPointsWidget::setClosed);

    connect(m_spinBoxSnapX, &QSpinBox::valueChanged,
            this, &PathPropertiesDialog::changed);
    connect(m_spinBoxSnapY, &QSpinBox::valueChanged,
            this, &PathPropertiesDialog::changed);
    connect(m_ui->actionGridEnabled, &QAction::toggled,
            this, &PathPropertiesDialog::changed);

    connect(m_ui->spinBoxX, &QSpinBox::valueChanged,
            this, &PathPropertiesDialog::pointFieldsChanged);
    connect(m_ui->spinBoxY, &QSpinBox::valueChanged,
            this, &PathPropertiesDialog::pointFieldsChanged);
    connect(m_ui->spinBoxSp, &QSpinBox::valueChanged,
            this, &PathPropertiesDialog::pointFieldsChanged);

    connect(m_ui->pushButtonAdd, &QAbstractButton::clicked,
            this, &PathPropertiesDialog::addPoint);
    connect(m_ui->pushButtonInsert, &QAbstractButton::clicked,
            this, &PathPropertiesDialog::insertPoint);
    connect(m_ui->pushButtonDelete, &QAbstractButton::clicked,
            this, &PathPropertiesDialog::deletePoint);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &PathPropertiesDialog::changed);

    connect(m_pointsModel.get(), &QAbstractTableModel::rowsInserted,
            this, &PathPropertiesDialog::changed);
    connect(m_pointsModel.get(), &QAbstractTableModel::rowsInserted,
            m_ui->pathPointsWidget, qOverload<>(&PathPointsWidget::update));
    connect(m_pointsModel.get(), &QAbstractTableModel::rowsRemoved,
            this, &PathPropertiesDialog::changed);
    connect(m_pointsModel.get(), &QAbstractTableModel::rowsRemoved,
            m_ui->pathPointsWidget, qOverload<>(&PathPointsWidget::update));
    connect(m_pointsModel.get(), &QAbstractTableModel::dataChanged,
            this, &PathPropertiesDialog::dataChanged);
    connect(m_pointsModel.get(), &QAbstractTableModel::dataChanged,
            this, &PathPropertiesDialog::changed);
    connect(m_pointsModel.get(), &QAbstractTableModel::dataChanged,
            m_ui->pathPointsWidget, qOverload<>(&PathPointsWidget::update));
    connect(m_ui->radioButtonStraight, &QRadioButton::toggled,
            this, &PathPropertiesDialog::changed);
    connect(m_ui->radioButtonSmooth, &QRadioButton::toggled,
            this, &PathPropertiesDialog::changed);
    connect(m_ui->checkBoxClosed, &QCheckBox::toggled,
            this, &PathPropertiesDialog::changed);
    connect(m_ui->spinBoxPrecision, &QSpinBox::valueChanged,
            this, &PathPropertiesDialog::changed);
}

PathPropertiesDialog::~PathPropertiesDialog() = default;

void PathPropertiesDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (m_path.name != m_ui->lineEditName->text())
    {
        if (!m_projectModel.rename<Path>(m_path, m_ui->lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Path failed!"), tr("Renaming Path failed!"));
            return;
        }
    }

    m_path.points = m_points;

    if (m_ui->radioButtonStraight->isChecked())
        m_path.type = Path::Type::Straight;
    else if (m_ui->radioButtonSmooth->isChecked())
        m_path.type = Path::Type::Smooth;
    else
    {
        QMessageBox::critical(this, tr("No kind selected!"), tr("No kind selected!"));
        return;
    }

    m_path.closed = m_ui->checkBoxClosed->isChecked();
    m_path.precision = m_ui->spinBoxPrecision->value();
    m_path.snapX = m_spinBoxSnapX->value();
    m_path.snapY = m_spinBoxSnapY->value();
    m_path.gridEnabled = m_ui->actionGridEnabled->isChecked();

    QDialog::accept();
}

void PathPropertiesDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Path has been modified."),
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

void PathPropertiesDialog::selectionChanged(const QModelIndex &index)
{
    if (!index.isValid())
    {
        m_ui->pathPointsWidget->setSelectedIndex(std::nullopt);
        return;
    }

    m_ui->pathPointsWidget->setSelectedIndex(index.row());

    updatePointFields();
}

void PathPropertiesDialog::selectedPointChanged(const std::optional<std::size_t> &index)
{
    m_ui->treeView->setCurrentIndex(index ? m_pointsModel->index(*index, 0, {}) : QModelIndex{});
}

void PathPropertiesDialog::dataChanged(const QModelIndex &index)
{
    if (index == m_ui->treeView->currentIndex())
        updatePointFields();
}

void PathPropertiesDialog::cursorMoved(const QPoint &point)
{
    m_labelX->setText(tr("X: %0").arg(point.x()));
    m_labelY->setText(tr("Y: %0").arg(point.y()));
}

void PathPropertiesDialog::addPoint()
{
    if (!m_pointsModel->insertPoint(m_points.size(), getPointFromFields()))
    {
        QMessageBox::warning(this, tr("Could not add Point!"), tr("Could not add Point!"));
        return;
    }
}

void PathPropertiesDialog::insertPoint()
{
    const auto index = m_ui->treeView->currentIndex();
    if (!index.isValid())
        return;

    if (!m_pointsModel->insertPoint(index.row() + 1, getPointFromFields()))
    {
        QMessageBox::warning(this, tr("Could not insert Point!"), tr("Could not insert Point!"));
        return;
    }
}

void PathPropertiesDialog::deletePoint()
{
    const auto index = m_ui->treeView->currentIndex();
    if (!index.isValid())
        return;

    if (!m_pointsModel->removeRows(index.row(), 1))
    {
        QMessageBox::warning(this, tr("Could not remove Point!"), tr("Could not remove Point!"));
        return;
    }
}

void PathPropertiesDialog::undo()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::clearPath()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::reversePath()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::shiftPath()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::mirrorPathHorizontally()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::mirrorPathVertically()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::rotatePath()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::scalePath()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::shiftViewLeft()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::shiftViewRight()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::shiftViewUp()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::shiftViewDown()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::centerView()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::pointFieldsChanged()
{
    const auto index = m_ui->treeView->currentIndex();
    if (!index.isValid())
        return;

    if (!m_pointsModel->updatePoint(index.row(), getPointFromFields()))
    {
        QMessageBox::warning(this, tr("Could not update Point!"), tr("Could not update Point!"));
        return;
    }

    changed();
}

void PathPropertiesDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void PathPropertiesDialog::pathNameChanged(const Path &path)
{
    if (&path != &m_path)
        return;

    {
        QSignalBlocker blocker{m_ui->lineEditName};
        m_ui->lineEditName->setText(path.name);
    }

    updateTitle();
}

void PathPropertiesDialog::roomsMenuAboutToShow()
{
    m_menuRooms->clear();
    qDebug() << "called" << tr("No Room");
    m_menuRooms->addAction(tr("No Room"));
    for (const auto &room : m_projectModel.project()->rooms)
    {
        qDebug() << "called" << room.name;
        m_menuRooms->addAction(room.name);
    }
}

void PathPropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Path Properties: %0%1")
                       .arg(m_path.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}

void PathPropertiesDialog::updatePointFields()
{
    const auto index = m_ui->treeView->currentIndex();
    if (!index.isValid())
        return;

    const auto point = m_pointsModel->getPoint(index);
    if (!point)
        return;

    {
        QSignalBlocker blocker{m_ui->spinBoxX};
        m_ui->spinBoxX->setValue(point->point.x());
    }
    {
        QSignalBlocker blocker{m_ui->spinBoxY};
        m_ui->spinBoxY->setValue(point->point.y());
    }
    {
        QSignalBlocker blocker{m_ui->spinBoxSp};
        m_ui->spinBoxSp->setValue(point->sp);
    }
}

Path::Point PathPropertiesDialog::getPointFromFields() const
{
    return Path::Point{
        .point{
            m_ui->spinBoxX->value(),
            m_ui->spinBoxY->value()
        },
        .sp = m_ui->spinBoxSp->value()
    };
}
