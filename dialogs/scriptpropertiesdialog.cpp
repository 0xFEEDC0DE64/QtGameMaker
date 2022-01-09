#include "scriptpropertiesdialog.h"
#include "ui_scriptpropertiesdialog.h"

#include <QDebug>
#include <QFont>
#include <QLineEdit>
#include <QLabel>
#include <QTextBlock>
#include <QMessageBox>

#include "projectcontainer.h"
#include "jshighlighter.h"

ScriptPropertiesDialog::ScriptPropertiesDialog(Script &script, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ScriptPropertiesDialog>()},
    m_script{script},
    m_lineEditName{new QLineEdit{this}},
    m_labelPosition{new QLabel{this}}
{
    m_ui->setupUi(this);

    setWindowTitle(tr("Script Properties: %0").arg(m_script.name));

    {
        auto label = new QLabel{tr("Name:"), this};
        label->setBuddy(m_lineEditName);
        m_ui->toolBar->addWidget(label);
    }
    m_lineEditName->setMaximumWidth(100);
    m_ui->toolBar->addWidget(m_lineEditName);

    m_labelPosition->setFrameStyle(QFrame::Sunken);
    m_ui->statusbar->addWidget(m_labelPosition);

    {
        QFont font;
        font.setFamily("Consolas");
        font.setFixedPitch(true);
        font.setPointSize(10);
        m_ui->codeEdit->setFont(font);
    }

    new JSHighlighter{m_ui->codeEdit->document()};

    m_lineEditName->setText(m_script.name);
    m_ui->codeEdit->setPlainText(m_script.script);

    updatePosition();

    connect(m_ui->actionLoad, &QAction::triggered,
            this, &ScriptPropertiesDialog::load);
    connect(m_ui->actionSave, &QAction::triggered,
            this, &ScriptPropertiesDialog::save);
    connect(m_ui->actionPrint, &QAction::triggered,
            this, &ScriptPropertiesDialog::print);

    connect(m_lineEditName, &QLineEdit::textChanged,
            this, &ScriptPropertiesDialog::changed);
    connect(m_ui->codeEdit, &QPlainTextEdit::textChanged,
            this, &ScriptPropertiesDialog::changed);
    connect(m_ui->codeEdit, &QPlainTextEdit::textChanged,
            this, &ScriptPropertiesDialog::updatePosition);
    connect(m_ui->codeEdit, &QPlainTextEdit::cursorPositionChanged,
            this, &ScriptPropertiesDialog::updatePosition);
}

ScriptPropertiesDialog::~ScriptPropertiesDialog() = default;

void ScriptPropertiesDialog::accept()
{
    if (m_script.name != m_lineEditName->text())
    {
        QMessageBox::critical(this, tr("Not implemented"), tr("Changing the name is not yet implemented!"));
        return;
    }

    m_script.script = m_ui->codeEdit->toPlainText();

    QDialog::accept();
}

void ScriptPropertiesDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
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
        QDialog::reject();
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
        setWindowTitle(tr("Script Properties: %0*").arg(m_script.name));
        m_unsavedChanges = true;
    }
}

void ScriptPropertiesDialog::load()
{

}

void ScriptPropertiesDialog::save()
{

}

void ScriptPropertiesDialog::print()
{

}

void ScriptPropertiesDialog::updatePosition()
{
    auto cursor = m_ui->codeEdit->textCursor();
    auto position = cursor.position();
    cursor.movePosition(QTextCursor::StartOfLine);
    position -= cursor.position() - 1;

    int lines = 1;
    while (cursor.positionInBlock() > 0)
    {
        cursor.movePosition(QTextCursor::Up);
        //lines++;
    }
    QTextBlock block = cursor.block().previous();
    while (block.isValid())
    {
        lines += 1; //block.lineCount();
        block = block.previous();
    }

    m_labelPosition->setText(tr("%0/%1: %2").arg(lines).arg(m_ui->codeEdit->blockCount()).arg(position));
}
