#pragma once

#include <QMessageBox>

#include "projectcontainer.h"
#include "widgets/objectselectorwidget.h"

template<typename Ui>
void loadActionTarget(Ui &ui, const AppliesTo &target)
{
    ui.radioButtonSelf->setChecked(std::holds_alternative<AppliesToSelf>(target));
    ui.radioButtonOther->setChecked(std::holds_alternative<AppliesToOther>(target));
    const auto *object = std::get_if<AppliesToObject>(&target);
    ui.radioButtonObject->setChecked(object != nullptr);
    if (object)
        ui.widgetObject->setObject(object->objectName);
    else
        ui.widgetObject->clearObject();
    ui.widgetObject->setVisible(object != nullptr);
}

template<typename Ui>
bool saveActionTarget(const Ui &ui, AppliesTo &target, QWidget *parent)
{
    if (ui.radioButtonSelf->isChecked())
        target = AppliesToSelf{};
    else if (ui.radioButtonOther->isChecked())
        target = AppliesToOther{};
    else if (ui.radioButtonObject->isChecked() && !ui.widgetObject->objectName().isEmpty())
        target = AppliesToObject{ui.widgetObject->objectName()};
    else
    {
        QMessageBox::warning(parent, QObject::tr("Select an object"),
                             QObject::tr("Select an object for this action."));
        return false;
    }
    return true;
}
