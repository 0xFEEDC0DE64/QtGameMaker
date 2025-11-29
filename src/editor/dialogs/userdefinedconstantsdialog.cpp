#include "userdefinedconstantsdialog.h"
#include "ui_userdefinedconstantsdialog.h"

#include <QDebug>
#include <QMessageBox>

#include "projectcontainer.h"
#include "models/constantsmodel.h"
#include "editorguiutils.h"

UserDefinedConstantsDialog::UserDefinedConstantsDialog(ProjectContainer &project, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::UserDefinedConstantsDialog>()},
    m_project{project},
    m_model{std::make_unique<ConstantsModel>(m_project, this)}
{
    m_ui->setupUi(this);

    improveButtonBox(m_ui->buttonBox);

    m_ui->treeView->setModel(m_model.get());

    connect(m_ui->pushButtonInsert, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::insertConstant);
    connect(m_ui->pushButtonAdd, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::addConstant);
    connect(m_ui->pushButtonDelete, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::deleteConstant);
    connect(m_ui->pushButtonClear, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::clearConstants);
    connect(m_ui->pushButtonUp, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::moveConstantUp);
    connect(m_ui->pushButtonDown, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::moveConstantDown);
    connect(m_ui->pushButtonSort, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::sortConstants);
    connect(m_ui->pushButtonLoad, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::loadConstant);
    connect(m_ui->pushButtonSave, &QAbstractButton::clicked,
            this, &UserDefinedConstantsDialog::saveConstant);
}

UserDefinedConstantsDialog::~UserDefinedConstantsDialog() = default;

void UserDefinedConstantsDialog::insertConstant()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::addConstant()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::deleteConstant()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::clearConstants()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::moveConstantUp()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::moveConstantDown()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::sortConstants()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::loadConstant()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void UserDefinedConstantsDialog::saveConstant()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}
