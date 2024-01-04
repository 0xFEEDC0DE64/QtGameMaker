#pragma once

#include "codeeditordialog.h"

class QRadioButton;
struct ExecuteCodeAction;

class CodeActionDialog : public CodeEditorDialog
{
    Q_OBJECT

public:
    explicit CodeActionDialog(ExecuteCodeAction &action, QWidget *parent = nullptr);

    void accept() override;

private:
    ExecuteCodeAction &m_action;

    QRadioButton * const m_radioButtonSelf;
    QRadioButton * const m_radioButtonOther;
    QRadioButton * const m_radioButtonObject;
};
