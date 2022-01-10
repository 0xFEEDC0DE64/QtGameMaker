#pragma once

#include <QDialog>

#include <memory>

class QLabel;
namespace Ui { class CodeEditorDialog; }

class CodeEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CodeEditorDialog(QWidget *parent = nullptr);
    ~CodeEditorDialog();

private slots:
    void load();
    void save();
    void print();

    void updatePosition();

protected:
    const std::unique_ptr<Ui::CodeEditorDialog> m_ui;

private:
    QLabel * const m_labelPosition;
};
