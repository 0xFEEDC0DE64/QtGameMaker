#pragma once

#include <QDialog>

#include <memory>

namespace Ui { class UserDefinedConstantsDialog; }
struct ProjectContainer;
class ConstantsModel;

class UserDefinedConstantsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDefinedConstantsDialog(ProjectContainer &project, QWidget *parent = nullptr);
    ~UserDefinedConstantsDialog();

private slots:
    void insertConstant();
    void addConstant();
    void deleteConstant();
    void clearConstants();
    void moveConstantUp();
    void moveConstantDown();
    void sortConstants();
    void loadConstant();
    void saveConstant();

private:
    const std::unique_ptr<Ui::UserDefinedConstantsDialog> m_ui;

    ProjectContainer &m_project;

    const std::unique_ptr<ConstantsModel> m_model;
};
