#include "roompropertiesdialog.h"
#include "ui_roompropertiesdialog.h"

RoomPropertiesDialog::RoomPropertiesDialog(Room &room, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::RoomPropertiesDialog>()}
{
    m_ui->setupUi(this);
}

RoomPropertiesDialog::~RoomPropertiesDialog() = default;
