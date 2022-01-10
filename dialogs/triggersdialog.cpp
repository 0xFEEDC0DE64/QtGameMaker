#include "triggersdialog.h"
#include "ui_triggersdialog.h"

#include "projectcontainer.h"
#include "triggersmodel.h"
#include "triggerconditiondialog.h"

TriggersDialog::TriggersDialog(ProjectContainer &project, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::TriggersDialog>()},
    m_project{project},
    m_model{std::make_unique<TriggersModel>(project, this)}
{
    m_ui->setupUi(this);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    m_ui->listView->setModel(m_model.get());

    connect(m_ui->pushButtonUseCodeEditor, &QAbstractButton::pressed,
            this, &TriggersDialog::openCodeEditor);
}

TriggersDialog::~TriggersDialog() = default;

void TriggersDialog::openCodeEditor()
{
    TriggerConditionDialog dialog{this};
    dialog.exec();
}
