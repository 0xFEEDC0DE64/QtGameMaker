#include "addeventdialog.h"
#include "ui_addeventdialog.h"

#include <QMessageBox>

AddEventDialog::AddEventDialog(QWidget *parent) :
    QDialog{parent},
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
