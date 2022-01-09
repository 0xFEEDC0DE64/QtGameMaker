#include "pathpropertiesdialog.h"
#include "ui_pathpropertiesdialog.h"

#include <QDebug>
#include <QMessageBox>

#include "projectcontainer.h"
#include "projecttreemodel.h"

PathPropertiesDialog::PathPropertiesDialog(Path &path, ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::PathPropertiesDialog>()},
    m_path{path},
    m_projectModel{projectModel},
    m_labelX{new QLabel{tr("x: %0").arg(0)}},
    m_labelY{new QLabel{tr("y: %0").arg(0)}},
    m_labelArea{new QLabel{tr("Area: (%0,%1)->(%2,%3)").arg(0).arg(0).arg(0).arg(0)}}
{
    m_ui->setupUi(this);

    updateTitle();

    m_labelX->setFrameStyle(QFrame::Sunken);
    m_ui->statusbar->addWidget(m_labelX, 1);
    m_labelY->setFrameStyle(QFrame::Sunken);
    m_ui->statusbar->addWidget(m_labelY, 1);
    m_labelArea->setFrameStyle(QFrame::Sunken);
    m_ui->statusbar->addWidget(m_labelArea, 4);

    {
        auto frame = new QFrame{this};
        frame->setFrameStyle(QFrame::Sunken);
        m_ui->statusbar->addPermanentWidget(frame, 2);
    }

    m_ui->lineEditName->setText(m_path.name);

    connect(&m_projectModel, &ProjectTreeModel::pathNameChanged,
            this, &PathPropertiesDialog::pathNameChanged);

    connect(m_ui->pushButtonAdd, &QAbstractButton::pressed,
            this, &PathPropertiesDialog::add);
    connect(m_ui->pushButtonInsert, &QAbstractButton::pressed,
            this, &PathPropertiesDialog::insert);
    connect(m_ui->pushButtonDelete, &QAbstractButton::pressed,
            this, &PathPropertiesDialog::delete_);

    connect(m_ui->lineEditName, &QLineEdit::textChanged,
            this, &PathPropertiesDialog::changed);
}

PathPropertiesDialog::~PathPropertiesDialog() = default;

void PathPropertiesDialog::accept()
{
    if (m_path.name != m_ui->lineEditName->text())
    {
        if (!m_projectModel.renamePath(m_path, m_ui->lineEditName->text()))
        {
            QMessageBox::critical(this, tr("Renaming Path failed!"), tr("Renaming Path failed!"));
            return;
        }
    }

    // TODO

    QDialog::accept();
}

void PathPropertiesDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Path has been modified."),
        tr("Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save
    );
    switch (result)
    {
    case QMessageBox::Save:
        accept();
        return;
    case QMessageBox::Discard:
        QDialog::reject();
        return;
    case QMessageBox::Cancel:
        return;
    default:
        qWarning() << "unexpected dialog result" << result;
    }
}

void PathPropertiesDialog::add()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::insert()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::delete_()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void PathPropertiesDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void PathPropertiesDialog::pathNameChanged(const Path &path)
{
    if (&path != &m_path)
        return;

    {
        QSignalBlocker blocker{m_ui->lineEditName};
        m_ui->lineEditName->setText(path.name);
    }

    updateTitle();
}

void PathPropertiesDialog::updateTitle()
{
    setWindowTitle(tr("Path Properties: %0%1")
                       .arg(m_path.name)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}
