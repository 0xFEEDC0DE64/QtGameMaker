#include "movefixeddialog.h"
#include "ui_movefixeddialog.h"

#include <QPushButton>

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

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});
}

MoveFixedDialog::~MoveFixedDialog() = default;
