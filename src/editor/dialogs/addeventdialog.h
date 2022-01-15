#pragma once

#include <QDialog>

#include <optional>

#include "projectcontainer.h"

namespace Ui { class AddEventDialog; }

class AddEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEventDialog(QWidget *parent = nullptr);
    ~AddEventDialog();

    Object::EventType eventType() const { return *m_eventType; }

    void accept() override;
    void reject() override;

private:
    const std::unique_ptr<Ui::AddEventDialog> m_ui;

    std::optional<Object::EventType> m_eventType;
};
