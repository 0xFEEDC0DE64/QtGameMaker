#include "setgravitydialog.h"
#include "ui_setgravitydialog.h"

#include <QPushButton>

#include "editorguiutils.h"
#include "actiondialogutils.h"

SetGravityDialog::SetGravityDialog(SetGravityAction &action, ProjectTreeModel *projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::SetGravityDialog>()},
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
    m_ui->spinBoxDirection->setValue(m_action.direction);
    m_ui->spinBoxGravity->setValue(m_action.gravity);
    m_ui->checkBoxRelative->setChecked(m_action.relative);
}

SetGravityDialog::~SetGravityDialog() = default;

void SetGravityDialog::accept()
{
    if (!saveActionTarget(*m_ui, m_action.appliesTo, this))
        return;
    m_action.direction = m_ui->spinBoxDirection->value();
    m_action.gravity = m_ui->spinBoxGravity->value();
    m_action.relative = m_ui->checkBoxRelative->isChecked();
    QDialog::accept();
}
