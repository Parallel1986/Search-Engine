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
    QDir path(config_path);
    if (path.isReadable())
        converter->setConfigPath(config_path.toStdString());
    else
    {
        engine_status |= ConverterStatus::CONFIG_MISSED;
    }
    fillFields();
}

void ConfigWindow::fillFields()
{

    if (converter->isInitialized())
    {
        ui->max_response_spin->setValue(converter->GetResponsesLimit());
        ui->engine_name_edit->setText(QString(converter->getEngineName().c_str()));
        ui->engine_ver_edit->setText(QString(converter->getEngineVersion().c_str()));
        if (!converter->getFilesPaths().empty())
        {
//          ui->file_list->clear();
            file_list.clear();
search_files_list->setStringList(QStringList());
            for (auto& file : converter->getFilesPaths())
            {
//             search_files.append(file.c_str());
               file_list << file.c_str();
//             ui->file_list->appendPlainText(search_files.back());
            }
            search_files_list->setStringList(file_list);
            ui->file_listView->setModel(search_files_list);
        }
        else
        {
            search_files_list->setStringList(QStringList());
            ui->file_listView->setModel(search_files_list);
        }
    }
    else
    {
        ui->max_response_spin->setValue(5);
        ui->engine_name_edit->setText(engine_name);
        ui->engine_ver_edit->setText(engine_ver);
        search_files_list->setStringList(QStringList());
        ui->file_listView->setModel(search_files_list);
    }
}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}

char ConfigWindow::getEngineStatus()
{
    return engine_status;
}

void ConfigWindow::clickedOk()
{
    ConfigList settings;
    settings.enegine_name = ui->engine_name_edit->text().toStdString();
    settings.engine_ver = ui->engine_ver_edit->text().toStdString();
    settings.max_responses = ui->max_response_spin->value();
    for (auto& file : file_list)
    {
        settings.files.push_back(file.toStdString());
    }
    converter->putConfig(settings, QDir::currentPath().toStdString());
    emit configPathChanged(config_path);
    this->close();
}
void ConfigWindow::clickedCancel()
{
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
    file_list = search_files_list->stringList();
}
void ConfigWindow::clickedAdd()
{
    QFileDialog* dlg = new QFileDialog();
    dlg->setNameFilter(tr("TXT file (*.txt)"));
    if (dlg->exec())
    {
        auto new_file = dlg->selectedFiles();
        file_list.append(new_file[0]);
        auto row = search_files_list->rowCount();
        search_files_list->insertRows(row,1,QModelIndex());
        search_files_list->setData(search_files_list->index(row),new_file[0],Qt::EditRole);
    }
    dlg->close();
    delete dlg;
}
void ConfigWindow::maxResponsesChanged()
{}
void ConfigWindow::engineVerChanged()
{}
void ConfigWindow::engineNameChanged()
{}

//void temp()
//{
//    QMessageBox* error_message = new QMessageBox();
//    error_message->setText("Missing config.json");
//    error_message->setInformativeText("Couldn`t find config.json, do you wish to configure new one?");
//    error_message->setStandardButtons(QMessageBox::Close|QMessageBox::Open|QMessageBox::Yes);
//    error_message->setDefaultButton(QMessageBox::Yes);
//    int result = error_message->exec();
//    switch (result) {
//    case QMessageBox::Close:
//        this->close();
//        this->parentWidget()->close();
//        break;
//    case QMessageBox::Open:
//        emit openConfigRequest();
//        this->close();
//        break;
//    case QMessageBox::Yes:
//        this->show();
//        break;
//    default:
//        break;
//    }
//}
