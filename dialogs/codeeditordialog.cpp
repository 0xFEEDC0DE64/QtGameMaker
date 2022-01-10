#include "codeeditordialog.h"
#include "ui_codeeditordialog.h"

#include <QDebug>
#include <QFont>
#include <QLabel>
#include <QTextBlock>

#include "jshighlighter.h"

CodeEditorDialog::CodeEditorDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::CodeEditorDialog>()},
    m_labelPosition{new QLabel{this}}
{
    m_ui->setupUi(this);

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
            this, &CodeEditorDialog::updatePosition);
    connect(m_ui->codeEdit, &QPlainTextEdit::cursorPositionChanged,
            this, &CodeEditorDialog::updatePosition);
}

CodeEditorDialog::~CodeEditorDialog() = default;

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
