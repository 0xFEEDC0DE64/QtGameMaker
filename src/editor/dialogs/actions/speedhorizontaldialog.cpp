#include "speedhorizontaldialog.h"
#include "ui_speedhorizontaldialog.h"

#include <QPushButton>

#include "editorguiutils.h"

SpeedHorizontalDialog::SpeedHorizontalDialog(SpeedHorizontalAction &action, ProjectTreeModel *projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::SpeedHorizontalDialog>()},
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

SpeedHorizontalDialog::~SpeedHorizontalDialog() = default;
