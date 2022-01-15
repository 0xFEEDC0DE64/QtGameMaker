#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class RoomPropertiesDialog; }
struct Room;
class ProjectTreeModel;

class RoomPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoomPropertiesDialog(Room &room, ProjectTreeModel &projectModel, QWidget *parent = nullptr);
    ~RoomPropertiesDialog();

private:
    const std::unique_ptr<Ui::RoomPropertiesDialog> m_ui;
};
