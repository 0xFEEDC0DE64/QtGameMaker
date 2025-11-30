#include "imageeditordialog.h"
#include "ui_imageeditordialog.h"

#include <QMessageBox>
#include <QDebug>
#include <QButtonGroup>
#include <QFontDialog>

#include "dialogs/transparentbackgroundsettingsdialog.h"
#include "imagehelpers.h"
#include "editorsettings.h"

ImageEditorDialog::ImageEditorDialog(const QPixmap &pixmap, const QString &title,
                                     EditorSettings &settings, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ImageEditorDialog>()},
    m_title{title},
    m_settings{settings},
    m_posLabel{*new QLabel(tr("(%0,%1)").arg(0).arg(0))},
    m_zoomLabel{*new QLabel},
    m_sizeLabel{*new QLabel},
    m_memoryLabel{*new QLabel}
{
    m_ui->setupUi(this);

#ifdef Q_OS_LINUX
    setWindowFlags((windowFlags() & ~Qt::Dialog) | Qt::Window);
#endif
    setWindowFlag(Qt::WindowMinimizeButtonHint);
    setWindowFlag(Qt::WindowMaximizeButtonHint);
    setWindowFlag(Qt::WindowCloseButtonHint);

    updateTitle();

    m_ui->actionNew->setShortcut(QKeySequence::New);
    m_ui->actionOpen->setShortcut(QKeySequence::Open);
    m_ui->actionSaveAsPngFile->setShortcut(QKeySequence::Save);
    m_ui->actionUndo->setShortcut(QKeySequence::Undo);
    m_ui->actionRedo->setShortcut(QKeySequence::Redo);
    m_ui->actionDelete->setShortcut(QKeySequence::Delete);
    m_ui->actionCut->setShortcut(QKeySequence::Cut);
    m_ui->actionCopy->setShortcut(QKeySequence::Copy);
    m_ui->actionPaste->setShortcut(QKeySequence::Paste);
    m_ui->actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    m_ui->actionZoomOut->setShortcut(QKeySequence::ZoomOut);

    m_ui->scrollArea->setBackgroundRole(QPalette::Dark);

    connect(m_ui->actionNew, &QAction::triggered,
            this, &ImageEditorDialog::newFile);
    connect(m_ui->actionOpen, &QAction::triggered,
            this, &ImageEditorDialog::openFile);
    connect(m_ui->actionSaveAsPngFile, &QAction::triggered,
            this, &ImageEditorDialog::saveAsPng);
    connect(m_ui->actionPreviousImage, &QAction::triggered,
            this, &ImageEditorDialog::previousSprite);
    connect(m_ui->actionNextImage, &QAction::triggered,
            this, &ImageEditorDialog::nextSprite);
    connect(m_ui->actionUndo, &QAction::triggered,
            this, &ImageEditorDialog::undo);
    connect(m_ui->actionRedo, &QAction::triggered,
            this, &ImageEditorDialog::redo);
    connect(m_ui->actionEraseToLeftColor, &QAction::triggered,
            this, &ImageEditorDialog::eraseToLeftColor);
    connect(m_ui->actionDelete, &QAction::triggered,
            this, &ImageEditorDialog::delete_);
    connect(m_ui->actionCut, &QAction::triggered,
            this, &ImageEditorDialog::cut);
    connect(m_ui->actionCopy, &QAction::triggered,
            this, &ImageEditorDialog::copy);
    connect(m_ui->actionPaste, &QAction::triggered,
            this, &ImageEditorDialog::paste);
    connect(m_ui->actionPasteFromFile, &QAction::triggered,
            this, &ImageEditorDialog::pasteFromFile);
    connect(m_ui->actionSelectAll, &QAction::triggered,
            this, &ImageEditorDialog::selectAll);
    connect(m_ui->actionZoomIn, &QAction::triggered,
            this, &ImageEditorDialog::zoomIn);
    connect(m_ui->actionNoZoom, &QAction::triggered,
            this, &ImageEditorDialog::resetZoom);
    connect(m_ui->actionZoomOut, &QAction::triggered,
            this, &ImageEditorDialog::zoomOut);
    connect(m_ui->actionSetTransparencyBackground, &QAction::triggered,
            this, &ImageEditorDialog::transparentBackgroundSettings);

    connect(m_ui->pushButtonSelectFont, &QAbstractButton::pressed,
            this, &ImageEditorDialog::selectFont);

    connect(m_ui->frameColorCoarse, &ColorWidget::colorLeftClicked,
            m_ui->canvas, &DrawingCanvasWidget::setLeftButtonColor);
    connect(m_ui->frameColorCoarse, &ColorWidget::colorRightClicked,
            m_ui->canvas, &DrawingCanvasWidget::setRightButtonColor);

    connect(m_ui->frameColorFine, &ColorWidget::colorLeftClicked,
            m_ui->canvas, &DrawingCanvasWidget::setLeftButtonColor);
    connect(m_ui->frameColorFine, &ColorWidget::colorRightClicked,
            m_ui->canvas, &DrawingCanvasWidget::setRightButtonColor);

    connect(m_ui->canvas, &DrawingCanvasWidget::changed,
            this, &ImageEditorDialog::changed);

    connect(m_ui->canvas, &DrawingCanvasWidget::leftButtonColorChanged,
            m_ui->pushButtonLeftButtonColor, &ColorButton::setColor);
    connect(m_ui->pushButtonLeftButtonColor, &ColorButton::colorChanged,
            m_ui->canvas, &DrawingCanvasWidget::setLeftButtonColor);

    connect(m_ui->canvas, &DrawingCanvasWidget::rightButtonColorChanged,
            m_ui->pushButtonRightButtonColor, &ColorButton::setColor);
    connect(m_ui->pushButtonRightButtonColor, &ColorButton::colorChanged,
            m_ui->canvas, &DrawingCanvasWidget::rightButtonColor);

    {
        auto group = new QButtonGroup{this};
        group->setExclusive(true);
        group->addButton(m_ui->toolButtonDraw,             std::to_underlying(DrawingCanvasWidget::Mode::Draw));
        group->addButton(m_ui->toolButtonSpray,            std::to_underlying(DrawingCanvasWidget::Mode::Spray));
        group->addButton(m_ui->toolButtonErase,            std::to_underlying(DrawingCanvasWidget::Mode::Erase));
        group->addButton(m_ui->toolButtonPick,             std::to_underlying(DrawingCanvasWidget::Mode::Pick));
        group->addButton(m_ui->toolButtonLine,             std::to_underlying(DrawingCanvasWidget::Mode::Line));
        group->addButton(m_ui->toolButtonPolygon,          std::to_underlying(DrawingCanvasWidget::Mode::Polygon));
        group->addButton(m_ui->toolButtonRectangle,        std::to_underlying(DrawingCanvasWidget::Mode::Rectangle));
        group->addButton(m_ui->toolButtonEllipse,          std::to_underlying(DrawingCanvasWidget::Mode::Ellipse));
        group->addButton(m_ui->toolButtonRoundedRectangle, std::to_underlying(DrawingCanvasWidget::Mode::RoundedRectangle));
        group->addButton(m_ui->toolButtonSelectRegion,     std::to_underlying(DrawingCanvasWidget::Mode::SelectRegion));
        group->addButton(m_ui->toolButtonSelectWand,       std::to_underlying(DrawingCanvasWidget::Mode::SelectWand));
        group->addButton(m_ui->toolButtonSelectSpray,      std::to_underlying(DrawingCanvasWidget::Mode::SelectSpray));
        group->addButton(m_ui->toolButtonText,             std::to_underlying(DrawingCanvasWidget::Mode::Text));
        group->addButton(m_ui->toolButtonFill,             std::to_underlying(DrawingCanvasWidget::Mode::Fill));
        group->addButton(m_ui->toolButtonReplace,          std::to_underlying(DrawingCanvasWidget::Mode::Replace));
        if (auto button = group->button(std::to_underlying(m_ui->canvas->mode())))
            button->click();
        connect(group, &QButtonGroup::idClicked,
                m_ui->canvas, [canvas=m_ui->canvas](int id){ canvas->setMode(DrawingCanvasWidget::Mode(id)); });
    }

    {
        auto group = new QButtonGroup{this};
        group->setExclusive(true);
        group->addButton(m_ui->toolButtonSize1);
        group->addButton(m_ui->toolButtonSize2);
        group->addButton(m_ui->toolButtonSize3);
        group->addButton(m_ui->toolButtonSize4);
        group->addButton(m_ui->toolButtonSize5);
        group->addButton(m_ui->toolButtonSize6);
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

    m_ui->pushButtonLeftButtonColor->setColor(m_ui->canvas->leftButtonColor());
    m_ui->pushButtonRightButtonColor->setColor(m_ui->canvas->rightButtonColor());

    m_ui->canvas->setPixmap(pixmap);
    m_ui->canvas->setTransparentBackgroundPattern(settings.transparentBackgroundPattern());

    connect(m_ui->canvas, &DrawingCanvasWidget::cursorMoved,
            this, &ImageEditorDialog::cursorMoved);

    connect(m_ui->canvas, &DrawingCanvasWidget::modeChanged,
            this, &ImageEditorDialog::modeChanged);
    modeChanged(m_ui->canvas->mode());

    connect(m_ui->canvas, &DrawingCanvasWidget::zoomChanged,
            this, &ImageEditorDialog::zoomChanged);
    zoomChanged(m_ui->canvas->zoom());

    updateSize(pixmap);

    for (QLabel *label : { &m_posLabel, &m_zoomLabel, &m_sizeLabel, &m_memoryLabel })
    {
        label->setFrameShape(QFrame::Panel);
        label->setFrameShadow(QFrame::Sunken);
        m_ui->statusBar->addPermanentWidget(label);
    }
}

ImageEditorDialog::~ImageEditorDialog() = default;

QPixmap ImageEditorDialog::pixmap() const
{
    return m_ui->canvas->pixmap();
}

const QImage &ImageEditorDialog::image() const
{
    return m_ui->canvas->image();
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

void ImageEditorDialog::newFile()
{
    if (m_unsavedChanges)
    {
        if (QMessageBox::warning(
            this,
            tr("The Image has been modified."),
            tr("Do you really want to start new?"),
            QMessageBox::Ok | QMessageBox::Cancel
        ) != QMessageBox::Ok)
            return;
    }

    QPixmap pixmap{m_ui->canvas->pixmap().size()};
    pixmap.fill(Qt::transparent);
    m_ui->canvas->setPixmap(pixmap);
    changed();
}

void ImageEditorDialog::openFile()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::saveAsPng()
{
    const auto &image = m_ui->canvas->image();
    if (image.isNull())
    {
        QMessageBox::warning(this, tr("Invalid sprite!"), tr("The sprite you tried to save is invalid!"));
        return;
    }

    saveImage(this, image);
}

void ImageEditorDialog::previousSprite()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::nextSprite()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::undo()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::redo()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::eraseToLeftColor()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::delete_()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::cut()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::copy()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::paste()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::pasteFromFile()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::selectAll()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void ImageEditorDialog::zoomIn()
{
    m_ui->canvas->setZoom(m_ui->canvas->zoom() * 2.f);
}

void ImageEditorDialog::resetZoom()
{
    m_ui->canvas->setZoom(1.f);
}

void ImageEditorDialog::zoomOut()
{
    m_ui->canvas->setZoom(m_ui->canvas->zoom() / 2.f);
}

void ImageEditorDialog::transparentBackgroundSettings()
{
    TransparentBackgroundSettingsDialog dialog{m_settings, this};
    if (dialog.exec() == QDialog::Accepted)
    {
        dialog.save(m_settings);
        m_ui->canvas->setTransparentBackgroundPattern(m_settings.transparentBackgroundPattern());
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

void ImageEditorDialog::selectFont()
{
    QFontDialog dialog{m_ui->pushButtonSelectFont->font(), this};
    if (dialog.exec() == QDialog::Accepted)
        m_ui->pushButtonSelectFont->setFont(dialog.selectedFont());
}

void ImageEditorDialog::modeChanged(DrawingCanvasWidget::Mode mode)
{
    QString msg;
    switch (mode)
    {
    case DrawingCanvasWidget::Mode::Draw: msg = tr("Paint with the mouse. <Shift> for hor/vert"); break;
    case DrawingCanvasWidget::Mode::Spray: msg = tr("Spray with the mouse. <Shift> for hor/vert"); break;
    case DrawingCanvasWidget::Mode::Erase: msg = tr("Erase with the mouse (based on transparency)"); break;
    case DrawingCanvasWidget::Mode::Pick: msg = tr("Pick a color with the mouse"); break;
    case DrawingCanvasWidget::Mode::Line: msg = tr("Draw a line. <Shift> for diagonal"); break;
    case DrawingCanvasWidget::Mode::Polygon: msg = tr("Draw a polygon. <Esc> to end"); break;
    case DrawingCanvasWidget::Mode::Rectangle: msg = tr("Draw a rectangle. <Shift> for square"); break;
    case DrawingCanvasWidget::Mode::Ellipse: msg = tr("Draw an ellipse. <Shift> for circle"); break;
    case DrawingCanvasWidget::Mode::RoundedRectangle: msg = tr("Draw a rounded rectangle. <Shift> for square"); break;
    case DrawingCanvasWidget::Mode::SelectRegion: msg = tr("Select a region with the mouse"); break;
    case DrawingCanvasWidget::Mode::SelectWand: msg = tr("Click to select with magic wand"); break;
    case DrawingCanvasWidget::Mode::SelectSpray: msg = tr("Spray selection with the mouse"); break;
    case DrawingCanvasWidget::Mode::Text: msg = tr("Click the mouse to start a new text"); break;
    case DrawingCanvasWidget::Mode::Fill: msg = tr("Flood filling a region"); break;
    case DrawingCanvasWidget::Mode::Replace: msg = tr("Click on a pixel whose color to change"); break;
    };

    if (msg.isNull())
        qWarning() << "unknown mode" << mode;

    m_ui->statusBar->showMessage(msg);
}

void ImageEditorDialog::cursorMoved(const QPoint &pos)
{
    m_posLabel.setText(tr("(%0,%1)").arg(pos.x()).arg(pos.y()));
}

void ImageEditorDialog::zoomChanged(float zoom)
{
    m_zoomLabel.setText(tr("Zoom: %0%").arg(zoom * 100.f, 0, 'f', 0));
}

void ImageEditorDialog::updateTitle()
{
    setWindowTitle(tr("%0%1")
                       .arg(m_title)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}

void ImageEditorDialog::updateSize(const QPixmap &pixmap)
{
    m_sizeLabel.setText(tr("Size: %0 x %1").arg(pixmap.width()).arg(pixmap.height()));
    m_memoryLabel.setText(tr("Memory: %0 KB").arg(pixmap.width() * pixmap.height() * 4 / 1000));
}
