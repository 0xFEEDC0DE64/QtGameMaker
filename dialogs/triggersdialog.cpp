#include "triggersdialog.h"
#include "ui_triggersdialog.h"

#include <QDebug>
#include <QMessageBox>

#include "projectcontainer.h"
#include "triggersmodel.h"
#include "triggerconditiondialog.h"

TriggersDialog::TriggersDialog(ProjectContainer &project, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::TriggersDialog>()},
    m_project{project},
    m_model{std::make_unique<TriggersModel>(project, this)}
{
    m_ui->setupUi(this);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    m_ui->listView->setModel(m_model.get());

    connect(m_ui->pushButtonAdd, &QAbstractButton::clicked,
            this, &TriggersDialog::add);
    connect(m_ui->pushButtonDelete, &QAbstractButton::clicked,
            this, &TriggersDialog::delete_);
    connect(m_ui->pushButtonLoad, &QAbstractButton::clicked,
            this, &TriggersDialog::load);
    connect(m_ui->pushButtonSave, &QAbstractButton::clicked,
            this, &TriggersDialog::save);
    connect(m_ui->pushButtonClear, &QAbstractButton::clicked,
            this, &TriggersDialog::clear);
    connect(m_ui->pushButtonUseCodeEditor, &QAbstractButton::clicked,
            this, &TriggersDialog::openCodeEditor);
}

void TriggersDialog::add()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TriggersDialog::delete_()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TriggersDialog::load()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TriggersDialog::save()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void TriggersDialog::clear()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

TriggersDialog::~TriggersDialog() = default;

void TriggersDialog::openCodeEditor()
{
    TriggerConditionDialog dialog{this};
    if (dialog.exec() == QDialog::Accepted)
        m_ui->plainTextEdit->setPlainText(dialog.script());
}
