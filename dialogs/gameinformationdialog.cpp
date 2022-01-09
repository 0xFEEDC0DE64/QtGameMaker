#include "gameinformationdialog.h"
#include "ui_gameinformationdialog.h"

GameInformationDialog::GameInformationDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::GameInformationDialog>()}
{
    m_ui->setupUi(this);
    setWindowFlags(windowFlags()
                   & ~Qt::Dialog
                   | Qt::Window
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint
                   | Qt::WindowCloseButtonHint);
}

GameInformationDialog::~GameInformationDialog() = default;
