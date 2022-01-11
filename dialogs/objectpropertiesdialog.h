#pragma once

#include <QDialog>

#include <memory>

class QMenu;
namespace Ui { class ObjectPropertiesDialog; }
struct Object;
class ProjectTreeModel;
class ObjectEventsModel;
class ObjectActionsModel;
class Sprite;

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

    void changed();

    void objectNameChanged(const Object &object);

    void spritesMenuAboutToShow();

    void clearSprite();
    void setSprite(const Sprite &sprite);

private:
    void updateTitle();

    const std::unique_ptr<Ui::ObjectPropertiesDialog> m_ui;

    Object &m_object;
    ProjectTreeModel &m_projectModel;

    const std::unique_ptr<ObjectEventsModel> m_eventsModel;
    const std::unique_ptr<ObjectActionsModel> m_actionsModel;

    QMenu * const m_spritesMenu;

    QString m_spriteName;

    bool m_unsavedChanges{};
};
