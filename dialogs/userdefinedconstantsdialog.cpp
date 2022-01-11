#include "userdefinedconstantsdialog.h"
#include "ui_userdefinedconstantsdialog.h"

#include <QDebug>
#include <QMessageBox>

#include "projectcontainer.h"
#include "constantsmodel.h"

UserDefinedConstantsDialog::UserDefinedConstantsDialog(ProjectContainer &project, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::UserDefinedConstantsDialog>()},
    m_project{project},
    m_model{std::make_unique<ConstantsModel>(m_project, this)}
{
    m_ui->setupUi(this);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    m_ui->treeView->setModel(m_model.get());

    connect(m_ui->pushButtonInsert, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::insert);
    connect(m_ui->pushButtonAdd, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::add);
    connect(m_ui->pushButtonDelete, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::delete_);
    connect(m_ui->pushButtonClear, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::clear);
    connect(m_ui->pushButtonUp, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::up);
    connect(m_ui->pushButtonDown, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::down);
    connect(m_ui->pushButtonSort, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::sort);
    connect(m_ui->pushButtonLoad, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::load);
    connect(m_ui->pushButtonSave, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::save);
}

UserDefinedConstantsDialog::~UserDefinedConstantsDialog() = default;

void UserDefinedConstantsDialog::insert()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::add()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::delete_()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::clear()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::up()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::down()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::sort()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::load()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::save()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}
