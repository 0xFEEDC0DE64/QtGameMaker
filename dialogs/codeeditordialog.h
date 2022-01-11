#pragma once

#include <QDialog>

#include <memory>

class QLabel;
namespace Ui { class CodeEditorDialog; }

class CodeEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CodeEditorDialog(const QString &title, QWidget *parent = nullptr);
    ~CodeEditorDialog();

    void accept() override;
    void reject() override;

    const QString &title() const { return m_title; }
    void setTitle(const QString &title) { if (m_title == title) return; m_title = title; updateTitle(); }

    QString script() const;
    void setScript(const QString &script);

protected slots:
    void changed();

protected:
    bool m_unsavedChanges{};

    void addToolbarWidget(QWidget *widget);

private slots:
    void load();
    void save();
    void print();

    void updatePosition();

private:
    void updateTitle();

    const std::unique_ptr<Ui::CodeEditorDialog> m_ui;

    QString m_title;

    QLabel * const m_labelPosition;
};
