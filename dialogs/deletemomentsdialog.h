#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class DeleteMomentsDialog; }

class DeleteMomentsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteMomentsDialog(QWidget *parent = nullptr);
    explicit DeleteMomentsDialog(int fromMoment, int tillMoment, QWidget *parent = nullptr);
    ~DeleteMomentsDialog();

    int fromMoment() const;
    void setFromMoment(int fromMoment);

    int tillMoment() const;
    void setTillMoment(int tillMoment);

    void setFromTillMoment(int fromMoment, int tillMoment);

private:
    const std::unique_ptr<Ui::DeleteMomentsDialog> m_ui;
};
