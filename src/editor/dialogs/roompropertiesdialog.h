#pragma once

#include <QDialog>

#include <memory>

class QSpinBox;
class QLabel;
namespace Ui { class RoomPropertiesDialog; }
struct Room;
class ProjectTreeModel;

class RoomPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RoomPropertiesDialog(Room &room, ProjectTreeModel &projectModel, QWidget *parent = nullptr);
    ~RoomPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void undo();
    void clearInstances();
    void shiftInstances();
    void sortInstancesX();
    void sortInstancesY();
    void lockInstances();
    void unlockInstances();
    void editCreationCode();

    void changed();

    void roomNameChanged(const Room &room);

    void cursorMoved(const QPoint &point);

private:
    void updateTitle();

    const std::unique_ptr<Ui::RoomPropertiesDialog> m_ui;

    Room &m_room;
    ProjectTreeModel &m_projectModel;

    QString m_creationCode;

    bool m_unsavedChanges{};

    QSpinBox * const m_spinBoxSnapX;
    QSpinBox * const m_spinBoxSnapY;

    QLabel * const m_labelX;
    QLabel * const m_labelY;
};
