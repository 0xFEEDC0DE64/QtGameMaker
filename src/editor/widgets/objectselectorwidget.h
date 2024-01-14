#pragma once

#include <QWidget>

#include <memory>

namespace Ui { class ObjectSelectorWidget; }
class ProjectTreeModel;

class ObjectSelectorWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString emptySelectionText READ emptySelectionText WRITE setEmptySelectionText DESIGNABLE true FINAL)
    Q_PROPERTY(bool showClearObjectAction READ showClearObjectAction WRITE setShowClearObjectAction DESIGNABLE true FINAL)

public:
    explicit ObjectSelectorWidget(QWidget *parent = nullptr);
    ~ObjectSelectorWidget() override;

    ProjectTreeModel *projectModel() { return m_projectModel; }
    const ProjectTreeModel *projectModel() const { return m_projectModel; }
    void setProjectModel(ProjectTreeModel *projectModel) { m_projectModel = projectModel; }

    QString objectName() const { return m_objectName; }
    virtual void setObject(const QString &objectName);
    void clearObject();

    QString emptySelectionText() const { return m_emptySelectionText; }
    void setEmptySelectionText(const QString &emptySelectionText);

    bool showClearObjectAction() const { return m_showClearObjectAction; }
    void setShowClearObjectAction(bool showClearObjectAction) { m_showClearObjectAction = showClearObjectAction; }

signals:
    void changed();

private slots:
    void menuAboutToShow();

private:
    const std::unique_ptr<Ui::ObjectSelectorWidget> m_ui;

    ProjectTreeModel *m_projectModel{};

    QString m_objectName;

    QString m_emptySelectionText;
    bool m_showClearObjectAction;

    QMenu * const m_menuObjects;
};
