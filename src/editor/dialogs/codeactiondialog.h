#pragma once

#include "codeeditordialog.h"

class QRadioButton;
struct Action;

class CodeActionDialog : public CodeEditorDialog
{
    Q_OBJECT

public:
    explicit CodeActionDialog(Action &action, QWidget *parent = nullptr);

    void accept() override;

private:
    Action &m_action;

    QRadioButton * const m_radioButtonSelf;
    QRadioButton * const m_radioButtonOther;
    QRadioButton * const m_radioButtonObject;
};
