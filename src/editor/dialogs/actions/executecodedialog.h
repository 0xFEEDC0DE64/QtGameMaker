#pragma once

#include "dialogs/codeeditordialog.h"

class QRadioButton;
struct ExecuteCodeAction;
class ProjectTreeModel;
class ObjectSelectorWidget;

class ExecuteCodeDialog : public CodeEditorDialog
{
    Q_OBJECT

public:
    explicit ExecuteCodeDialog(ExecuteCodeAction &action, ProjectTreeModel *projectModel, QWidget *parent = nullptr);

    void accept() override;

private:
    ExecuteCodeAction &m_action;

    QRadioButton * const m_radioButtonSelf;
    QRadioButton * const m_radioButtonOther;
    QRadioButton * const m_radioButtonObject;

    ObjectSelectorWidget * const m_objectSelector;
};
