#include "roompropertiesdialog.h"
#include "ui_roompropertiesdialog.h"

RoomPropertiesDialog::RoomPropertiesDialog(Room &room, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::RoomPropertiesDialog>()}
{
    Q_UNUSED(room)
    Q_UNUSED(projectModel)

    m_ui->setupUi(this);
}

RoomPropertiesDialog::~RoomPropertiesDialog() = default;
