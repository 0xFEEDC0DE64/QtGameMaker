#include "userdefinedconstantsdialog.h"
#include "ui_userdefinedconstantsdialog.h"

#include "projectcontainer.h"
#include "constantsmodel.h"

UserDefinedConstantsDialog::UserDefinedConstantsDialog(ProjectContainer &project, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::UserDefinedConstantsDialog>()},
    m_project{project},
    m_model{std::make_unique<ConstantsModel>(m_project, this)}
{
    m_ui->setupUi(this);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    m_ui->treeView->setModel(m_model.get());
}

UserDefinedConstantsDialog::~UserDefinedConstantsDialog() = default;
