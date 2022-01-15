#pragma once

#include "codeeditordialog.h"

class GenericCodeEditorDialog : public CodeEditorDialog
{
    Q_OBJECT

public:
    explicit GenericCodeEditorDialog(const QString &title, QWidget *parent = nullptr);
};
