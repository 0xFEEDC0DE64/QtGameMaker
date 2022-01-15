#include "timelinepropertiesdialog.h"
#include "ui_timelinepropertiesdialog.h"

#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>

#include <limits>

#include "models/projecttreemodel.h"
#include "models/timelinemomentsmodel.h"
#include "dialogs/deletemomentsdialog.h"

TimeLinePropertiesDialog::TimeLinePropertiesDialog(TimeLine &timeLine, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::TimeLinePropertiesDialog>()},
    m_timeLine{timeLine},
    m_projectModel{projectModel},
    m_moments{timeLine.moments},
    m_momentsModel{std::make_unique<TimelineMomentsModel>(m_moments, this)}
{
    m_ui->setupUi(this);

    updateTitle();

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    m_ui->lineEditName->setText(m_timeLine.name);

    m_ui->listViewMoments->setModel(m_momentsModel.get());

    connect(&m_projectModel, &ProjectTreeModel::timeLineNameChanged,
            this, &TimeLinePropertiesDialog::timeLineNameChanged);

    connect(m_momentsModel.get(), &QAbstractItemModel::modelReset,
            this, &TimeLinePropertiesDialog::changed);
    connect(m_momentsModel.get(), &QAbstractItemModel::rowsInserted,
            this, &TimeLinePropertiesDialog::changed);
    connect(m_momentsModel.get(), &QAbstractItemModel::dataChanged,
            this, &TimeLinePropertiesDialog::changed);
    connect(m_momentsModel.get(), &QAbstractItemModel::rowsRemoved,
            this, &TimeLinePropertiesDialog::changed);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &TimeLinePropertiesDialog::changed);

    connect(m_ui->pushButtonAdd, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::addMoment);
    connect(m_ui->pushButtonChange, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::changeMoment);
    connect(m_ui->pushButtonDelete, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::deleteMoments);
    connect(m_ui->pushButtonClear, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::clearMoments);
    connect(m_ui->pushButtonShift, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::shiftMoments);
    connect(m_ui->pushButtonDuplicate, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::duplicateMoments);
    connect(m_ui->pushButtonSpread, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::spreadMoments);
    connect(m_ui->pushButtonMerge, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::mergeMoments);
    connect(m_ui->pushButtonShowInformation, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::showInformation);

    connect(m_ui->listViewMoments->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &TimeLinePropertiesDialog::currentMomentChanged);
    connect(m_momentsModel.get(), &QAbstractItemModel::modelAboutToBeReset,
            m_ui->listViewMoments, [listView=m_ui->listViewMoments](){
        listView->setCurrentIndex(QModelIndex{});
    });
    connect(m_ui->listViewMoments, &QListView::customContextMenuRequested,
            this, &TimeLinePropertiesDialog::momentsContextMenuRequested);

    connect(m_ui->actionsWidget, &ActionsContainerWidget::changed,
            this, &TimeLinePropertiesDialog::changed);
}

TimeLinePropertiesDialog::~TimeLinePropertiesDialog() = default;

void TimeLinePropertiesDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (m_timeLine.name != m_ui->lineEditName->text())
    {
        if (!m_projectModel.rename<TimeLine>(m_timeLine, m_ui->lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Time Line failed!"), tr("Renaming Time Line failed!"));
            return;
        }
    }

    m_timeLine.moments = m_moments;

    QDialog::accept();
}

void TimeLinePropertiesDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Time Line has been modified."),
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

void TimeLinePropertiesDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void TimeLinePropertiesDialog::timeLineNameChanged(const TimeLine &timeLine)
{
    if (&timeLine != &m_timeLine)
        return;

    {
        QSignalBlocker blocker{m_ui->lineEditName};
        m_ui->lineEditName->setText(timeLine.name);
    }

    updateTitle();
}

void TimeLinePropertiesDialog::addMoment()
{
    TimeLine::moment_key_t newMoment{};
    for (const auto &moment : m_moments)
        if (moment.first >= newMoment)
            newMoment = moment.first + 1;

    bool ok;
    const auto moment = QInputDialog::getInt(this, tr("Adding a Moment"), tr("Indicate the moment:"), newMoment, 0, std::numeric_limits<TimeLine::moment_key_t>::max(), 1, &ok);
    if (!ok)
        return;

    if (!m_momentsModel->addMoment(moment))
        QMessageBox::warning(this, tr("Could not add Moment!"), tr("Could not add Moment!"));
}

