#pragma once

#include <QPlainTextEdit>

class CodeEditorWidget : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditorWidget(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *lineNumberArea;
};

class LineNumberArea : public QWidget
{
    Q_OBJECT

public:
    LineNumberArea(CodeEditorWidget *editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditorWidget *codeEditor;
};
