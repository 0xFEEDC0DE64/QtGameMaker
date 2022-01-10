#include "scriptpropertiesdialog.h"
#include "ui_codeeditordialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QDebug>

#include "projectcontainer.h"
#include "projecttreemodel.h"

ScriptPropertiesDialog::ScriptPropertiesDialog(Script &script, ProjectTreeModel &projectModel, QWidget *parent) :
    CodeEditorDialog{parent},
    m_script{script},
    m_projectModel{projectModel},
    m_lineEditName{new QLineEdit{this}}
{
    updateTitle();

    {
        auto label = new QLabel{tr("Name:"), this};
        label->setBuddy(m_lineEditName);
        m_ui->toolBar->addWidget(label);
    }
    m_lineEditName->setMaximumWidth(100);
    m_ui->toolBar->addWidget(m_lineEditName);


    m_lineEditName->setText(m_script.name);
    m_ui->codeEdit->setPlainText(m_script.script);

    connect(&m_projectModel, &ProjectTreeModel::scriptNameChanged,
            this, &ScriptPropertiesDialog::scriptNameChanged);

    connect(m_lineEditName, &QLineEdit::textChanged,
            this, &ScriptPropertiesDialog::changed);
    connect(m_ui->codeEdit, &QPlainTextEdit::textChanged,
            this, &ScriptPropertiesDialog::changed);
}

void ScriptPropertiesDialog::accept()
{
    if (m_script.name != m_lineEditName->text())
    {
        if (!m_projectModel.rename<Script>(m_script, m_lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Script failed!"), tr("Renaming Script failed!"));
            return;
        }
    }

    m_script.script = m_ui->codeEdit->toPlainText();

    CodeEditorDialog::accept();
}

void ScriptPropertiesDialog::reject()
{
    if (!m_unsavedChanges)
    {
        CodeEditorDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Script has been modified."),
        tr("Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save
    );
    switch (result)
    {
    case QMessageBox::Save:
        accept();
        return;
    case QMessageBox::Discard:
        CodeEditorDialog::reject();
        return;
    case QMessageBox::Cancel:
        return;
    default:
        qWarning() << "unexpected dialog result" << result;
    }
}

void ScriptPropertiesDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void ScriptPropertiesDialog::scriptNameChanged(const Script &script)
{
    if (&script != &m_script)
        return;

    {
        QSignalBlocker blocker{m_lineEditName};
        m_lineEditName->setText(script.name);
    }

    updateTitle();
}

void ScriptPropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Script Properties: %0%1")
                       .arg(m_script.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}

