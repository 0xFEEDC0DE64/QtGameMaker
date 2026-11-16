#include "objectselectorwidget.h"
#include "ui_objectselectorwidget.h"

#include <QMenu>

#include "models/projecttreemodel.h"
#include "projectcontainer.h"
#include "widgets/resourcemenuadapter.h"

ObjectSelectorWidget::ObjectSelectorWidget(QWidget *parent) :
    QWidget{parent},
    m_ui{std::make_unique<Ui::ObjectSelectorWidget>()},
    m_menuObjects{new QMenu{this}}
{
    m_ui->setupUi(this);

    m_ui->lineEdit->setMenu(m_menuObjects);
    m_ui->toolButton->setMenu(m_menuObjects);

}

ObjectSelectorWidget::~ObjectSelectorWidget() = default;

void ObjectSelectorWidget::setProjectModel(ProjectTreeModel *projectModel)
{
    m_menuAdapter.reset();
    if (m_projectModel)
        disconnect(m_projectModel, nullptr, this, nullptr);
    m_projectModel = projectModel;
    if (!m_projectModel)
    {
        clearObject();
        return;
    }
    connect(m_projectModel, &ProjectTreeModel::objectAboutToBeRemoved, this,
            [this](const Object &object) {
        if (m_objectName == object.name)
            clearObject();
    });
    connect(m_projectModel, &ProjectTreeModel::objectNameChanged, this,
            [this](const Object &object, const QString &oldName) {
        if (m_objectName == oldName)
            setObject(object.name);
    });
    connect(m_projectModel, &QAbstractItemModel::modelAboutToBeReset,
            this, &ObjectSelectorWidget::clearObject);
    m_menuAdapter = std::make_unique<ResourceMenuAdapter>(
        *m_projectModel, m_projectModel->rootFor<Object>(), *m_menuObjects,
        [this](const QModelIndex &index) {
            if (const Object *object = m_projectModel->get<Object>(index)) setObject(object->name);
        },
        [this](QMenu &menu) {
            if (m_showClearObjectAction)
                menu.addAction(m_emptySelectionText, this, &ObjectSelectorWidget::clearObject);
        });
}

void ObjectSelectorWidget::setObject(const QString &objectName)
{
    if (objectName.isEmpty())
    {
        clearObject();
        return;
    }
    if (m_objectName == objectName)
        return;

    m_objectName = objectName;
    m_ui->lineEdit->setText(objectName);
    emit changed();
}

void ObjectSelectorWidget::clearObject()
{
    if (m_objectName.isEmpty())
        return;

    m_objectName.clear();
    m_ui->lineEdit->setText(m_emptySelectionText);
    emit changed();
}

void ObjectSelectorWidget::setEmptySelectionText(const QString &emptySelectionText)
{
    m_emptySelectionText = emptySelectionText;
    if (m_objectName.isEmpty())
        m_ui->lineEdit->setText(m_emptySelectionText);
}
