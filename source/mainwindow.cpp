#include "mainwindow.h"
#include "ui_main_window.cpp"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent, EngineCore* search_engine)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , core(search_engine)
{
    ui->setupUi(this);
    ui->save_button->setEnabled(false);
    r_dialog = new RequestAddDialog(this);
    request_list_model = new QStringListModel;
    files_list_model = new QStringListModel;
    connect(r_dialog,&RequestAddDialog::newRequest, this, &MainWindow::modifyRequests);
    checkUIMarks(core->getEngineStatus());
    ui->results_tree_widget->setColumnCount(1);
}

MainWindow::~MainWindow()
{
    if (request_list_model != nullptr)
        delete request_list_model;
    if (ui != nullptr)
        delete ui;
    if (r_dialog != nullptr)
        delete r_dialog;
}

void MainWindow::openConfigPathDlg()
{
    QFileDialog* dlg = new QFileDialog();
    dlg->setNameFilter(tr("JSON file (*.json)"));
    if (dlg->exec())
    {
        auto config_file = dlg->selectedFiles();
        ui->config_path_edit->setText(config_file[0]);
        core->setConfigPath(config_file[0]);
    }
    dlg->close();
    delete dlg;
    checkUIMarks(core->getEngineStatus());
}

void MainWindow::openRequestsPathDlg()
{
    QFileDialog* dlg = new QFileDialog(this);
    dlg->setNameFilter(tr("JSON file (*.json)"));
    if (dlg->exec())
    {
        auto requests_file = dlg->selectedFiles();
        core->setRequestsPath(requests_file[0]);
        checkUIMarks(core->getEngineStatus());
    }
    delete dlg;
}

void MainWindow::openAnswersPathDlg()
{
    QFileDialog* dlg = new QFileDialog(this);
    dlg->setNameFilter(tr("JSON file (*.json)"));
    if (dlg->exec())
    {
        auto answers_file = dlg->selectedFiles();
        core->setAnswersPath(answers_file[0]);
    }
    delete dlg;
}

void MainWindow::addRequest()
{
    r_dialog->show();
}

void MainWindow::deleteRequest()
{
    QItemSelectionModel* selection = ui->requests_line_list->selectionModel();
    if (!selection->selectedIndexes().empty())
    {
        QModelIndexList index = selection->selectedIndexes();
        emit requestIsDeleted(index[0].data().toString());
    }
}

void MainWindow::modifyRequests(QString new_request)
{
    emit requestIsAdded(new_request);
}

void MainWindow::setConfigPath(QString new_path)
{
    ui->config_path_edit->setText(new_path);
}

void MainWindow::setRequestsPath(QString new_path)
{
    ui->requests_path_edit->setText(new_path);
}

void MainWindow::fillConfigFields(ConfigList configs)
{
    ui->engine_name_edit->setText(configs.enegine_name);
    ui->engine_version_edit->setText(configs.engine_version);
    ui->max_response_spin->setValue(configs.max_responses);
    files_list_model->setStringList(configs.files);
    ui->file_view->setModel(files_list_model);
    if (configs.files.isEmpty())
    {
        ui->no_files_mark->show();
        config_ready = false;
    }
    else
    {
        ui->no_files_mark->hide();
        config_ready = true;
    }
    char status = core->getEngineStatus();
    checkUIMarks(status);
    readinessCheck();
}

void MainWindow::fillRequestsFields(QStringList requests)
{    
    request_list_model->setStringList(requests);
    ui->requests_line_list->setModel(request_list_model);
    if (!request_list_model->stringList().isEmpty())
    {
        ui->no_requests_mark_2->hide();
        requests_ready = true;
    }
    else
    {
        ui->no_requests_mark_2->show();
        requests_ready = false;
    }
    checkUIMarks(core->getEngineStatus());
    readinessCheck();
}

void MainWindow::setAnswersPath(QString new_path)
{
    ui->answers_path_edit->setText(new_path);
}

void MainWindow::readinessCheck()
{
    ui->search_button->setEnabled(requests_ready&&config_ready);
}

