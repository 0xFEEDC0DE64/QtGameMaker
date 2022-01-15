#include "scriptpropertiesdialog.h"
#include "ui_codeeditordialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QDebug>

#include "projectcontainer.h"
#include "models/projecttreemodel.h"

ScriptPropertiesDialog::ScriptPropertiesDialog(Script &script, ProjectTreeModel &projectModel, QWidget *parent) :
    CodeEditorDialog{tr("Script Properties: %0").arg(script.name), parent},
    m_script{script},
    m_projectModel{projectModel},
    m_lineEditName{new QLineEdit{this}}
{
    {
        auto label = new QLabel{tr("&Name:"), this};
        label->setBuddy(m_lineEditName);
        addToolbarWidget(label);
    }
    m_lineEditName->setMaximumWidth(100);
    addToolbarWidget(m_lineEditName);


    m_lineEditName->setText(m_script.name);
    setScript(m_script.script);

    connect(&m_projectModel, &ProjectTreeModel::scriptNameChanged,
            this, &ScriptPropertiesDialog::scriptNameChanged);

    connect(m_lineEditName, &QLineEdit::textChanged,
            this, &ScriptPropertiesDialog::changed);
}

void ScriptPropertiesDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (m_script.name != m_lineEditName->text())
    {
        if (!m_projectModel.rename<Script>(m_script, m_lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Script failed!"), tr("Renaming Script failed!"));
            return;
        }
    }

    m_script.script = script();

    CodeEditorDialog::accept();
}

void ScriptPropertiesDialog::scriptNameChanged(const Script &script)
{
    if (&script != &m_script)
        return;

    setTitle(tr("Script Properties: %0").arg(script.name));

    {
        QSignalBlocker blocker{m_lineEditName};
        m_lineEditName->setText(script.name);
    }
}
