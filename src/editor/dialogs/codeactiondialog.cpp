#include "codeactiondialog.h"

#include <QRadioButton>
#include <QLabel>
#include <QMessageBox>

#include "projectcontainer.h"

CodeActionDialog::CodeActionDialog(ExecuteCodeAction &action, QWidget *parent) :
    CodeEditorDialog{tr("Execute Code"), parent},
    m_action{action},
    m_radioButtonSelf{new QRadioButton{tr("Self"), this}},
    m_radioButtonOther{new QRadioButton{tr("Other"), this}},
    m_radioButtonObject{new QRadioButton{tr("Object"), this}}
{
#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);

    addToolbarWidget(new QLabel{tr("Applies to:"), this});
    addToolbarWidget(m_radioButtonSelf);
    addToolbarWidget(m_radioButtonOther);
    addToolbarWidget(m_radioButtonObject);

    m_radioButtonSelf->setChecked(m_action.appliesTo == ExecuteCodeAction::AppliesTo::Self);
    m_radioButtonOther->setChecked(m_action.appliesTo == ExecuteCodeAction::AppliesTo::Other);
    m_radioButtonObject->setChecked(m_action.appliesTo == ExecuteCodeAction::AppliesTo::Object);

    setScript(m_action.script);

    connect(m_radioButtonSelf, &QRadioButton::toggled,
            this, &CodeActionDialog::changed);
    connect(m_radioButtonOther, &QRadioButton::toggled,
            this, &CodeActionDialog::changed);
    connect(m_radioButtonObject, &QRadioButton::toggled,
            this, &CodeActionDialog::changed);
}

void CodeActionDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (m_radioButtonSelf->isChecked())
        m_action.appliesTo = ExecuteCodeAction::AppliesTo::Self;
    else if (m_radioButtonOther->isChecked())
        m_action.appliesTo = ExecuteCodeAction::AppliesTo::Other;
    else if (m_radioButtonObject->isChecked())
        m_action.appliesTo = ExecuteCodeAction::AppliesTo::Object;
    else
    {
        QMessageBox::warning(this, tr("No Applies To selected!"), tr("No Applies To selected!"));
        return;
    }

    m_action.script = script();

    CodeEditorDialog::accept();
}
