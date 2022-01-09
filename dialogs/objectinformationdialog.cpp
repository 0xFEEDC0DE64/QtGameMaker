#include "objectinformationdialog.h"
#include "ui_objectinformationdialog.h"

ObjectInformationDialog::ObjectInformationDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ObjectInformationDialog>()}
{
    m_ui->setupUi(this);
}

ObjectInformationDialog::~ObjectInformationDialog() = default;
