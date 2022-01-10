#include "objectinformationdialog.h"
#include "ui_objectinformationdialog.h"

ObjectInformationDialog::ObjectInformationDialog(ProjectContainer &project, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ObjectInformationDialog>()},
    m_project{project}
{
    m_ui->setupUi(this);
}

ObjectInformationDialog::~ObjectInformationDialog() = default;
