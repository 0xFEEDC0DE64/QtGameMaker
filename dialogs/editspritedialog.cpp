#include "editspritedialog.h"
#include "ui_editspritedialog.h"

#include <QMessageBox>

#include "projectcontainer.h"
#include "spritesmodel.h"
#include "createspritedialog.h"
#include "imageeditordialog.h"

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

    m_ui->listView->setModel(m_model.get());

    connect(m_ui->actionNew, &QAction::triggered, this, &EditSpriteDialog::newSprite);

    connect(m_ui->listView, &QListView::doubleClicked, this, &EditSpriteDialog::doubleClicked);
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
        pixmap.fill(Qt::white);

        m_model->beginResetModel();
        m_pixmaps = std::vector<QPixmap> { std::move(pixmap) };
        m_model->endResetModel();
        qDebug() << m_model->rowCount({});
        changed();
    }
}

void EditSpriteDialog::doubleClicked(const QModelIndex &index)
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
