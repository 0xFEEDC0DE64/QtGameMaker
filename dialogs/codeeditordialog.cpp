#include "codeeditordialog.h"
#include "ui_codeeditordialog.h"

#include <QDebug>
#include <QFont>
#include <QLabel>
#include <QTextBlock>
#include <QMessageBox>
#include <QSignalBlocker>

#include "jshighlighter.h"

CodeEditorDialog::CodeEditorDialog(const QString &title, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::CodeEditorDialog>()},
    m_title{title},
    m_labelPosition{new QLabel{this}}
{
    m_ui->setupUi(this);

    updateTitle();

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

    updatePosition();

    connect(m_ui->actionLoad, &QAction::triggered,
            this, &CodeEditorDialog::load);
    connect(m_ui->actionSave, &QAction::triggered,
            this, &CodeEditorDialog::save);
    connect(m_ui->actionPrint, &QAction::triggered,
            this, &CodeEditorDialog::print);

    connect(m_ui->codeEdit, &QPlainTextEdit::textChanged,
            this, &CodeEditorDialog::changed);
    connect(m_ui->codeEdit, &QPlainTextEdit::textChanged,
            this, &CodeEditorDialog::updatePosition);
    connect(m_ui->codeEdit, &QPlainTextEdit::cursorPositionChanged,
            this, &CodeEditorDialog::updatePosition);
}

CodeEditorDialog::~CodeEditorDialog() = default;

void CodeEditorDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    QDialog::accept();
}

void CodeEditorDialog::reject()
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

QString CodeEditorDialog::script() const
{
    return m_ui->codeEdit->toPlainText();
}

void CodeEditorDialog::setScript(const QString &script)
{
    QSignalBlocker blocker{m_ui->codeEdit};
    m_ui->codeEdit->setPlainText(script);
}

void CodeEditorDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void CodeEditorDialog::addToolbarWidget(QWidget *widget)
{
    m_ui->toolBar->addWidget(widget);
}

void CodeEditorDialog::load()
{

}

void CodeEditorDialog::save()
{

}

void CodeEditorDialog::print()
{

}

void CodeEditorDialog::updatePosition()
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

void CodeEditorDialog::updateTitle()
{
    setWindowTitle(tr("%0%1")
                       .arg(m_title)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}
