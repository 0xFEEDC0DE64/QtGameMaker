#include "actionscontainerwidget.h"
#include "ui_actionscontainerwidget.h"

#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QKeySequence>

#include "models/actionscontainermodel.h"
#include "dialogs/actions/movefixeddialog.h"
#include "dialogs/actions/movefreedialog.h"
#include "dialogs/actions/movetowardsdialog.h"
#include "dialogs/actions/speedhorizontaldialog.h"
#include "dialogs/actions/speedverticaldialog.h"
#include "dialogs/actions/setgravitydialog.h"
#include "dialogs/actions/executecodedialog.h"

namespace {
template<typename T> struct ActionDialogForImpl;
template<> struct ActionDialogForImpl<MoveFixedAction> { using Dialog = MoveFixedDialog; };
template<> struct ActionDialogForImpl<MoveFreeAction> { using Dialog = MoveFreeDialog; };
template<> struct ActionDialogForImpl<MoveTowardsAction> { using Dialog = MoveTowardsDialog; };
template<> struct ActionDialogForImpl<SpeedHorizontalAction> { using Dialog = SpeedHorizontalDialog; };
template<> struct ActionDialogForImpl<SpeedVerticalAction> { using Dialog = SpeedVerticalDialog; };
template<> struct ActionDialogForImpl<SetGravityAction> { using Dialog = SetGravityDialog; };
template<> struct ActionDialogForImpl<ExecuteCodeAction> { using Dialog = ExecuteCodeDialog; };
template<typename T> using ActionDialogFor = ActionDialogForImpl<T>::Dialog;
}

ActionsContainerWidget::ActionsContainerWidget(QWidget *parent) :
    QWidget{parent},
    m_ui{std::make_unique<Ui::ActionsContainerWidget>()},
    m_actionsModel{std::make_unique<ActionsContainerModel>(this)}
{
    m_ui->setupUi(this);

    m_ui->listViewActions->setModel(m_actionsModel.get());

    connect(m_actionsModel.get(), &ActionsContainerModel::changed,
            this, &ActionsContainerWidget::changed);

    connect(m_actionsModel.get(), &ActionsContainerModel::actionsContainerMissing,
            this, &ActionsContainerWidget::actionsContainerMissing);

    connect(m_ui->listViewActions, &QListView::customContextMenuRequested,
            this, &ActionsContainerWidget::actionsContextMenuRequested);

    connect(m_ui->listViewActions, &QListView::doubleClicked,
            this, &ActionsContainerWidget::actionDoubleClicked);

    connect(m_actionsModel.get(), &QAbstractItemModel::modelAboutToBeReset,
            m_ui->listViewActions, [listView=m_ui->listViewActions](){
                listView->setCurrentIndex(QModelIndex{});
    });

    connect(m_ui->toolButtonMoveFixed, &QAbstractButton::clicked,
            this, &ActionsContainerWidget::createNewAction<MoveFixedAction>);
    connect(m_ui->toolButtonMoveFree, &QAbstractButton::clicked,
            this, &ActionsContainerWidget::createNewAction<MoveFreeAction>);
    connect(m_ui->toolButtonMoveTowards, &QAbstractButton::clicked,
            this, &ActionsContainerWidget::createNewAction<MoveTowardsAction>);
    connect(m_ui->toolButtonSpeedHorizontal, &QAbstractButton::clicked,
            this, &ActionsContainerWidget::createNewAction<SpeedHorizontalAction>);
    connect(m_ui->toolButtonSpeedVertical, &QAbstractButton::clicked,
            this, &ActionsContainerWidget::createNewAction<SpeedVerticalAction>);
    connect(m_ui->toolButtonSetGravity, &QAbstractButton::clicked,
            this, &ActionsContainerWidget::createNewAction<SetGravityAction>);
    connect(m_ui->toolButtonExecuteCode, &QAbstractButton::clicked,
            this, &ActionsContainerWidget::createNewAction<ExecuteCodeAction>);

    m_ui->toolButtonMoveFixed->setAction(MoveFixedAction{});
    m_ui->toolButtonMoveFree->setAction(MoveFreeAction{});
    m_ui->toolButtonMoveTowards->setAction(MoveTowardsAction{});
    m_ui->toolButtonSpeedHorizontal->setAction(SpeedHorizontalAction{});
    m_ui->toolButtonSpeedVertical->setAction(SpeedVerticalAction{});
    m_ui->toolButtonSetGravity->setAction(SetGravityAction{});
    m_ui->toolButtonExecuteCode->setAction(ExecuteCodeAction{});
}

ActionsContainerWidget::~ActionsContainerWidget() = default;

ActionsContainer *ActionsContainerWidget::actionsContainer()
{
    return m_actionsModel->actionsContainer();
}

const ActionsContainer *ActionsContainerWidget::actionsContainer() const
{
    return m_actionsModel->actionsContainer();
}

void ActionsContainerWidget::setActionsContainer(ActionsContainer *actionsContainer)
{
    m_actionsModel->setActionsContainer(actionsContainer);
}

