#include "pathpropertiesdialog.h"
#include "ui_pathpropertiesdialog.h"

#include <QSpinBox>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>

#include "projectcontainer.h"
#include "models/projecttreemodel.h"
#include "models/pathpointsmodel.h"

PathPropertiesDialog::PathPropertiesDialog(Path &path, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::PathPropertiesDialog>()},
    m_path{path},
    m_projectModel{projectModel},
    m_points{path.points},
    m_pointsModel{std::make_unique<PathPointsModel>(m_points, this)},
    m_spinBoxSnapX{new QSpinBox{this}},
    m_spinBoxSnapY{new QSpinBox{this}},
    m_labelX{new QLabel{tr("x: %0").arg(0)}},
    m_labelY{new QLabel{tr("y: %0").arg(0)}},
    m_labelArea{new QLabel{tr("Area: (%0,%1)->(%2,%3)").arg(0).arg(0).arg(0).arg(0)}}
{
    m_ui->setupUi(this);

    m_ui->widget->setPoints(&m_points);

    updateTitle();

    {
        auto label = new QLabel{tr("Snap X:"), this};
        label->setBuddy(m_spinBoxSnapX);
        m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(18), label);
    }
    m_spinBoxSnapX->setValue(m_ui->widget->gridX());
    m_spinBoxSnapX->setMaximumWidth(50);
    m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(19), m_spinBoxSnapX);

    {
        auto label = new QLabel{tr("Snap Y:"), this};
        label->setBuddy(m_spinBoxSnapY);
        m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(20), label);
    }
    m_spinBoxSnapY->setValue(m_ui->widget->gridY());
    m_spinBoxSnapY->setMaximumWidth(50);
    m_ui->toolBar->insertWidget(m_ui->toolBar->actions().at(21), m_spinBoxSnapY);

    m_ui->actionGrid->setChecked(m_ui->widget->showGrid());

    m_ui->treeView->setModel(m_pointsModel.get());

    m_ui->radioButtonStraight->setChecked(m_path.type == Path::Type::Straight);
    m_ui->radioButtonSmooth->setChecked(m_path.type == Path::Type::Smooth);
    m_ui->checkBoxClosed->setChecked(m_path.closed);
    m_ui->spinBoxPrecision->setValue(m_path.precision);

    m_ui->widget->setClosed(m_path.closed);

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

    connect(m_ui->widget, &PathPointsWidget::pointInserted,
            m_pointsModel.get(), &PathPointsModel::pointInserted);
    connect(m_ui->widget, &PathPointsWidget::pointMoved,
            m_pointsModel.get(), &PathPointsModel::pointMoved);

    connect(&m_projectModel, &ProjectTreeModel::pathNameChanged,
            this, &PathPropertiesDialog::pathNameChanged);

    connect(m_ui->treeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &PathPropertiesDialog::selectionChanged);

    connect(m_ui->widget, &PathPointsWidget::gridXChanged,
            m_spinBoxSnapX, &QSpinBox::setValue);
    connect(m_ui->widget, &PathPointsWidget::gridYChanged,
            m_spinBoxSnapY, &QSpinBox::setValue);
    connect(m_ui->widget, &PathPointsWidget::showGridChanged,
            m_ui->actionGrid, &QAction::setChecked);
    connect(m_ui->widget, &PathPointsWidget::closedChanged,
            m_ui->checkBoxClosed, &QCheckBox::setChecked);
    connect(m_ui->widget, &PathPointsWidget::cursorMoved,
            this, &PathPropertiesDialog::cursorMoved);
    connect(m_ui->widget, &PathPointsWidget::selectedIndexChanged,
            this, &PathPropertiesDialog::selectedPointChanged);

    connect(m_spinBoxSnapX, &QSpinBox::valueChanged,
            m_ui->widget, &PathPointsWidget::setGridX);
    connect(m_spinBoxSnapY, &QSpinBox::valueChanged,
            m_ui->widget, &PathPointsWidget::setGridY);
    connect(m_ui->actionGrid, &QAction::toggled,
            m_ui->widget, &PathPointsWidget::setShowGrid);
    connect(m_ui->checkBoxClosed, &QCheckBox::toggled,
            m_ui->widget, &PathPointsWidget::setClosed);

    connect(m_ui->pushButtonAdd, &QAbstractButton::clicked,
            this, &PathPropertiesDialog::add);
    connect(m_ui->pushButtonInsert, &QAbstractButton::clicked,
            this, &PathPropertiesDialog::insert);
    connect(m_ui->pushButtonDelete, &QAbstractButton::clicked,
            this, &PathPropertiesDialog::delete_);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &PathPropertiesDialog::changed);

    connect(m_pointsModel.get(), &QAbstractTableModel::rowsInserted,
            this, &PathPropertiesDialog::changed);
    connect(m_pointsModel.get(), &QAbstractTableModel::rowsRemoved,
            this, &PathPropertiesDialog::changed);
    connect(m_pointsModel.get(), &QAbstractTableModel::dataChanged,
            this, &PathPropertiesDialog::dataChanged);
    connect(m_pointsModel.get(), &QAbstractTableModel::dataChanged,
            this, &PathPropertiesDialog::changed);
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

    // TODO update points

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
        m_ui->widget->setSelectedIndex(std::nullopt);
        return;
    }

    m_ui->widget->setSelectedIndex(index.row());

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

void PathPropertiesDialog::add()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::insert()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::delete_()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
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

void PathPropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Path Properties: %0%1")
                       .arg(m_path.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}

void PathPropertiesDialog::updatePointFields()
{
    const auto point = m_pointsModel->getPoint(m_ui->treeView->currentIndex());
    m_ui->spinBoxX->setValue(point.point.x());
    m_ui->spinBoxY->setValue(point.point.y());
    m_ui->spinBoxSp->setValue(point.sp);
}
