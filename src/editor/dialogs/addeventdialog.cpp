#include "addeventdialog.h"
#include "ui_addeventdialog.h"

#include <QMessageBox>
#include <QMenu>

#include "models/projecttreemodel.h"
#include "widgets/resourcemenuadapter.h"
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
        m_collisionMenuAdapter = std::make_unique<ResourceMenuAdapter>(
            m_projectModel, m_projectModel.rootFor<Object>(), *menu,
            [this](const QModelIndex &index) {
                if (const Object *object = m_projectModel.get<Object>(index)) {
                    m_eventType = object->name;
                    accept();
                }
            });
        m_ui->pushButtonCollision->setMenu(menu);
    }

    {
        auto menu = new QMenu;
        menu->addAction(tr("Left button"),  this, [this](){ m_eventType = Object::EventType::MouseLeftButton; accept(); });
        menu->addAction(tr("Right button"),  this, [this](){ m_eventType = Object::EventType::MouseRightButton; accept(); });
        menu->addAction(tr("Middle button"),  this, [this](){ m_eventType = Object::EventType::MouseMiddleButton; accept(); });
        menu->addSeparator();
        menu->addAction(tr("No button"),  this, [this](){ m_eventType = Object::EventType::MouseNoButton; accept(); });
        menu->addSeparator();
        menu->addAction(tr("Left pressed"),  this, [this](){ m_eventType = Object::EventType::MouseLeftPressed; accept(); });
        menu->addAction(tr("Right pressed"),  this, [this](){ m_eventType = Object::EventType::MouseRightPressed; accept(); });
        menu->addAction(tr("Middle pressed"),  this, [this](){ m_eventType = Object::EventType::MouseMiddlePressed; accept(); });
        menu->addSeparator();
        menu->addAction(tr("Left released"),  this, [this](){ m_eventType = Object::EventType::MouseLeftReleased; accept(); });
        menu->addAction(tr("Right released"),  this, [this](){ m_eventType = Object::EventType::MouseRightReleased; accept(); });
        menu->addAction(tr("Middle released"),  this, [this](){ m_eventType = Object::EventType::MouseMiddleReleased; accept(); });
        menu->addSeparator();
        menu->addAction(tr("Mouse enter"),  this, [this](){ m_eventType = Object::EventType::MouseEnter; accept(); });
        menu->addAction(tr("Mouse leave"),  this, [this](){ m_eventType = Object::EventType::MouseLeave; accept(); });
        menu->addSeparator();
        menu->addAction(tr("Mouse wheel up"),  this, [this](){ m_eventType = Object::EventType::MouseWheelUp; accept(); });
        menu->addAction(tr("Mouse wheel down"),  this, [this](){ m_eventType = Object::EventType::MouseWheelDown; accept(); });
        menu->addSeparator();
        {
            auto subMenu = menu->addMenu(tr("Global mouse"));
            subMenu->addAction(tr("Global Left button"),  this, [this](){ m_eventType = Object::EventType::MouseGlobalLeftButton; accept(); });
            subMenu->addAction(tr("Global Right button"),  this, [this](){ m_eventType = Object::EventType::MouseGlobalRightButton; accept(); });
            subMenu->addAction(tr("Global Middle button"),  this, [this](){ m_eventType = Object::EventType::MouseGlobalMiddleButton; accept(); });
            subMenu->addSeparator();
            subMenu->addAction(tr("Global Left pressed"),  this, [this](){ m_eventType = Object::EventType::MouseGlobalLeftPressed; accept(); });
            subMenu->addAction(tr("Global Right pressed"),  this, [this](){ m_eventType = Object::EventType::MouseGlobalRightPressed; accept(); });
            subMenu->addAction(tr("Global Middle pressed"),  this, [this](){ m_eventType = Object::EventType::MouseGlobalMiddlePressed; accept(); });
            subMenu->addSeparator();
            subMenu->addAction(tr("Global Left released"),  this, [this](){ m_eventType = Object::EventType::MouseGlobalLeftReleased; accept(); });
            subMenu->addAction(tr("Global Right released"),  this, [this](){ m_eventType = Object::EventType::MouseGlobalRightReleased; accept(); });
            subMenu->addAction(tr("Global Middle released"),  this, [this](){ m_eventType = Object::EventType::MouseGlobalMiddleReleased; accept(); });
        }
        menu->addSeparator();
        {
            auto subMenu = menu->addMenu(tr("Joystick 1"));
            subMenu->addAction(tr("Left"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Left; accept(); });
            subMenu->addAction(tr("Right"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Right; accept(); });
            subMenu->addAction(tr("Up"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Up; accept(); });
            subMenu->addAction(tr("Down"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Down; accept(); });
            subMenu->addSeparator();
            subMenu->addAction(tr("Button 1"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Button1; accept(); });
            subMenu->addAction(tr("Button 2"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Button2; accept(); });
            subMenu->addAction(tr("Button 3"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Button3; accept(); });
            subMenu->addAction(tr("Button 4"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Button4; accept(); });
            subMenu->addAction(tr("Button 5"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Button5; accept(); });
            subMenu->addAction(tr("Button 6"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Button6; accept(); });
            subMenu->addAction(tr("Button 7"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Button7; accept(); });
            subMenu->addAction(tr("Button 8"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick1Button8; accept(); });
        }
        {
            auto subMenu = menu->addMenu(tr("Joystick 2"));
            subMenu->addAction(tr("Left"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Left; accept(); });
            subMenu->addAction(tr("Right"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Right; accept(); });
            subMenu->addAction(tr("Up"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Up; accept(); });
            subMenu->addAction(tr("Down"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Down; accept(); });
            subMenu->addSeparator();
            subMenu->addAction(tr("Button 1"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Button1; accept(); });
            subMenu->addAction(tr("Button 2"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Button2; accept(); });
            subMenu->addAction(tr("Button 3"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Button3; accept(); });
            subMenu->addAction(tr("Button 4"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Button4; accept(); });
            subMenu->addAction(tr("Button 5"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Button5; accept(); });
            subMenu->addAction(tr("Button 6"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Button6; accept(); });
            subMenu->addAction(tr("Button 7"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Button7; accept(); });
            subMenu->addAction(tr("Button 8"),  this, [this](){ m_eventType = Object::EventType::MouseJoystick2Button8; accept(); });
        }
        m_ui->pushButtonMouse->setMenu(menu);
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
