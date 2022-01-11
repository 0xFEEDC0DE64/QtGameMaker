#include "timelinepropertiesdialog.h"
#include "ui_timelinepropertiesdialog.h"

#include <QDebug>
#include <QMessageBox>

#include "projectcontainer.h"
#include "projecttreemodel.h"
#include "timelinemomentsmodel.h"
#include "timelineactionsmodel.h"

TimeLinePropertiesDialog::TimeLinePropertiesDialog(TimeLine &timeLine, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::TimeLinePropertiesDialog>()},
    m_timeLine{timeLine},
    m_projectModel{projectModel},
    m_momentsModel{std::make_unique<TimelineMomentsModel>(this)},
    m_actionsModel{std::make_unique<TimelineActionsModel>(this)}
{
    m_ui->setupUi(this);

    updateTitle();

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    m_ui->lineEditName->setText(m_timeLine.name);

    m_ui->listViewMoments->setModel(m_momentsModel.get());

    m_ui->listViewActions->setModel(m_actionsModel.get());

    connect(&m_projectModel, &ProjectTreeModel::timeLineNameChanged,
            this, &TimeLinePropertiesDialog::timeLineNameChanged);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &TimeLinePropertiesDialog::changed);

    connect(m_ui->pushButtonAdd, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::add);
    connect(m_ui->pushButtonChange, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::change);
    connect(m_ui->pushButtonDelete, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::delete_);
    connect(m_ui->pushButtonClear, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::clear);
    connect(m_ui->pushButtonShift, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::shift);
    connect(m_ui->pushButtonDuplicate, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::duplicate);
    connect(m_ui->pushButtonSpread, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::spread);
    connect(m_ui->pushButtonMerge, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::merge);
    connect(m_ui->pushButtonShowInformation, &QAbstractButton::clicked,
            this, &TimeLinePropertiesDialog::showInformation);
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

    // TODO

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

void TimeLinePropertiesDialog::add()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::change()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::delete_()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::clear()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::shift()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::duplicate()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::spread()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::merge()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::showInformation()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TimeLinePropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Time Line Properties: %0%1")
                       .arg(m_timeLine.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}
