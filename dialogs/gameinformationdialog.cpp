#include "gameinformationdialog.h"
#include "ui_gameinformationdialog.h"

GameInformationDialog::GameInformationDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::GameInformationDialog>()}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowMinimizeButtonHint);
    setWindowFlag(Qt::WindowMaximizeButtonHint);
    setWindowFlag(Qt::WindowCloseButtonHint);
}

GameInformationDialog::~GameInformationDialog() = default;
