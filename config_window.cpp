#include "config_window.h"
#include "./ui/ui_config_window.cpp"
#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

class MainWindow;

ConfigWindow::ConfigWindow(QWidget *parent, EngineCore *core)
    : QMainWindow(parent)
    , ui(new Ui::Config_window)
    , core(core)
{
    ui->setupUi(this);
    this->core = core;
    this->hide();
    search_files_list = new QStringListModel();
    core->initialize();
    fillSettings();
    fillFields();
}

//Loading text fields
void ConfigWindow::fillFields()
{
    ui->max_response_spin->setValue(settings.max_responses);
    ui->engine_name_edit->setText(settings.enegine_name);
    ui->engine_ver_edit->setText(settings.engine_ver);
    search_files_list->setStringList(QStringList());    
    search_list.clear();
    (bad_files.size()>1)?search_list = settings.files + bad_files:search_list = settings.files;
    search_files_list->setStringList(search_list);
    ui->file_listView->setModel(search_files_list);
    emit ready();
}

//Loading configurations
void ConfigWindow::fillSettings()
{
    settings.enegine_name = core->getEngineName();
    settings.engine_ver = core->getEngineVersion();
    settings.max_responses = core->getResponsesLimit();
    settings.files.clear();
    bad_files.clear();
    bad_files.append("    Error files! Could not open:");
    for (auto& file : core->getFilesPaths())
    {
        QDir check(file);
        if (check.isReadable())
            settings.files.append(file);
        else
            bad_files.append(file);
    }

}

ConfigWindow::~ConfigWindow() {delete ui;}

//Ok button is pressed
void ConfigWindow::clickedOk()
{
    settings.enegine_name = ui->engine_name_edit->text();
    settings.engine_ver = ui->engine_ver_edit->text();
    settings.max_responses = ui->max_response_spin->value();
    settings.files.clear();
    for (auto& line : search_files_list->stringList())
    {
        if (line != "    Error files! Could not open:")
            settings.files.append(line);
        else
            break;
    }
    core->putConfig(settings, config_path);
    core->initialize();
    emit configPathChanged(config_path);
    if (settings.files.empty())
    {
        ((MainWindow*)parent())->configError();
    }
    else
        this->close();
}

//Cancel button is pressed
void ConfigWindow::clickedCancel()
{
    emit noConfigChanges();
    this->close();
}

//Remove button is pressed
void ConfigWindow::clickedRemove()
{
    QItemSelectionModel* selection = ui->file_listView->selectionModel();
    if (!selection->selectedIndexes().empty())
    {
        QModelIndexList index = selection->selectedIndexes();
        int row = index[0].row();
        search_files_list->removeRows(row,1,QModelIndex());
        ui->file_listView->setModel(search_files_list);
    }
    settings.files = search_files_list->stringList();
}

//Add button is pressed
void ConfigWindow::clickedAdd()
{
    QFileDialog* dlg = new QFileDialog();
    dlg->setNameFilter(tr("TXT file (*.txt)"));
    if (dlg->exec())
    {
        auto new_file = dlg->selectedFiles();        
        settings.files.append(new_file[0]);
        auto row = search_files_list->rowCount();
        search_files_list->insertRows(row,1,QModelIndex());
        search_files_list->setData(search_files_list->index(row),new_file[0],Qt::EditRole);
    }
    dlg->close();
    delete dlg;
}
