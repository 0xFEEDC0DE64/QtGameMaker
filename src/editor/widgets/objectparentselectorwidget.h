#pragma once

#include "objectselectorwidget.h"

class ObjectParentSelectorWidget : public ObjectSelectorWidget
{
    Q_OBJECT

public:
    using ObjectSelectorWidget::ObjectSelectorWidget;

    QString forbiddenObjectName() const { return m_forbiddenObjectName; };
    void setForbiddenObjectName(const QString &forbiddenObjectName) { m_forbiddenObjectName = forbiddenObjectName; }

    void setObject(const QString &objectName) override;

private:
    QString m_forbiddenObjectName;
};
