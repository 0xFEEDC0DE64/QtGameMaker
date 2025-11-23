#include "imageeditordialog.h"
#include "ui_imageeditordialog.h"

#include <QMessageBox>
#include <QDebug>
#include <QButtonGroup>

ImageEditorDialog::ImageEditorDialog(const QPixmap &pixmap, const QString &title, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ImageEditorDialog>()},
    m_pixmap{pixmap},
    m_title{title}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowMinimizeButtonHint);
    setWindowFlag(Qt::WindowMaximizeButtonHint);
    setWindowFlag(Qt::WindowCloseButtonHint);

    updateTitle();

    m_ui->scrollArea->setBackgroundRole(QPalette::Dark);

    {
        auto group = new QButtonGroup{this};
        group->setExclusive(true);
        group->addButton(m_ui->toolButtonDraw);
        group->addButton(m_ui->toolButtonSpray);
        group->addButton(m_ui->toolButtonErase);
        group->addButton(m_ui->toolButtonPick);
        group->addButton(m_ui->toolButtonLine);
        group->addButton(m_ui->toolButtonPolygon);
        group->addButton(m_ui->toolButtonRectangle);
        group->addButton(m_ui->toolButtonEllipse);
        group->addButton(m_ui->toolButtonRoundedRectangle);
        group->addButton(m_ui->toolButtonSelectRegion);
        group->addButton(m_ui->toolButtonSelectWand);
        group->addButton(m_ui->toolButtonSelectSpray);
        group->addButton(m_ui->toolButtonText);
        group->addButton(m_ui->toolButtonFill);
        group->addButton(m_ui->toolButtonReplace);
    }

    {
        auto group = new QButtonGroup{this};
        group->setExclusive(true);
        group->addButton(m_ui->pushButtonLine1);
        group->addButton(m_ui->pushButtonLine2);
        group->addButton(m_ui->pushButtonLine3);
        group->addButton(m_ui->pushButtonLine4);
    }

    {
        auto group = new QButtonGroup{this};
        group->setExclusive(true);
        group->addButton(m_ui->pushButtonBorder);
        group->addButton(m_ui->pushButtonBorderFill);
        group->addButton(m_ui->pushButtonFill);
    }

    {
        auto group = new QButtonGroup{this};
        group->setExclusive(true);
        group->addButton(m_ui->toolButtonAlignLeft);
        group->addButton(m_ui->toolButtonCenter);
        group->addButton(m_ui->toolButtonAlignRight);
    }

    m_ui->toolButtonDraw->click();

    m_ui->canvas->setPixmap(m_pixmap);
}

ImageEditorDialog::~ImageEditorDialog() = default;

void ImageEditorDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    // TODO

    QDialog::accept();
}

void ImageEditorDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Image has been modified."),
        tr("Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save
    );
    switch (result)
    {
    case QMessageBox::Save:
        accept();
        return;
    case QMessageBox::Discard:
        QDialog::reject();
        return;
    case QMessageBox::Cancel:
        return;
    default:
        qWarning() << "unexpected dialog result" << result;
    }
}

void ImageEditorDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void ImageEditorDialog::updateTitle()
{
    setWindowTitle(tr("%0%1")
                       .arg(m_title)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}
