#include "timelinepropertiesdialog.h"
#include "ui_timelinepropertiesdialog.h"

TimeLinePropertiesDialog::TimeLinePropertiesDialog(TimeLine &timeLine, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::TimeLinePropertiesDialog>()}
{
    m_ui->setupUi(this);
}

TimeLinePropertiesDialog::~TimeLinePropertiesDialog() = default;
