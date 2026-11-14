#include "movefixeddialog.h"
#include "ui_movefixeddialog.h"

#include <QPushButton>

#include "editorguiutils.h"
#include "actiondialogutils.h"

MoveFixedDialog::MoveFixedDialog(MoveFixedAction &action, ProjectTreeModel *projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::MoveFixedDialog>()},
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
    const std::array buttons{m_ui->pushButton, m_ui->pushButton_2, m_ui->pushButton_3,
                             m_ui->pushButton_4, m_ui->pushButton_5, m_ui->pushButton_6,
                             m_ui->pushButton_7, m_ui->pushButton_8, m_ui->pushButton_9};
    for (std::size_t i = 0; i < buttons.size(); ++i)
        buttons[i]->setChecked(m_action.directions[i]);
    m_ui->spinBox->setValue(m_action.speed);
    m_ui->checkBoxRelative->setChecked(m_action.relative);
}

MoveFixedDialog::~MoveFixedDialog() = default;

void MoveFixedDialog::accept()
{
    if (!saveActionTarget(*m_ui, m_action.appliesTo, this))
        return;
    const std::array buttons{m_ui->pushButton, m_ui->pushButton_2, m_ui->pushButton_3,
                             m_ui->pushButton_4, m_ui->pushButton_5, m_ui->pushButton_6,
                             m_ui->pushButton_7, m_ui->pushButton_8, m_ui->pushButton_9};
    for (std::size_t i = 0; i < buttons.size(); ++i)
        m_action.directions[i] = buttons[i]->isChecked();
    m_action.speed = m_ui->spinBox->value();
    m_action.relative = m_ui->checkBoxRelative->isChecked();
    QDialog::accept();
}
