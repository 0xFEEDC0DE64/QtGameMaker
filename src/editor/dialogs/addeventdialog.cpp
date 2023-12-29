#include "addeventdialog.h"
#include "ui_addeventdialog.h"

#include <QMessageBox>
#include <QMenu>

#include "models/projecttreemodel.h"

AddEventDialog::AddEventDialog(ProjectTreeModel &projectModel, QWidget *parent) :
    QDialog{parent},
    m_projectModel{projectModel},
    m_ui{std::make_unique<Ui::AddEventDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);

    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Ok))
        button->setIcon(QIcon{":/qtgameengine/icons/ok.png"});
    if (auto button = m_ui->buttonBox->button(QDialogButtonBox::Cancel))
        button->setIcon(QIcon{":/qtgameengine/icons/delete.png"});

    connect(m_ui->pushButtonCreate, &QAbstractButton::clicked,
            this, [this](){ m_eventType = Object::EventType::Create; accept(); });
    connect(m_ui->pushButtonDestroy, &QAbstractButton::clicked,
            this, [this](){ m_eventType = Object::EventType::Destroy; accept(); });
    connect(m_ui->pushButtonDraw, &QAbstractButton::clicked,
            this, [this](){ m_eventType = Object::EventType::Draw; accept(); });
    connect(m_ui->pushButtonStep, &QAbstractButton::clicked,
            this, [this](){ m_eventType = Object::EventType::Step; accept(); });

    auto menu = new QMenu;
    connect(menu, &QMenu::aboutToShow, menu, [this,menu](){
        for (const Object &object : m_projectModel.project()->objects)
        {
            QIcon icon;
            if (!object.spriteName.isEmpty())
            {
                const auto &sprites = m_projectModel.project()->sprites;
                const auto iter = std::find_if(std::cbegin(sprites), std::cend(sprites),
                                               [&](const Sprite &sprite){ return sprite.name == object.spriteName; });
                if (iter != std::cend(sprites))
                {
                    if (!iter->pixmaps.empty())
                        icon = iter->pixmaps.front();
                }
            }
            menu->addAction(icon, object.name, this, [this,&object](){
                m_eventType = object.name; accept();
            });
        }
    }, Qt::SingleShotConnection);
    m_ui->pushButtonCollision->setMenu(menu);
}

AddEventDialog::~AddEventDialog() = default;

void AddEventDialog::accept()
{
    if (!m_eventType)
    {
        QMessageBox::warning(this, tr("No Event selected!"), tr("No Event selected!"));
        return;
    }

    QDialog::accept();
}

void AddEventDialog::reject()
{
    m_eventType = std::nullopt;

    QDialog::reject();
}
