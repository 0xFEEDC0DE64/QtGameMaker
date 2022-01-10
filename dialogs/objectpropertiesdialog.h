#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class ObjectPropertiesDialog; }
struct Object;
class ProjectTreeModel;

class ObjectPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ObjectPropertiesDialog(Object &object, ProjectTreeModel &projectModel, QWidget *parent = nullptr);
    ~ObjectPropertiesDialog();

private:
    const std::unique_ptr<Ui::ObjectPropertiesDialog> m_ui;
};
