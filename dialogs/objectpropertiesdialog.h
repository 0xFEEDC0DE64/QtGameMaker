#pragma once

#include <QDialog>

#include <memory>
#include <map>

#include "projectcontainer.h"

class QMenu;
namespace Ui { class ObjectPropertiesDialog; }
class ProjectTreeModel;
class ObjectEventsModel;

class ObjectPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectPropertiesDialog(Object &object, ProjectTreeModel &projectModel, QWidget *parent = nullptr);
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

    void spritesMenuAboutToShow();
    void currentEventChanged(const QModelIndex &index);
    void eventsContextMenuRequested(const QPoint &pos);

    void clearSprite();
    void setSprite(const Sprite &sprite);

private:
    void updateTitle();

    const std::unique_ptr<Ui::ObjectPropertiesDialog> m_ui;

    Object &m_object;
    ProjectTreeModel &m_projectModel;

    Object::events_container_t m_events;

    const std::unique_ptr<ObjectEventsModel> m_eventsModel;

    QMenu * const m_spritesMenu;

    QString m_spriteName;

    bool m_unsavedChanges{};
};
