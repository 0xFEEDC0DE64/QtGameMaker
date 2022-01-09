#pragma once

#include <QDialog>

#include <memory>

class QLineEdit;
class QLabel;
namespace Ui { class ScriptPropertiesDialog; }
struct Script;
class ProjectTreeModel;

class ScriptPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptPropertiesDialog(Script &script, ProjectTreeModel &projectModel, QWidget *parent = nullptr);
    ~ScriptPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void changed();

    void load();
    void save();
    void print();

    void updatePosition();

    void scriptNameChanged(const Script &script);

private:
    void updateTitle();

    const std::unique_ptr<Ui::ScriptPropertiesDialog> m_ui;

    Script &m_script;
    ProjectTreeModel &m_projectModel;

    bool m_unsavedChanges{};

    QLineEdit * const m_lineEditName;

    QLabel * const m_labelPosition;
};
