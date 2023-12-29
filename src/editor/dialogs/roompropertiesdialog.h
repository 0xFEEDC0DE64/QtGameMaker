#pragma once

#include <QDialog>

#include <memory>

#include "projectcontainer.h"

class QSpinBox;
class QLabel;
class QMenu;
namespace Ui { class RoomPropertiesDialog; }
struct Room;
struct Sprite;
struct Object;
class ProjectTreeModel;
class RoomScene;

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
    void spritePixmapsChanged(const Sprite &sprite);
    void objectNameChanged(const Object &object, const QString &oldName);
    void objectAboutToBeRemoved(const Object &object);
    void objectSpriteNameChanged(const Object &object);

    void objectsMenuAboutToShow();

    void cursorMoved(const QPoint &point);

private:
    void setObject(const Object &object);
    void updateTitle();

    const std::unique_ptr<Ui::RoomPropertiesDialog> m_ui;

    Room &m_room;
    ProjectTreeModel &m_projectModel;

    std::vector<Room::Object> m_objects;

    const std::unique_ptr<RoomScene> m_scene;

    QString m_creationCode;

    bool m_unsavedChanges{};

    QSpinBox * const m_spinBoxSnapX;
    QSpinBox * const m_spinBoxSnapY;

    QMenu * const m_menuObjects;

    QLabel * const m_labelX;
    QLabel * const m_labelY;

    const Object *m_selectedObject{};
};
