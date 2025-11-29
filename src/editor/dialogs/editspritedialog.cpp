#include "editspritedialog.h"
#include "ui_editspritedialog.h"

#include <QMessageBox>
#include <QDebug>

#include "projectcontainer.h"
#include "models/spritesmodel.h"
#include "dialogs/transparentbackgroundsettingsdialog.h"
#include "createspritedialog.h"
#include "imageeditordialog.h"
#include "imagehelpers.h"

EditSpriteDialog::EditSpriteDialog(const std::vector<QPixmap> &pixmaps, const QString &spriteName, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::EditSpriteDialog>()},
    m_pixmaps{pixmaps},
    m_spriteName{spriteName},
    m_model{std::make_unique<SpritesModel>(m_pixmaps, this)}
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
    m_ui->actionCreateFromFile->setShortcut(QKeySequence::Open);
    m_ui->actionSaveAsPngFile->setShortcut(QKeySequence::Save);
    m_ui->actionUndo->setShortcut(QKeySequence::Undo);
    m_ui->actionRedo->setShortcut(QKeySequence::Redo);
    m_ui->actionCut->setShortcut(QKeySequence::Cut);
    m_ui->actionCopy->setShortcut(QKeySequence::Copy);
    m_ui->actionPaste->setShortcut(QKeySequence::Paste);
    m_ui->actionDelete->setShortcut(QKeySequence::Delete);

    m_ui->listView->setModel(m_model.get());

    connect(m_ui->actionNew, &QAction::triggered,
            this, &EditSpriteDialog::newSprite);
    connect(m_ui->actionCreateFromFile, &QAction::triggered,
            this, &EditSpriteDialog::createFromFile);
    connect(m_ui->actionAddFromFile, &QAction::triggered,
            this, &EditSpriteDialog::addFromFile);
    connect(m_ui->actionSaveAsPngFile, &QAction::triggered,
            this, &EditSpriteDialog::saveAsPng);
    connect(m_ui->actionCreateFromStrip, &QAction::triggered,
            this, &EditSpriteDialog::createFromStrip);
    connect(m_ui->actionAddFromStrip, &QAction::triggered,
            this, &EditSpriteDialog::addFromStrip);
    connect(m_ui->actionSetTransparencyBackground, &QAction::triggered,
            this, &EditSpriteDialog::transparentBackgroundSettings);

    connect(m_ui->listView, &QAbstractItemView::activated,
            this, &EditSpriteDialog::activated);
    connect(m_ui->listView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &EditSpriteDialog::currentChanged);
    currentChanged(m_ui->listView->currentIndex());
}

EditSpriteDialog::~EditSpriteDialog() = default;

void EditSpriteDialog::accept()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    // TODO

    QDialog::accept();
}

void EditSpriteDialog::reject()
{
    if (!m_unsavedChanges)
    {
        QDialog::reject();
        return;
    }

    const auto result = QMessageBox::warning(
        this,
        tr("The Sprite has been modified."),
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

void EditSpriteDialog::newSprite()
{
    CreateSpriteDialog dialog{this};
    if (dialog.exec() == QDialog::Accepted)
    {
        QPixmap pixmap{dialog.spriteSize()};
        pixmap.fill(Qt::transparent);

        m_model->beginResetModel();
        m_pixmaps = std::vector<QPixmap> { std::move(pixmap) };
        m_model->endResetModel();

        changed();
    }
}

void EditSpriteDialog::createFromFile()
{
    auto pixmap = loadPixmap(this);
    if (pixmap.isNull())
        return;

    m_model->beginResetModel();
    m_pixmaps = std::vector<QPixmap> { std::move(pixmap) };
    m_model->endResetModel();

    changed();
}

void EditSpriteDialog::addFromFile()
{
    auto pixmap = loadPixmap(this);
    if (pixmap.isNull())
        return;

    if (!m_pixmaps.empty() && m_pixmaps.front().size() != pixmap.size())
    {
        QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
        return;
    }
    else
    {
        m_model->beginInsertRows({}, m_pixmaps.size(), m_pixmaps.size());
        m_pixmaps.emplace_back(std::move(pixmap));
        m_model->endInsertRows();
    }
}

void EditSpriteDialog::saveAsPng()
{
    auto index = m_ui->listView->currentIndex();
    if (!index.isValid())
        return;

    if (index.row() < 0 || (size_t)index.row() >= m_pixmaps.size())
    {
        qWarning() << "invalid row" << index.row();
        return;
    }

    const auto &pixmap = m_pixmaps[index.row()];
    if (pixmap.isNull())
    {
        QMessageBox::warning(this, tr("Invalid sprite!"), tr("The sprite you tried to save is invalid!"));
        return;
    }

    saveImage(this, pixmap.toImage());
}

void EditSpriteDialog::createFromStrip()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void EditSpriteDialog::addFromStrip()
{
    QMessageBox::warning(this, tr("Not yet implemented"), tr("Not yet implemented"));
}

void EditSpriteDialog::transparentBackgroundSettings()
{
    TransparentBackgroundSettingsDialog dialog{this};
    if (dialog.exec() == QDialog::Accepted)
    {
        // TODO apply
    }
}

void EditSpriteDialog::activated(const QModelIndex &index)
{
    if (!index.isValid())
    {
        qWarning() << "unexpected invalid index";
        return;
    }

    ImageEditorDialog dialog{m_model->pixmap(index), tr("Image Editor: %0").arg(m_spriteName), this};
    if (dialog.exec() == QDialog::Accepted)
    {
        m_pixmaps[index.row()] = dialog.pixmap();
        emit m_model->dataChanged(index, index, {Qt::DecorationRole});

        changed();
    }
}

void EditSpriteDialog::currentChanged(const QModelIndex &index)
{
    m_ui->actionSaveAsPngFile->setEnabled(index.isValid());
}

void EditSpriteDialog::changed()
{
    if (!m_unsavedChanges)
    {
        m_unsavedChanges = true;
        updateTitle();
    }
}

void EditSpriteDialog::updateTitle()
{
    setWindowTitle(tr("Sprite editor - %0%1")
                       .arg(m_spriteName)
                       .arg(m_unsavedChanges ? tr("*") : QString{})
                   );
}
