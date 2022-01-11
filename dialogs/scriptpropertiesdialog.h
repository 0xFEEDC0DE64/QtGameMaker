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

private slots:
    void scriptNameChanged(const Script &script);

private:
    Script &m_script;
    ProjectTreeModel &m_projectModel;

    QLineEdit * const m_lineEditName;
};