void TimeLinePropertiesDialog::changeMoment()
{
    const auto index = m_ui->listViewMoments->currentIndex();
    if (!index.isValid())
        return;

    const auto moment = m_momentsModel->getMoment(index);
    if (!moment)
        return;

    bool ok;
    const auto newMoment = QInputDialog::getInt(this, tr("Changing a Moment"), tr("Indicate the moment:"), moment->first, 0, std::numeric_limits<TimeLine::moment_key_t>::max(), 1, &ok);
    if (!ok)
        return;

    if (!m_momentsModel->changeMoment(moment->first, newMoment))
        QMessageBox::warning(this, tr("Could not change Moment!"), tr("Could not change Moment!"));
}

void TimeLinePropertiesDialog::deleteMoments()
{
    if (m_moments.empty())
        return;

    std::optional<TimeLine::moment_key_t> momentKey;

    if (const auto index = m_ui->listViewMoments->currentIndex(); index.isValid())
        if (const auto moment = m_momentsModel->getMoment(index); moment)
            momentKey = moment->first;

    DeleteMomentsDialog dialog = momentKey ? DeleteMomentsDialog{*momentKey, *momentKey, this} : DeleteMomentsDialog{this};
    if (dialog.exec() == QDialog::Accepted)
    {
        if (dialog.fromMoment() > dialog.tillMoment())
        {
            QMessageBox::warning(this, tr("Invalid input!"), tr("Invalid input!") + "\n\n" + tr("From moment cannot be greater than Till moment!"));
            return;
        }

        if (!m_momentsModel->removeRows(dialog.fromMoment(), dialog.tillMoment() - dialog.fromMoment() + 1))
            QMessageBox::warning(this, tr("Could not remove Moments!"), tr("Could not remove Moments!"));
    }
}

void TimeLinePropertiesDialog::clearMoments()
{
    if (m_moments.empty())
        return;
    if (QMessageBox::question(this, tr("Confirm"), tr("Are you sure, that you really want to remove all Moments?")) != QMessageBox::Yes)
        return;
    if (!m_momentsModel->clearMoments())
        QMessageBox::warning(this, tr("Could not clear Moments!"), tr("Could not clear Moments!"));
}

void TimeLinePropertiesDialog::shiftMoments()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::duplicateMoments()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::spreadMoments()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::mergeMoments()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::showInformation()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::currentMomentChanged(const QModelIndex &index)
{
    qDebug() << index.isValid() << index.row();

    if (index.isValid())
    {
        if (auto moment = m_momentsModel->getMoment(index))
            m_ui->actionsWidget->setActionsContainer(&moment->second);
        else
            goto none;
    }
    else
    {
    none:
        m_ui->actionsWidget->setActionsContainer(nullptr);
    }
}

void TimeLinePropertiesDialog::momentsContextMenuRequested(const QPoint &pos)
{
    const auto index = m_ui->listViewMoments->indexAt(pos);
    auto moment = index.isValid() ? m_momentsModel->getMoment(index) : nullptr;

    QMenu menu{this};
    menu.addAction(tr("&Add Moment"), this, &TimeLinePropertiesDialog::addMoment);
    menu.addAction(tr("&Change Moment"), this, &TimeLinePropertiesDialog::changeMoment)->setEnabled(moment);
    menu.addAction(tr("&Delete Moment"), this, &TimeLinePropertiesDialog::deleteMoments)->setEnabled(moment);
    menu.addSeparator();
    menu.addAction(tr("&Shift Moments"), this, &TimeLinePropertiesDialog::shiftMoments)->setEnabled(moment);
    menu.addAction(tr("&Merge Moments"), this, &TimeLinePropertiesDialog::mergeMoments)->setEnabled(moment);
    menu.exec(m_ui->listViewMoments->viewport()->mapToGlobal(pos));
}

void TimeLinePropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Time Line Properties: %0%1")
                       .arg(m_timeLine.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}
