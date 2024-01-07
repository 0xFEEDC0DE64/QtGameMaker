#include "includedfilesdialog.h"
#include "ui_includedfilesdialog.h"

#include "projectcontainer.h"
#include "models/includedfilesmodel.h"
#include "dialogs/includedfilepropertiesdialog.h"

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

    connect(m_ui->pushButtonAdd, &QAbstractButton::clicked, this, &IncludedFilesDialog::addPressed);
}

IncludedFilesDialog::~IncludedFilesDialog() = default;

void IncludedFilesDialog::addPressed()
{
    IncludedFile includedFile;
    IncludedFilePropertiesDialog dialog{includedFile, this};
    if (dialog.exec() == QDialog::Accepted)
    {
        // TODO
    }
}
