#include "config_window.h"
#include "./ui_config_window.h"
#include <QFileDialog>
#include <QMessageBox>

ConfigWindow::ConfigWindow(QWidget *parent, ConverterJSON *converter)
    : QMainWindow(parent)
    , ui(new Ui::Config_window)
{
    ui->setupUi(this);
    this->converter = converter;
    this->hide();
    search_files_list = new QStringListModel();
    converter->initialize();
    fillSettings();
    fillFields();
}

void ConfigWindow::fillFields()
{
    ui->max_response_spin->setValue(settings.max_responses);
    ui->engine_name_edit->setText(settings.enegine_name);
    ui->engine_ver_edit->setText(settings.engine_ver);
    search_files_list->setStringList(settings.files);
    ui->file_listView->setModel(search_files_list);
    emit ready();
}

void ConfigWindow::fillSettings()
{
    settings.enegine_name = converter->getEngineName();
    settings.engine_ver = converter->getEngineVersion();
    settings.max_responses = converter->GetResponsesLimit();
    settings.files = converter->getFilesPaths();
}

ConfigWindow::~ConfigWindow()
{delete ui;}

void ConfigWindow::clickedOk()
{
//    fillSettings();
    settings.enegine_name = ui->engine_name_edit->text();
    settings.engine_ver = ui->engine_ver_edit->text();
    settings.max_responses = ui->max_response_spin->value();
    settings.files.clear();
    for (auto& line : search_files_list->stringList())
    {
        if (line != "Error files! Could not open!")
            settings.files.append(line);
    }

    converter->putConfig(settings, QDir::currentPath());
    emit configPathChanged(config_path);
    this->close();
}
void ConfigWindow::clickedCancel()
{
    emit noConfigChanges();
    this->close();
}
void ConfigWindow::clickedRemove()
{
    QItemSelectionModel* selection = ui->file_listView->selectionModel();   //Выделенный элемент
    if (!selection->selectedIndexes().empty())                  //Проверяем что выделение не пустое
    {
        QModelIndexList index = selection->selectedIndexes();   //Список выбраных строк
        int row = index[0].row();                               //Первая выбранная строка
        search_files_list->removeRows(row,1,QModelIndex());     //Удаляем строку row
        ui->file_listView->setModel(search_files_list);         //Переназначаем список строк
    }
    settings.files = search_files_list->stringList();
}
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

//void ConfigWindow::maxResponsesChanged(int new_max)
//{settings.max_responses = new_max;}

//void ConfigWindow::engineVerChanged(QString new_ver)
//{settings.engine_ver = new_ver;}

//void ConfigWindow::engineNameChanged(QString new_name)
//{settings.enegine_name = new_name;}