void MainWindow::configErrorDlg(char engine_status)
{    
    switch (engine_status)
    {
        case ConverterStatus::CONFIG_MISSED:
        case ConverterStatus::CONFIG_FIELD_MISSED:
        {
            ui->no_config_mark->show();
            QMessageBox* error_message = new QMessageBox();
            error_message->setText(engine_status&ConverterStatus::CONFIG_MISSED ? "Missing configurations' file"
                                                                    :"Corrupted configurations' file");
            error_message->setInformativeText("Couldn`t find configurations, do you wish to"
                                              " open existing file?");

            error_message->setStandardButtons(QMessageBox::Open|QMessageBox::Close);
            error_message->setDefaultButton(QMessageBox::Open);
            int result = error_message->exec();
            delete error_message;
            switch (result)
            {
            case QMessageBox::Close:
            {
                ui->no_config_mark->show();
                config_ready = false;
                ui->config_path_edit->clear();
                break;
            }
            case QMessageBox::Open:
            {                
                openConfigPathDlg();
                break;
            }
            //Should not get here
            default:
                break;
            }            
        }

        case ConverterStatus::SEARCH_FILES_MISSED:
        {
            QMessageBox* error_message = new QMessageBox();
            error_message->setText("Files for searching missing!");
            error_message->setInformativeText("Couldn`t find files for search! Add at least one file");
            error_message->setStandardButtons(QMessageBox::Ok);
            error_message->setDefaultButton(QMessageBox::Ok);
            error_message->exec();
            delete error_message;
            ui->no_files_mark->show();
            config_ready = false;
            break;
        }
        //Should not get here
        default:
            break;
    }
}

void MainWindow::fillSearchFilesFields(QStringList files)
{
    files_list_model->setStringList(QStringList());
    files_list_model->setStringList(files);
    ui->file_view->setModel(files_list_model);
}

void MainWindow::search()
{
    core->search();
}

void MainWindow::showResult(QList<QList<RelativeIndex>> search_result,
                            const FileIDTable* file_id_table,
                            const RequestIDTable* requests_id_table)
{
    int rec_id = 0;
    ui->results_tree_widget->clear();
    QTreeWidgetItem* main_item= new QTreeWidgetItem(ui->results_tree_widget);
    main_item->setText(0,"Search results");
    for (auto& result : search_result)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(main_item);
        item->setText(0,QString("request: " + requests_id_table->frame[rec_id].request));
        QTreeWidgetItem* is_result = new QTreeWidgetItem(item);

        is_result->setText(0,result.empty()? "result: false"
                                           : "result: true");
        if (!result.empty())
        {
            for (auto& line : result)
            {
                QTreeWidgetItem* record = new QTreeWidgetItem(is_result);
                record->setSizeHint(0,QSize(150,15));
                QString error;
                if (file_id_table->frame[line.doc_id].err == FileErrorType::NotAFile)
                {
                    error = "Not a file! Error";
                }
                else if (file_id_table->frame[line.doc_id].err == FileErrorType::FileNotExistError)
                    error = "File not exist!";
                else if (file_id_table->frame[line.doc_id].err == FileErrorType::ReadFileError)
                    error = "Error while reading a file!";

                record->setText(0,((file_id_table->frame[line.doc_id].err == FileErrorType::NotError)? "rank: " + QString::number(line.rank):error) + "\t document: "+ file_id_table->frame[line.doc_id].file_path);
            }
        }
        ++rec_id;
    }
    ui->results_tree_widget->setItemWidget(main_item,0,nullptr);
    ui->save_button->setEnabled(true);
    ui->save_as_text_button->setEnabled(true);

    QMessageBox* done = new QMessageBox();
    done->setText("Search completed!");
    done->setStandardButtons(QMessageBox::Ok);
    done->setDefaultButton(QMessageBox::Ok);
    done->exec();
    delete done;
}

void MainWindow::saveResult()
{
    core->saveResult();
        QMessageBox* done = new QMessageBox();
    done->setText("Successfully saved!");
    done->setInformativeText("Result is saved at: " + ui->answers_path_edit->text());
    done->setStandardButtons(QMessageBox::Ok);
    done->setDefaultButton(QMessageBox::Ok);
    done->exec();
    delete done;
}

void MainWindow::checkUIMarks(char engine_status)
{
    if (engine_status&ConverterStatus::REQUESTS_EMPTY)
    {
        ui->no_requests_path_mark->show();
        ui->no_requests_mark_2->show();
    }
    else if (engine_status&ConverterStatus::REQUESTS_MISSED)
    {
        ui->no_requests_path_mark->show();
        ui->no_requests_mark_2->show();
    }
    else
    {
        ui->no_requests_mark_2->hide();
        ui->no_requests_path_mark->hide();
    }

    if (engine_status&ConverterStatus::CONFIG_MISSED
        ||engine_status&ConverterStatus::CONFIG_FIELD_MISSED)
    {
        ui->no_files_mark->show();
        ui->no_config_mark->show();
    }
    else if (engine_status&ConverterStatus::SEARCH_FILES_MISSED)
    {
        ui->no_files_mark->show();
    }
    else
    {
        ui->no_config_mark->hide();
        ui->no_files_mark->hide();
    }
}

