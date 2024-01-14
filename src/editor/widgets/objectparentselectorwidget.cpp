#include "objectparentselectorwidget.h"

#include <QMessageBox>

void ObjectParentSelectorWidget::setObject(const QString &objectName)
{
    if (!m_forbiddenObjectName.isEmpty() && m_forbiddenObjectName == objectName)
    {
        QMessageBox::warning(this, tr("This will create a loop in parents."), tr("This will create a loop in parents."));
        return;
    }

    ObjectSelectorWidget::setObject(objectName);
}
