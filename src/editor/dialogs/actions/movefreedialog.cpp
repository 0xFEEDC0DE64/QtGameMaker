#include "movefreedialog.h"
#include "ui_movefreedialog.h"

#include <QPushButton>

#include "editorguiutils.h"
#include "actiondialogutils.h"

MoveFreeDialog::MoveFreeDialog(MoveFreeAction &action, ProjectTreeModel *projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::MoveFreeDialog>()},
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
    m_ui->spinBoxSpeed->setValue(m_action.speed);
    m_ui->checkBoxRelative->setChecked(m_action.relative);
}

MoveFreeDialog::~MoveFreeDialog() = default;

void MoveFreeDialog::accept()
{
    if (!saveActionTarget(*m_ui, m_action.appliesTo, this))
        return;
    m_action.direction = m_ui->spinBoxDirection->value();
    m_action.speed = m_ui->spinBoxSpeed->value();
    m_action.relative = m_ui->checkBoxRelative->isChecked();
    QDialog::accept();
}