void ActionsContainerWidget::actionDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    auto action = m_actionsModel->getAction(index);
    if (!action)
        return;

    if (auto ptr = std::get_if<MoveFixedAction>(action))
    {
        MoveFixedDialog dialog{*ptr, m_projectModel, this};
        if (dialog.exec() == QDialog::Accepted)
            emit changed();
    }
    else if (auto ptr = std::get_if<MoveFreeAction>(action))
    {
        MoveFreeDialog dialog{*ptr, m_projectModel, this};
        if (dialog.exec() == QDialog::Accepted)
            emit changed();
    }
    else if (auto ptr = std::get_if<MoveTowardsAction>(action))
    {
        MoveTowardsDialog dialog{*ptr, m_projectModel, this};
        if (dialog.exec() == QDialog::Accepted)
            emit changed();
    }
    else if (auto ptr = std::get_if<SpeedHorizontalAction>(action))
    {
        SpeedHorizontalDialog dialog{*ptr, m_projectModel, this};
        if (dialog.exec() == QDialog::Accepted)
            emit changed();
    }
    else if (auto ptr = std::get_if<SpeedVerticalAction>(action))
    {
        SpeedVerticalDialog dialog{*ptr, m_projectModel, this};
        if (dialog.exec() == QDialog::Accepted)
            emit changed();
    }
    else if (auto ptr = std::get_if<SetGravityAction>(action))
    {
        SetGravityDialog dialog{*ptr, m_projectModel, this};
        if (dialog.exec() == QDialog::Accepted)
            emit changed();
    }
    else if (auto ptr = std::get_if<ExecuteCodeAction>(action))
    {
        ExecuteCodeDialog dialog{*ptr, m_projectModel, this};
        if (dialog.exec() == QDialog::Accepted)
            emit changed();
    }
    else
        QMessageBox::information(this, tr("Not implemented!"), tr("Not implemented!"));
}

void ActionsContainerWidget::actionsContextMenuRequested(const QPoint &pos)
{
    const auto index = m_ui->listViewActions->indexAt(pos);
    auto action = index.isValid() ? m_actionsModel->getAction(index) : nullptr;

    QMenu menu{this};
    if (auto action = menu.addAction(tr("&Edit Values"), this, [this,index](){
        actionDoubleClicked(index);
    }))
        action->setEnabled(action);
    menu.addSeparator();
    if (auto action = menu.addAction(tr("C&ut"), this, [this](){
        QMessageBox::information(this, tr("Not implemented!"), tr("Not implemented!"));
    }))
    {
        action->setEnabled(action);
        action->setShortcut(QKeySequence::Cut);
    }
    if (auto action = menu.addAction(tr("&Copy"), this, [this](){
        QMessageBox::information(this, tr("Not implemented!"), tr("Not implemented!"));
    }))
    {
        action->setEnabled(action);
        action->setShortcut(QKeySequence::Copy);
    }
    if (auto action = menu.addAction(tr("&Paste"), this, [this](){
        QMessageBox::information(this, tr("Not implemented!"), tr("Not implemented!"));
    }))
    {
        action->setEnabled(action);
        action->setShortcut(QKeySequence::Paste);
    }
    menu.addSeparator();
    if (auto action = menu.addAction(tr("&Delete"), this, [this,index](){
        if (!m_actionsModel->removeRow(index.row()))
            QMessageBox::warning(this, tr("Could not remove action!"), tr("Could not remove action!"));
    }))
    {
        action->setEnabled(action);
        action->setShortcut(QKeySequence::Delete);
    }
    menu.addSeparator();
    if (auto action = menu.addAction(tr("Select &All"), this, [this](){
        QMessageBox::information(this, tr("Not implemented!"), tr("Not implemented!"));
    }))
    {
        action->setEnabled(m_actionsModel->actionsContainer() && !m_actionsModel->actionsContainer()->empty());
        action->setShortcut(QKeySequence::SelectAll);
    }
    menu.addSeparator();
    if (auto action = menu.addAction(tr("C&lear"), this, [this](){
        QMessageBox::information(this, tr("Not implemented!"), tr("Not implemented!"));
    }))
    {
        action->setEnabled(m_actionsModel->actionsContainer() && !m_actionsModel->actionsContainer()->empty());
    }
    menu.exec(m_ui->listViewActions->viewport()->mapToGlobal(pos));
}

void ActionsContainerWidget::actionsContainerMissing()
{
    QMessageBox::warning(this, tr("You need to select or add an event before you can add actions"),
                               tr("You need to select or add an event before you can add actions"));
}

template<typename T>
void ActionsContainerWidget::createNewAction()
{
    if (!m_actionsModel->actionsContainer())
        return;

    T action;
    ActionDialogFor<T> dialog{action, m_projectModel, this};
    if (dialog.exec() == QDialog::Accepted)
        m_actionsModel->appendAction(std::move(action));
}
