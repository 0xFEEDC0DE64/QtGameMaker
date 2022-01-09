#pragma once

#include <QDialog>

#include <memory>

class QLineEdit;
class QLabel;
namespace Ui { class ScriptPropertiesDialog; }
struct Script;

class ScriptPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptPropertiesDialog(Script &script, QWidget *parent = nullptr);
    ~ScriptPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void changed();

    void load();
    void save();
    void print();

    void updatePosition();

private:
    const std::unique_ptr<Ui::ScriptPropertiesDialog> m_ui;

    Script &m_script;

    QLineEdit * const m_lineEditName;

    QLabel * const m_labelPosition;

    bool m_unsavedChanges{};
};
