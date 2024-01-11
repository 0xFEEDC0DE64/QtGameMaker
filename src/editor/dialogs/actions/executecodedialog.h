#pragma once

#include "dialogs/codeeditordialog.h"

class QRadioButton;
struct ExecuteCodeAction;

class ExecuteCodeDialog : public CodeEditorDialog
{
    Q_OBJECT

public:
    explicit ExecuteCodeDialog(ExecuteCodeAction &action, QWidget *parent = nullptr);

    void accept() override;

private:
    ExecuteCodeAction &m_action;

    QRadioButton * const m_radioButtonSelf;
    QRadioButton * const m_radioButtonOther;
    QRadioButton * const m_radioButtonObject;
};
