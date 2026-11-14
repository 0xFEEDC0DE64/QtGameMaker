#include "movetowardsdialog.h"
#include "ui_movetowardsdialog.h"

#include <QPushButton>

#include "editorguiutils.h"
#include "actiondialogutils.h"

MoveTowardsDialog::MoveTowardsDialog(MoveTowardsAction &action, ProjectTreeModel *projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::MoveTowardsDialog>()},
    m_action{action}
{
    m_ui->setupUi(this);

    {
        QSizePolicy sp_retain = m_ui->widgetObject->sizePolicy();
        sp_retain.setRetainSizeWhenHidden(true);
        m_ui->widgetObject->setSizePolicy(sp_retain);
    }
    m_ui->widgetObject->setProjectModel(projectModel);

    m_ui->widgetObject->setEmptySelectionText(tr("self"));
    m_ui->widgetObject->setShowClearObjectAction(false);

    improveButtonBox(m_ui->buttonBox);
    loadActionTarget(*m_ui, m_action.appliesTo);
    m_ui->spinBoxX->setValue(m_action.x);
    m_ui->spinBoxY->setValue(m_action.y);
    m_ui->spinBoxSpeed->setValue(m_action.speed);
    m_ui->checkBoxRelative->setChecked(m_action.relative);
}

MoveTowardsDialog::~MoveTowardsDialog() = default;

void MoveTowardsDialog::accept()
{
    if (!saveActionTarget(*m_ui, m_action.appliesTo, this))
        return;
    m_action.x = m_ui->spinBoxX->value();
    m_action.y = m_ui->spinBoxY->value();
    m_action.speed = m_ui->spinBoxSpeed->value();
    m_action.relative = m_ui->checkBoxRelative->isChecked();
    QDialog::accept();
}
