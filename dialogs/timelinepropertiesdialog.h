#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class TimeLinePropertiesDialog; }
struct TimeLine;
class ProjectTreeModel;

class TimeLinePropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimeLinePropertiesDialog(TimeLine &timeLine, ProjectTreeModel &projectModel, QWidget *parent = nullptr);
    ~TimeLinePropertiesDialog();

private:
    const std::unique_ptr<Ui::TimeLinePropertiesDialog> m_ui;
};
