#include "includedfilesdialog.h"
#include "ui_includedfilesdialog.h"

#include "projectcontainer.h"
#include "includedfilesmodel.h"

IncludedFilesDialog::IncludedFilesDialog(ProjectContainer &project, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::IncludedFilesDialog>()},
    m_project{project},
    m_model{std::make_unique<IncludedFilesModel>(m_project, this)}
{
    m_ui->setupUi(this);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    m_ui->listView->setModel(m_model.get());
}

IncludedFilesDialog::~IncludedFilesDialog() = default;
