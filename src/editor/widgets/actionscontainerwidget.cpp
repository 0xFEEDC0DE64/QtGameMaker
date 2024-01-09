#include "actionscontainerwidget.h"
#include "ui_actionscontainerwidget.h"

#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QKeySequence>

#include "models/actionscontainermodel.h"
#include "dialogs/codeactiondialog.h"
#include "dialogs/actions/movefixeddialog.h"

ActionsContainerWidget::ActionsContainerWidget(QWidget *parent) :
    QWidget{parent},
    m_ui{std::make_unique<Ui::ActionsContainerWidget>()},
    m_actionsModel{std::make_unique<ActionsContainerModel>(this)}
{
    m_ui->setupUi(this);

    m_ui->listViewActions->setModel(m_actionsModel.get());

    connect(m_ui->listViewActions, &QListView::customContextMenuRequested,
            this, &ActionsContainerWidget::actionsContextMenuRequested);

    connect(m_ui->listViewActions, &QListView::doubleClicked,
            this, &ActionsContainerWidget::actionDoubleClicked);

    connect(m_actionsModel.get(), &QAbstractItemModel::modelAboutToBeReset,
            m_ui->listViewActions, [listView=m_ui->listViewActions](){
                listView->setCurrentIndex(QModelIndex{});
    });

    connect(m_ui->toolButtonMoveFixed, &QAbstractButton::clicked,
            this, [this](){
        MoveFixedAction action;
        MoveFixedDialog dialog{action, this};
        if (dialog.exec() == QDialog::Accepted)
        {
            // TODO insert into model
            emit changed();
        }
    });

    m_ui->toolButtonMoveFixed->setAction(MoveFixedAction{});
    m_ui->toolButtonMoveFree->setAction(MoveFreeAction{});
    m_ui->toolButtonMoveTowards->setAction(MoveTowardsAction{});
    m_ui->toolButtonExecuteCode->setAction(ExecuteCodeAction{});
}

ActionsContainerWidget::~ActionsContainerWidget() = default;

ActionsContainer *ActionsContainerWidget::actionsContainer() const
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
        MoveFixedDialog dialog{*ptr, this};
        if (dialog.exec() == QDialog::Accepted)
            emit changed();
    }
    else if (auto ptr = std::get_if<ExecuteCodeAction>(action))
    {
        CodeActionDialog dialog{*ptr, this};
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
