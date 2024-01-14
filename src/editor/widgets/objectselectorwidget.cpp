#include "objectselectorwidget.h"
#include "ui_objectselectorwidget.h"

#include <QMenu>
#include <QDebug>

#include "models/projecttreemodel.h"
#include "projectcontainer.h"

ObjectSelectorWidget::ObjectSelectorWidget(QWidget *parent) :
    QWidget{parent},
    m_ui{std::make_unique<Ui::ObjectSelectorWidget>()},
    m_menuObjects{new QMenu{this}}
{
    m_ui->setupUi(this);

    m_ui->lineEdit->setMenu(m_menuObjects);
    m_ui->toolButton->setMenu(m_menuObjects);

    connect(m_menuObjects, &QMenu::aboutToShow,
            this, &ObjectSelectorWidget::menuAboutToShow);
}

ObjectSelectorWidget::~ObjectSelectorWidget() = default;

void ObjectSelectorWidget::setObject(const QString &objectName)
{
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

void ObjectSelectorWidget::menuAboutToShow()
{
    if (!m_projectModel)
    {
        qCritical() << "no project model!";
        return;
    }

    m_menuObjects->clear();

    if (m_showClearObjectAction)
        m_menuObjects->addAction(m_emptySelectionText, this, &ObjectSelectorWidget::clearObject);

    for (const Object &object : m_projectModel->project()->objects)
    {
        QIcon icon;
        if (!object.spriteName.isEmpty())
        {
            const auto &sprites = m_projectModel->project()->sprites;
            const auto iter = std::find_if(std::cbegin(sprites), std::cend(sprites),
                                           [&](const Sprite &sprite){ return sprite.name == object.spriteName; });
            if (iter != std::cend(sprites))
            {
                if (!iter->pixmaps.empty())
                    icon = iter->pixmaps.front();
            }
        }
        m_menuObjects->addAction(icon, object.name, this,
                                 [&object,this](){ setObject(object.name); });
    }
}
