#include "executecodedialog.h"

#include <QRadioButton>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>

#include "projectcontainer.h"
#include "widgets/objectselectorwidget.h"

ExecuteCodeDialog::ExecuteCodeDialog(ExecuteCodeAction &action, ProjectTreeModel *projectModel, QWidget *parent) :
    CodeEditorDialog{tr("Execute Code"), parent},
    m_action{action},
    m_radioButtonSelf{new QRadioButton{tr("Self"), this}},
    m_radioButtonOther{new QRadioButton{tr("Other"), this}},
    m_radioButtonObject{new QRadioButton{tr("Object"), this}},
    m_objectSelector{new ObjectSelectorWidget{this}}
{
#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);

    m_objectSelector->setProjectModel(projectModel);

    m_objectSelector->setEmptySelectionText(tr("self"));
    m_objectSelector->setShowClearObjectAction(false);

    addToolbarWidget(new QLabel{tr("Applies to:"), this});
    addToolbarWidget(m_radioButtonSelf);
    addToolbarWidget(m_radioButtonOther);
    addToolbarWidget(m_radioButtonObject);
    auto action_ = addToolbarWidget(m_objectSelector);

    connect(m_radioButtonObject, &QRadioButton::toggled,
            m_objectSelector, &QWidget::setVisible);
    connect(m_radioButtonObject, &QRadioButton::toggled,
            action_, &QAction::setVisible);

    m_radioButtonSelf->setChecked(std::holds_alternative<AppliesToSelf>(m_action.appliesTo));
    m_radioButtonOther->setChecked(std::holds_alternative<AppliesToOther>(m_action.appliesTo));
    {
        auto ptr = std::get_if<AppliesToObject>(&m_action.appliesTo);
        m_radioButtonObject->setChecked(ptr != nullptr);
        m_objectSelector->setVisible(ptr != nullptr);
        action_->setVisible(ptr != nullptr);
        if (ptr)
            m_objectSelector->setObject(ptr->objectName);
        else
            m_objectSelector->clearObject();
    }

    setScript(m_action.script);

    connect(m_radioButtonSelf, &QRadioButton::toggled,
            this, &ExecuteCodeDialog::changed);
    connect(m_radioButtonOther, &QRadioButton::toggled,
            this, &ExecuteCodeDialog::changed);
    connect(m_radioButtonObject, &QRadioButton::toggled,
            this, &ExecuteCodeDialog::changed);
}

void ExecuteCodeDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    if (m_radioButtonSelf->isChecked())
        m_action.appliesTo = AppliesToSelf{};
    else if (m_radioButtonOther->isChecked())
        m_action.appliesTo = AppliesToOther{};
    else if (m_radioButtonObject->isChecked())
        m_action.appliesTo = AppliesToObject{ .objectName = m_objectSelector->objectName() };
    else
    {
        QMessageBox::warning(this, tr("No Applies To selected!"), tr("No Applies To selected!"));
        return;
    }

    m_action.script = script();

    CodeEditorDialog::accept();
}
