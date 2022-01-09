#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class FontPropertiesDialog; }
struct Font;

class FontPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FontPropertiesDialog(Font &font, QWidget *parent = nullptr);
    ~FontPropertiesDialog();

    void accept() override;
    void reject() override;

private slots:
    void normalRange();
    void digitsRange();
    void allRange();
    void lettersRange();

    void changed();

private:
    QFont currentFont() const;

    const std::unique_ptr<Ui::FontPropertiesDialog> m_ui;

    Font &m_font;

    bool m_unsavedChanges{};
};
