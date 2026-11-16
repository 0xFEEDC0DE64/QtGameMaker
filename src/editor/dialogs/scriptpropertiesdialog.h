#include "codeeditordialog.h"
#include "projectchangesapplier.h"

class QLineEdit;
struct Script;
class ProjectTreeModel;
class MainWindow;

class ScriptPropertiesDialog : public CodeEditorDialog, public ProjectChangesApplier
{
    Q_OBJECT

public:
    ScriptPropertiesDialog(Script &script, ProjectTreeModel &projectModel, MainWindow &mainWindow);

    void accept() override;
    bool applyChanges() override;

private slots:
    void scriptNameChanged(const Script &script);

private:
    Script &m_script;
    ProjectTreeModel &m_projectModel;

    QLineEdit * const m_lineEditName;
};
