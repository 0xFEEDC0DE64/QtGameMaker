#pragma once

#include <QDialog>

#include <memory>

class QLabel;
namespace Ui { class PathPropertiesDialog; }
struct Path;

class PathPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathPropertiesDialog(Path &path, QWidget *parent = nullptr);
    ~PathPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void changed();

private:
    const std::unique_ptr<Ui::PathPropertiesDialog> m_ui;

    Path &m_path;

    QLabel * const m_labelX;
    QLabel * const m_labelY;
    QLabel * const m_labelArea;

    bool m_unsavedChanges{};
};
