#include "imageeditordialog.h"
#include "ui_imageeditordialog.h"

ImageEditorDialog::ImageEditorDialog(QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ImageEditorDialog>()}
{
    m_ui->setupUi(this);

    setWindowFlags(windowFlags()
                    & ~Qt::Dialog
                   | Qt::Window
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint
                   | Qt::WindowCloseButtonHint);
}

ImageEditorDialog::~ImageEditorDialog() = default;