void MainWindow::requestsErrorDlg(char engine_status)
{
    checkUIMarks(engine_status);
    QMessageBox* error_message = new QMessageBox();
    error_message->setText(engine_status&ConverterStatus::REQUESTS_MISSED?"requesets.json is missing!"
                                                            :"requests.json is corrupted!");
    error_message->setInformativeText("Please add at least one request");
    error_message->setStandardButtons(QMessageBox::Ok);
    error_message->setDefaultButton(QMessageBox::Ok);
    error_message->exec();
    delete error_message;
}

void MainWindow::changeMode(int new_mode)
{
    switch (new_mode) {
    case EngineMode_MW::STAND:
        core->setMode(EngineMode::STANDARD);
        checkUIMarks(core->getEngineStatus());
        ui->config_path_button->setEnabled(true);
        ui->requests_path_button->setEnabled(true);
        ui->generate_config_button->setEnabled(true);
        break;
    case EngineMode_MW::MAN:
        core->setMode(EngineMode::MANUAL);
        ui->no_config_mark->hide();
        ui->generate_config_button->setEnabled(false);
        ui->no_requests_path_mark->setEnabled(false);
        ui->config_path_button->setEnabled(false);
        ui->requests_path_button->setEnabled(false);
    case EngineMode_MW::NO_CONF:
        core->setMode(EngineMode::NO_CONFIG);
        checkUIMarks(core->getEngineStatus());
        ui->requests_path_button->setEnabled(true);
        ui->no_config_mark->hide();
        ui->generate_config_button->setEnabled(false);
        ui->config_path_button->setEnabled(false);
    case EngineMode_MW::NO_REQ:
        core->setMode(EngineMode::NO_REQUESTS);
        checkUIMarks(core->getEngineStatus());
        ui->config_path_button->setEnabled(true);
        ui->generate_config_button->setEnabled(true);
        ui->no_requests_path_mark->setEnabled(false);
        ui->requests_path_button->setEnabled(false);
    default:
        break;
    }
}

void MainWindow::saveConfig()
{    
    emit configIsSaved(ConfigList (
    {
        ui->engine_name_edit->text(),
        ui->engine_version_edit->text(),
        ui->max_response_spin->value(),
        files_list_model->stringList()
    }));
}

void MainWindow::addFileDlg()
{
    QFileDialog* dlg = new QFileDialog();
    dlg->setNameFilter(tr("TXT file (*.txt)"));
    dlg->setFileMode(QFileDialog::ExistingFiles);
    if (dlg->exec())
    {
        auto new_file = dlg->selectedFiles();
        for (auto file = new_file.begin(); file != new_file.end();++file)
        {
            emit fileIsAdded(*file);
            auto row = files_list_model->rowCount();
            files_list_model->insertRows(row,1,QModelIndex());
            files_list_model->setData(files_list_model->index(row),*file,Qt::EditRole);
        }
    }
    dlg->close();
    delete dlg;
}

void MainWindow::deleteFile()
{
    QItemSelectionModel* selection = ui->file_view->selectionModel();
    if (!selection->selectedIndexes().empty())
    {
        QModelIndexList index = selection->selectedIndexes();
        emit fileIsDeleted(index[0].data().toString());
    }
}

void MainWindow::reloadFiles()
{
    files_list_model->setStringList(core->getFiles());
}

void MainWindow::saveResultAsText()
{
    core->saveResultAsText();
        QMessageBox* done = new QMessageBox();
    done->setText("Search completed!");
    QString file_path = ui->answers_path_edit->text();
    file_path.chop(4);
    done->setInformativeText("Result is saved as text at: " + file_path + "txt");
    done->setStandardButtons(QMessageBox::Ok);
    done->setDefaultButton(QMessageBox::Ok);
    done->exec();
    delete done;
}

void MainWindow::generateConfig()
{
    core->generateConfigFile(files_list_model->stringList(),ui->max_response_spin->value());
}

void MainWindow::showError(FileError err)
{
    QMessageBox* error_message = new QMessageBox();
    error_message->setText(Error::writeErrorType(err.getExceptionType(), err.getExceptionSource()));
    error_message->setInformativeText(err.getAdditionalData());
    error_message->setStandardButtons(QMessageBox::Ok);
    error_message->setDefaultButton(QMessageBox::Ok);
    error_message->exec();
    delete error_message;
}

void MainWindow::showErrorList(QList<FileError> error_list)
{
    QMessageBox* error_message = new QMessageBox();
    error_message->setText("Some errors had been aquired!");
    QString info;
    for (auto& error:error_list)
    {
        info += Error::writeErrorType(error.getExceptionType(),error.getExceptionSource()) + "\n";
    }
    error_message->setInformativeText(info);
    error_message->setStandardButtons(QMessageBox::Ok);
    error_message->setDefaultButton(QMessageBox::Ok);
    error_message->exec();
    delete error_message;

}
