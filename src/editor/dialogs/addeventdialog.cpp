#include "addeventdialog.h"
#include "ui_addeventdialog.h"

#include <QMessageBox>
#include <QMenu>

#include "models/projecttreemodel.h"
#include "mainwindow.h"
#include "editorguiutils.h"
#include "editorsettings.h"

AddEventDialog::AddEventDialog(ProjectTreeModel &projectModel, MainWindow &mainWindow, QWidget *parent) :
    QDialog{parent},
    m_projectModel{projectModel},
    m_mainWindow{mainWindow},
    m_ui{std::make_unique<Ui::AddEventDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowCloseButtonHint);

    improveButtonBox(m_ui->buttonBox);

    connect(m_ui->pushButtonCreate, &QAbstractButton::clicked,
            this, [this](){ m_eventType = Object::EventType::Create; accept(); });
    connect(m_ui->pushButtonDestroy, &QAbstractButton::clicked,
            this, [this](){ m_eventType = Object::EventType::Destroy; accept(); });
    connect(m_ui->pushButtonDraw, &QAbstractButton::clicked,
            this, [this](){ m_eventType = Object::EventType::Draw; accept(); });

    {
        auto menu = new QMenu;
        menu->addAction(tr("Alarm 0"),  this, [this](){ m_eventType = Object::EventType::Alarm0; accept(); });
        menu->addAction(tr("Alarm 1"),  this, [this](){ m_eventType = Object::EventType::Alarm1; accept(); });
        menu->addAction(tr("Alarm 2"),  this, [this](){ m_eventType = Object::EventType::Alarm2; accept(); });
        menu->addAction(tr("Alarm 3"),  this, [this](){ m_eventType = Object::EventType::Alarm3; accept(); });
        menu->addAction(tr("Alarm 4"),  this, [this](){ m_eventType = Object::EventType::Alarm4; accept(); });
        menu->addAction(tr("Alarm 5"),  this, [this](){ m_eventType = Object::EventType::Alarm5; accept(); });
        menu->addAction(tr("Alarm 6"),  this, [this](){ m_eventType = Object::EventType::Alarm6; accept(); });
        menu->addAction(tr("Alarm 7"),  this, [this](){ m_eventType = Object::EventType::Alarm7; accept(); });
        menu->addAction(tr("Alarm 8"),  this, [this](){ m_eventType = Object::EventType::Alarm8; accept(); });
        menu->addAction(tr("Alarm 9"),  this, [this](){ m_eventType = Object::EventType::Alarm9; accept(); });
        menu->addAction(tr("Alarm 10"), this, [this](){ m_eventType = Object::EventType::Alarm10; accept(); });
        menu->addAction(tr("Alarm 11"), this, [this](){ m_eventType = Object::EventType::Alarm11; accept(); });
        m_ui->pushButtonAlarm->setMenu(menu);
    }

    {
        auto menu = new QMenu;
        menu->addAction(tr("Step"),       this, [this](){ m_eventType = Object::EventType::Step; accept(); });
        menu->addAction(tr("Begin Step"), this, [this](){ m_eventType = Object::EventType::BeginStep; accept(); });
        menu->addAction(tr("End Step"),   this, [this](){ m_eventType = Object::EventType::EndStep; accept(); });
        m_ui->pushButtonStep->setMenu(menu);
    }

    {
        auto menu = new QMenu;
        connect(menu, &QMenu::aboutToShow, menu, [this,menu](){
            menu->clear();
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
        });
        m_ui->pushButtonCollision->setMenu(menu);
    }

    {
        auto menu = new QMenu;
        menu->addAction(tr("Add/Modify Triggers..."), this, [this](){ m_mainWindow.showDefineTriggers(); reject(); });
        m_ui->pushButtonTrigger->setMenu(menu);
    }

    connect(&mainWindow.settings(), &EditorSettings::advancedModeChanged,
            this, &AddEventDialog::advancedModeChanged);
    advancedModeChanged(mainWindow.settings().advancedMode());
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

void AddEventDialog::advancedModeChanged(bool advancedMode)
{
    for (QWidget *widgets[] {
             m_ui->pushButtonTrigger
         }; QWidget *widget : widgets)
        widget->setVisible(advancedMode);
}
