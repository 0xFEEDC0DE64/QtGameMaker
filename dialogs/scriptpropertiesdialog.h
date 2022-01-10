#include "codeeditordialog.h"

class QLineEdit;
struct Script;
class ProjectTreeModel;

class ScriptPropertiesDialog : public CodeEditorDialog
{
    Q_OBJECT

public:
    ScriptPropertiesDialog(Script &script, ProjectTreeModel &projectModel, QWidget *parent = nullptr);

    void accept() override;
    void reject() override;

private slots:
    void changed();

    void scriptNameChanged(const Script &script);

private:
    void updateTitle();

    Script &m_script;
    ProjectTreeModel &m_projectModel;

    bool m_unsavedChanges{};

    QLineEdit * const m_lineEditName;
};
