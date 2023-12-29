#pragma once

#include <QDialog>

#include <memory>
#include <map>

#include "projectcontainer.h"

class QMenu;
namespace Ui { class ObjectPropertiesDialog; }
class ProjectTreeModel;
class ObjectEventsModel;
class MainWindow;

class ObjectPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectPropertiesDialog(Object &object, ProjectTreeModel &projectModel, MainWindow *mainWindow);
    ~ObjectPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void newSprite();
    void editSprite();
    void showInformation();
    void addEvent();
    void deleteEvent();
    void replaceEvent();
    void duplicateEvent();

    void changed();

    void objectNameChanged(const Object &object);
    void spriteNameChanged(const Sprite &sprite, const QString &oldName);
    void spriteAboutToBeRemoved(const Sprite &sprite);
    void spritePixmapsChanged(const Sprite &sprite);

    void spritesMenuAboutToShow();
    void currentEventChanged(const QModelIndex &index);
    void eventsContextMenuRequested(const QPoint &pos);
    void rowsInserted(const QModelIndex &parent, int first);

    void clearSprite();
    void setSprite(const Sprite &sprite);

private:
    void updateTitle();
    void updateSpritePreview();
    void updateSpritePreview(const std::vector<QPixmap> &pixmaps);

    const std::unique_ptr<Ui::ObjectPropertiesDialog> m_ui;

    Object &m_object;
    ProjectTreeModel &m_projectModel;
    MainWindow * const m_mainWindow;

    Object::events_container_t m_events;
    Object::collision_events_container_t m_collisionEvents;

    const std::unique_ptr<ObjectEventsModel> m_eventsModel;

    QMenu * const m_menuSprites;

    QString m_spriteName;

    bool m_unsavedChanges{};
};
