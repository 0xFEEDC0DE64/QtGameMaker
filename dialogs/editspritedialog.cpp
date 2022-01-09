#include "editspritedialog.h"
#include "ui_editspritedialog.h"

#include "projectcontainer.h"
#include "spritesmodel.h"
#include "createspritedialog.h"
#include "imageeditordialog.h"

EditSpriteDialog::EditSpriteDialog(Sprite &sprite, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::EditSpriteDialog>()},
    m_sprite{sprite},
    m_model{std::make_unique<SpritesModel>(this)}
{
    m_ui->setupUi(this);

    setWindowFlags(windowFlags()
                    & ~Qt::Dialog
                   | Qt::Window
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint
                   | Qt::WindowCloseButtonHint);

    setWindowTitle(tr("Sprite editor - %0").arg(m_sprite.name));

    m_ui->actionNew->setShortcut(QKeySequence::New);
    m_ui->actionCreateFromFile->setShortcut(QKeySequence::Open);
    m_ui->actionSaveAsPngFile->setShortcut(QKeySequence::Save);

    m_ui->listView->setModel(m_model.get());

    connect(m_ui->actionNew, &QAction::triggered, this, &EditSpriteDialog::newSprite);

    connect(m_ui->listView, &QListView::doubleClicked, this, &EditSpriteDialog::doubleClicked);
}

EditSpriteDialog::~EditSpriteDialog() = default;

void EditSpriteDialog::newSprite()
{
    CreateSpriteDialog dialog{this};
    if (dialog.exec() == QDialog::Accepted)
    {

    }
}

void EditSpriteDialog::doubleClicked(const QModelIndex &index)
{
    ImageEditorDialog{this}.exec();
}
