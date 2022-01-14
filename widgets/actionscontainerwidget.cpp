#include "actionscontainerwidget.h"
#include "ui_actionscontainerwidget.h"

#include <QDebug>
#include <QMessageBox>
#include <QMenu>

#include "models/actionscontainermodel.h"
#include "dialogs/codeactiondialog.h"

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

    CodeActionDialog dialog{*action, this};
    if (dialog.exec() == QDialog::Accepted)
        emit changed();
}

void ActionsContainerWidget::actionsContextMenuRequested(const QPoint &pos)
{
    const auto index = m_ui->listViewActions->indexAt(pos);
    auto action = index.isValid() ? m_actionsModel->getAction(index) : nullptr;

    QMenu menu{this};
    menu.addAction(tr("&Edit Values"), this, [this,index](){ actionDoubleClicked(index); })->setEnabled(action);
    menu.addSeparator();
    menu.addAction(tr("C&ut"))->setEnabled(false);
    menu.addAction(tr("&Copy"))->setEnabled(false);
    menu.addAction(tr("&Paste"))->setEnabled(false);
    menu.addSeparator();
    menu.addAction(tr("&Delete"))->setEnabled(false);
    menu.addSeparator();
    menu.addAction(tr("Select &All"))->setEnabled(false);
    menu.addSeparator();
    menu.addAction(tr("C&lear"))->setEnabled(false);
    menu.exec(m_ui->listViewActions->viewport()->mapToGlobal(pos));
}
