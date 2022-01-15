#include "deletemomentsdialog.h"
#include "ui_deletemomentsdialog.h"

DeleteMomentsDialog::DeleteMomentsDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::DeleteMomentsDialog>()}
{
    m_ui->setupUi(this);
}

DeleteMomentsDialog::DeleteMomentsDialog(int fromMoment, int tillMoment, QWidget *parent) :
    DeleteMomentsDialog{parent}
{
    setFromMoment(fromMoment);
    setTillMoment(tillMoment);
}

DeleteMomentsDialog::~DeleteMomentsDialog() = default;

int DeleteMomentsDialog::fromMoment() const
{
    return m_ui->spinBoxFromMoment->value();
}

void DeleteMomentsDialog::setFromMoment(int fromMoment)
{
    m_ui->spinBoxFromMoment->setValue(fromMoment);
}

int DeleteMomentsDialog::tillMoment() const
{
    return m_ui->spinBoxTillMoment->value();
}

void DeleteMomentsDialog::setTillMoment(int tillMoment)
{
    m_ui->spinBoxTillMoment->setValue(tillMoment);
}

void DeleteMomentsDialog::setFromTillMoment(int fromMoment, int tillMoment)
{
    setFromMoment(fromMoment);
    setTillMoment(tillMoment);
}
