#include "speedverticaldialog.h"
#include "ui_speedverticaldialog.h"

#include <QPushButton>

#include "editorguiutils.h"

SpeedVerticalDialog::SpeedVerticalDialog(SpeedVerticalAction &action, ProjectTreeModel *projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::SpeedVerticalDialog>()},
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
}

SpeedVerticalDialog::~SpeedVerticalDialog() = default;
