#include "objectpropertiesdialog.h"
#include "ui_objectpropertiesdialog.h"

ObjectPropertiesDialog::ObjectPropertiesDialog(Object &object, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ObjectPropertiesDialog>()}
{
    m_ui->setupUi(this);
}

ObjectPropertiesDialog::~ObjectPropertiesDialog() = default;
